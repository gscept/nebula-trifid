//------------------------------------------------------------------------------
//  ssr_cs.fx
//  (C) 2015 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"
#include "lib/shared.fxh"

sampler2D DepthBuffer;
sampler2D ColorBuffer;
sampler2D SpecularBuffer;
sampler2D NormalBuffer;
readwrite rgba16f image2D EmissiveBuffer;

const float SearchDist = 5.0f;
const float SearchDistInv = 0.2f;
const int MaxBinarySearchSteps = 2;
const int MaxSteps = 5;
const float RayStep = 0.25f;
const float MinRayStep = 0.1f;
vec2 Resolution;
vec2 InvResolution;

#define KERNEL_RADIUS 16
#define HALF_KERNEL_RADIUS (KERNEL_RADIUS/2.0f)

#define SSR_TILE_WIDTH 320
#define SHARED_MEM_SIZE (KERNEL_RADIUS + SSR_TILE_WIDTH + KERNEL_RADIUS)
//groupshared vec4 SharedMemory[SSR_TILE_WIDTH];
groupshared float SharedDepth[32*32];

//------------------------------------------------------------------------------
/**
*/
vec3
BinarySearch(vec3 dir, inout vec3 hit, out float odepth)
{
	float depth;
	for (int i = 0; i < MaxBinarySearchSteps; i++)
	{
		vec4 projCoord = Projection * vec4(hit, 1.0f);
		projCoord.xy /= projCoord.w;
		projCoord.xy = projCoord.xy * 0.5f + 0.5f;
		depth = texture(DepthBuffer, projCoord.xy).r;
		odepth = hit.z - depth;
		if (depth > 0.0f) hit += dir;
		
		dir *= 0.5f;
		hit -= dir;
	}
	
	vec4 projCoord = Projection * vec4(hit, 1.0f);
	projCoord.xy /= projCoord.w;
	projCoord.xy = projCoord.xy * 0.5f + 0.5f;
	projCoord.z = depth;
	return projCoord.xyz;
}

//------------------------------------------------------------------------------
/**
*/
vec4
RayCast(vec3 dir, inout vec3 hit, out float odepth)
{
	dir *= RayStep;
	float depth;
	for (int i = 0; i < MaxSteps; i++)
	{
		hit += dir;
		vec4 projCoord = Projection * vec4(hit, 1.0f);
		projCoord.xy /= projCoord.w;
		projCoord.xy = projCoord.xy * 0.5f + 0.5f;
		depth = textureLod(DepthBuffer, projCoord.xy, 0).r;
		odepth = hit.z - depth;
		if (depth < 0.0f) return vec4(BinarySearch(dir, hit, odepth), 1.0f);
	}
	return vec4(0.0f);
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = 32
[localsizey] = 32
//[localsizex] = 1
shader
void
csMain()
{
	ivec2 UV = ivec2(gl_WorkGroupID.xy * ivec2(32, 32) + gl_LocalInvocationID.xy);
	float Depth = texelFetch(DepthBuffer, UV, 0).r;
	vec4 Spec = texelFetch(SpecularBuffer, UV, 0);
	vec3 viewSpaceNormal = UnpackViewSpaceNormal(texelFetch(NormalBuffer, UV, 0));
	vec3 viewSpacePosition = vec3((UV * InvResolution) * FocalLength.xy, -1);
	vec3 viewVec = normalize(viewSpacePosition);
	vec3 reflection = normalize(reflect(viewVec, viewSpaceNormal));
	vec3 hit = viewVec;
	float depth;
	
	vec4 coords = RayCast(reflection * max(MinRayStep, -viewSpacePosition.z), hit, depth);
	//SharedMemory[gl_]
	vec2 clampval = abs(vec2(0.5f) - coords.xy);
	float edgeFactor = clamp(1.0f - (clampval.x + clampval.y), 0.0f, 1.0f);
	vec4 result = textureLod(ColorBuffer, coords.xy, 0);
	result.a = edgeFactor * 
		clamp(-reflection.z, 0.0f, 1.0f) * 
		Spec.a *
		clamp((SearchDist - length(viewSpacePosition - hit)) * SearchDistInv, 0.0f, 1.0f) * coords.w;
		
	// load original emissive, then perform simple alpha blending.
	vec4 emissive = imageLoad(EmissiveBuffer, UV);
	imageStore(EmissiveBuffer, UV, lerp(emissive, result, result.a));
}


//------------------------------------------------------------------------------
/**
*/
program SSR [ string Mask = "Alt0"; ]
{
	ComputeShader = csMain();
};
