//------------------------------------------------------------------------------
//  hbao_cs.fx
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"

vec2 UVToViewA = vec2(0.0f, 0.0f);
vec2 UVToViewB = vec2(0.0f, 0.0f);
vec2 AOResolution = vec2(0.0f, 0.0f);
vec2 InvAOResolution = vec2(0.0f, 0.0f);
float TanAngleBias = 0.0f;
float Strength = 0.0f;
float R2 = 0.0f;

// Step size in number of pixels
#ifndef STEP_SIZE
#define STEP_SIZE 2
#endif

// Number of shared-memory samples per direction
#ifndef NUM_STEPS
#define NUM_STEPS 8
#endif

sampler2D DepthBuffer;
sampler2D RandomMap;
readwrite r16f image2D HBAO0;
write rg16f image2D HBAO1;

samplerstate DepthSampler
{
	Samplers = { DepthBuffer };
	Filter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

// Maximum kernel radius in number of pixels
#define KERNEL_RADIUS NUM_STEPS*STEP_SIZE

// The last sample has weight = exp(-KERNEL_FALLOFF)
#define KERNEL_FALLOFF 3.0f

// Must match the HBAO_TILE_WIDTH value from HBAO_DX11_LIB.cpp
#define HBAO_TILE_WIDTH 320

#define SHARED_MEM_SIZE KERNEL_RADIUS + HBAO_TILE_WIDTH + KERNEL_RADIUS
groupshared vec2 SharedMemory[SHARED_MEM_SIZE];

//----------------------------------------------------------------------------------
float Tangent(float2 V)
{
    // Add an epsilon to avoid any division by zero.
    return -V.y / (abs(V.x) + 1.e-6f);
}

//----------------------------------------------------------------------------------
float TanToSin(float x)
{
    return x * rsqrt(x*x + 1.0f);
}

//----------------------------------------------------------------------------------
float Falloff(float sampleId)
{
    // Pre-computed by fxc.
    float r = sampleId / (NUM_STEPS-1);
    return exp(-KERNEL_FALLOFF*r*r);
}

//----------------------------------------------------------------------------------
vec2 MinDiff(vec2 P, vec2 Pr, vec2 Pl)
{
    vec2 V1 = Pr - P;
    vec2 V2 = P - Pl;
    return (dot(V1,V1) < dot(V2,V2)) ? V1 : V2;
}

//----------------------------------------------------------------------------------
// Load (X,Z) view-space coordinates from shared memory.
// On Fermi, such strided 64-bit accesses should not have any bank conflicts.
//----------------------------------------------------------------------------------
vec2 SharedMemoryLoad(uint centerId, uint x)
{
    return SharedMemory[centerId + x];
}

//----------------------------------------------------------------------------------
// Compute (X,Z) view-space coordinates from the depth texture.
//----------------------------------------------------------------------------------
vec2 LoadXZFromTexture(uint x, uint y)
{ 
    vec2 uv = (vec2(x, y) + vec2(0.5f)) * InvAOResolution;
    float z_eye = textureLod(DepthBuffer, uv, 0).r;
    float x_eye = (UVToViewA.x * uv.x + UVToViewB.x) * z_eye;
    return vec2(x_eye, z_eye);
}

//----------------------------------------------------------------------------------
// Compute (Y,Z) view-space coordinates from the depth texture.
//----------------------------------------------------------------------------------
vec2 LoadYZFromTexture(uint x, uint y)
{
    vec2 uv = (vec2(x, y) + vec2(0.5f)) * InvAOResolution;
    float z_eye = textureLod(DepthBuffer, uv, 0).r;
    float y_eye = (UVToViewA.y * uv.y + UVToViewB.y) * z_eye;
    return vec2(y_eye, z_eye);
}

//----------------------------------------------------------------------------------
// Compute the HBAO contribution in a given direction on screen by fetching 2D 
// view-space coordinates available in shared memory:
// - (X,Z) for the horizontal directions (approximating Y by a constant).
// - (Y,Z) for the vertical directions (approximating X by a constant).
//----------------------------------------------------------------------------------
void IntegrateDirection(out float ao,
                        vec2 P,
                        float tanT,
                        uint threadId,
                        uint X0,
                        uint deltaX)
{
    float tanH = tanT;
    float sinH = TanToSin(tanH);
    float sinT = TanToSin(tanT);

    for (uint sampleId = 0; sampleId < NUM_STEPS; ++sampleId)
    {
        vec2 S = SharedMemoryLoad(threadId, sampleId * deltaX + X0);
        vec2 V = S - P;
        float tanS = Tangent(V);
        float d2 = dot(V, V);
        
        if ((d2 < R2) && (tanS > tanH))
        {
            // Accumulate AO between the horizon and the sample
            float sinS = TanToSin(tanS);
            ao += Falloff(sampleId) * (sinS - sinH);
            
            // Update the current horizon angle
            tanH = tanS;
            sinH = sinS;
        }
    }
}

//----------------------------------------------------------------------------------
// Bias tangent angle and compute HBAO in the +X/-X or +Y/-Y directions.
//----------------------------------------------------------------------------------
float ComputeHBAO(vec2 P, vec2 T, uint centerId)
{
    float ao = 0;
    float tanT = Tangent(T);
    IntegrateDirection(ao, P,  tanT + TanAngleBias, centerId,  STEP_SIZE,  STEP_SIZE);
    IntegrateDirection(ao, P, -tanT + TanAngleBias, centerId, -STEP_SIZE, -STEP_SIZE);
    return ao;
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = HBAO_TILE_WIDTH
[localsizey] = 1
shader
void
csMainX() 
{
    const uint         tileStart = gl_WorkGroupID.x * HBAO_TILE_WIDTH;
    const uint           tileEnd = tileStart + HBAO_TILE_WIDTH;
    const uint        apronStart = tileStart - KERNEL_RADIUS;
    const uint          apronEnd = tileEnd   + KERNEL_RADIUS;

    const uint x = apronStart + gl_LocalInvocationID.x;
    const uint y = gl_WorkGroupID.y;

    // Load float2 samples into shared memory
    SharedMemory[gl_LocalInvocationID.x] = LoadXZFromTexture(x,y);
    SharedMemory[min(2 * KERNEL_RADIUS + gl_LocalInvocationID.x, SHARED_MEM_SIZE - 1)] = LoadXZFromTexture(2 * KERNEL_RADIUS + x, y);
    groupMemoryBarrier();

    const uint writePos = tileStart + gl_LocalInvocationID.x;
    const uint tileEndClamped = min(tileEnd, int(AOResolution.x));
    
    if (writePos < tileEndClamped)
    {
        uint centerId = gl_LocalInvocationID.x + KERNEL_RADIUS;
        uint ox = writePos;
        uint oy = gl_WorkGroupID.y;

        // Fetch the 2D coordinates of the center point and its nearest neighbors
        float2 P = 	SharedMemoryLoad(centerId, 0);
        float2 Pr = SharedMemoryLoad(centerId, 1);
        float2 Pl = SharedMemoryLoad(centerId, -1);
        
        // Compute tangent vector using central differences
        float2 T = MinDiff(P, Pr, Pl);

        float ao = ComputeHBAO(P, T, centerId);
		imageStore(HBAO0, int2(ox, oy), vec4(ao, 1, 0, 0));
    }
	imageStore(HBAO0, int2(writePos, gl_WorkGroupID.y), vec4(1, 1, 0, 0));
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = HBAO_TILE_WIDTH
[localsizey] = 1
shader
void
csMainY() 
{
	const uint         tileStart = gl_WorkGroupID.x * HBAO_TILE_WIDTH;
    const uint           tileEnd = tileStart + HBAO_TILE_WIDTH;
    const uint        apronStart = tileStart - KERNEL_RADIUS;
    const uint          apronEnd = tileEnd   + KERNEL_RADIUS;

    const uint x = gl_WorkGroupID.y;
    const uint y = apronStart + gl_LocalInvocationID.x;

    // Load float2 samples into shared memory
    SharedMemory[gl_LocalInvocationID.x] = LoadYZFromTexture(x,y);
    SharedMemory[min(2 * KERNEL_RADIUS + gl_LocalInvocationID.x, SHARED_MEM_SIZE - 1)] = LoadYZFromTexture(x, 2 * KERNEL_RADIUS + y);
    groupMemoryBarrier();

    const uint writePos = tileStart + gl_LocalInvocationID.x;
    const uint tileEndClamped = min(tileEnd, int(AOResolution.y));
    
    if (writePos < tileEndClamped)
    {
        uint centerId = gl_LocalInvocationID.x + KERNEL_RADIUS;
        uint ox = gl_WorkGroupID.y;
        uint oy = writePos;

        // Fetch the 2D coordinates of the center point and its nearest neighbors
        float2 P = 	SharedMemoryLoad(centerId, 0);
        float2 Pt = SharedMemoryLoad(centerId, 1);
        float2 Pb = SharedMemoryLoad(centerId, -1);

        // Compute tangent vector using central differences
        float2 T = MinDiff(P, Pt, Pb);

        float aoy = ComputeHBAO(P, T, centerId);
        float aox = imageLoad(HBAO0, int2(gl_WorkGroupID.y, writePos)).r;
        float ao = (aox + aoy) * 0.25;

        ao = saturate(ao * Strength);
		imageStore(HBAO1, int2(ox, oy), vec4(ao, P.y, 0, 0));
    }
}

//------------------------------------------------------------------------------
/**
*/
program HBAOX [ string Mask = "X"; ]
{
	ComputeShader = csMainX();
};

program HBAOY [ string Mask = "Y"; ]
{
	ComputeShader = csMainY();
};