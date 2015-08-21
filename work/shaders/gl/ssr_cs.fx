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
const int MaxBinarySearchSteps = 5;
const int MaxSteps = 10;
const float RayStep = 0.25f;
const float MinRayStep = 0.01f;
vec2 Resolution;
vec2 InvResolution;

samplerstate LinearState
{
	Samplers = {DepthBuffer, ColorBuffer, SpecularBuffer, NormalBuffer};
	Filter = Point;
	BorderColor = {0,0,0,0};
	AddressU = Border;
	AddressV = Border;
};

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
	vec4 projCoord;
	for (int i = 0; i < MaxBinarySearchSteps; i++)
	{
		projCoord = Projection * vec4(hit, 1.0f);
		projCoord.xy /= projCoord.ww;
		projCoord.xy = projCoord.xy * vec2(0.5f, 0.5f) + 0.5f;
		depth = texture(DepthBuffer, projCoord.xy).r;
		odepth = hit.z - depth;
		if (depth > 0.0f) hit += dir;
		
		dir *= 0.5f;
		hit -= dir;
	}
	
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
		projCoord.xy /= projCoord.ww;
		projCoord.xy = projCoord.xy * vec2(0.5f, 0.5f) + 0.5f;
		depth = textureLod(DepthBuffer, projCoord.xy, 0).r;
		odepth = hit.z - depth;
		if (odepth < 0.0f) return vec4(BinarySearch(dir, hit, odepth), 1.0f);
	}
	return vec4(0.0f);
}


//------------------------------------------------------------------------------
/**
*/
vec4
RayTrace(vec3 reflection, vec3 startDir, in float startDepth, in vec2 UV, out vec2 outUV)
{
	vec4 result = vec4(0.0f);
	float size = length(startDir.xy);
	vec3 dir = normalize(Projection * vec4(startDir, 0) / size).xyz;
	dir *= MinRayStep;
	//vec3 dir = startDir * MinRayStep;
	vec2 samplePos = UV;
	float sampleDepth;
	float currentDepth = startDepth;
	
	for (int i = 0; i < MaxSteps; i++)
	//while (samplePos.x <= 1.0f && samplePos.x >= 0.0f && samplePos.y <= 1.0f && samplePos.y >= 0.0f)
	{	
		sampleDepth = textureLod(DepthBuffer, samplePos, 0).r;
		if (currentDepth > sampleDepth)
		{
			float delta = abs(currentDepth - sampleDepth);
			if (delta < 0.03f)
			{
				result = textureLod(ColorBuffer, samplePos, 0);
				//imageStore(EmissiveBuffer, ivec2(samplePos * Resolution), vec4(samplePos, 0, 1));
				break;
			}
		}
		
		samplePos += dir.xy;
		currentDepth = length(vec3(samplePos.xy, reflection.z * sampleDepth * (i + 1)));
		
	}
	outUV = samplePos;
	return result;
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = 32
[localsizey] = 32
shader
void
csMain()
{
	//ivec2 size = textureSize(DepthBuffer, 0);
	ivec2 PixelCoord = ivec2(gl_GlobalInvocationID.xy);
	PixelCoord.y = int(Resolution.y) - PixelCoord.y;
	vec2 UV = PixelCoord * InvResolution;
	//UV = FlipY(UV);
	vec3 viewSpaceNormal = normalize(UnpackViewSpaceNormal(textureLod(NormalBuffer, UV, 0)));
	float depth = textureLod(DepthBuffer, UV, 0).r;
	vec4 Spec = textureLod(SpecularBuffer, UV, 0);
	
	vec3 viewVec = normalize(vec3(UV * FocalLength.xy, -1));
	vec3 worldNormal = (InvView * vec4(viewSpaceNormal, 0)).xyz;
	vec4 worldPos = vec4(viewVec * depth, 1);
	vec3 eye = normalize(vec3(0, 0, 0.1f));
	
	vec4 reflection = vec4(normalize(reflect(-eye, viewSpaceNormal)), 0);
	vec4 reflectionProjectionSpace = Projection * reflection;
	//vec4 reflection = Projection * normalize(vec4(reflect(worldPos.xyz - EyePos.xyz, worldNormal), 0));
	vec2 samplePos;
	vec4 color = RayTrace(reflection.xyz, reflection.xyz, depth, UV, samplePos);	
	
	vec2 clampval = abs(vec2(0.5f) - samplePos);
	float edgeFactor = clamp(1.0f - (clampval.x + clampval.y), 0.0f, 1.0f);
	//float blend = edgeFactor * clamp(reflection.z, 0.0f, 1.0f) * Spec.a;
	float blend = Spec.a;// * edgeFactor;
	//color.a = ;
	//color.a = Spec.a * edgeFactor;
	//color.rgb *= Spec.rgb;

	vec4 emissive = imageLoad(EmissiveBuffer, PixelCoord);
	groupMemoryBarrier();
	//color.a = emissive.a;
	//imageStore(EmissiveBuffer, PixelCoord, vec4(lerp(emissive.rgb, color.rgb * Spec.a, color.a), 1));	]
	//imageStore(EmissiveBuffer, PixelCoord, lerp(emissive, color, blend));
	//
	imageStore(EmissiveBuffer, PixelCoord, lerp(emissive, color, blend));	
}

/*
shader
void
csMain()
{
	ivec2 PixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 UV = PixelCoord * InvResolution;
	//UV = FlipY(UV);
	float Depth = textureLod(DepthBuffer, UV, 0).r;
	vec4 Spec = textureLod(SpecularBuffer, UV, 0);
	vec3 viewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, UV, 0));
	vec3 viewSpacePosition = vec3(UV, -1);
	
	//viewSpacePosition.z = Depth;
	vec3 viewVec = normalize(viewSpacePosition);
	vec3 worldNormal = (InvView * vec4(viewSpaceNormal, 0)).xyz;
	vec4 worldPos = vec4(viewVec * Depth, 1);
	vec3 eye = normalize(vec3(0, 0, 0.1f));
	//vec4 reflection = Projection * reflect(vec4(-eye, 0), vec4(viewSpaceNormal, 0));
	//vec3 reflection = normalize(reflect(worldPos.xyz, worldNormal));
	vec4 reflection = reflect(vec4(-eye, 0), vec4(viewSpaceNormal, 0));
	vec3 hit = viewVec;
	float depth;
	
	vec4 coords = RayCast(reflection.xyz * max(MinRayStep, -viewSpacePosition.z), hit, depth);
	//SharedMemory[gl_]
	vec2 clampval = abs(vec2(0.5f) - coords.xy);
	float edgeFactor = clamp(1.0f - (clampval.x + clampval.y), 0.0f, 1.0f);
	vec4 result = textureLod(ColorBuffer, coords.xy, 0);
	result.a = edgeFactor * 
		clamp(-reflection.z, 0.0f, 1.0f) * 
		Spec.a *
		clamp((SearchDist - length(viewSpacePosition - hit)) * SearchDistInv, 0.0f, 1.0f) * coords.w;
		
	// load original emissive, then perform simple alpha blending.
	vec4 emissive = imageLoad(EmissiveBuffer, PixelCoord);
	imageStore(EmissiveBuffer, PixelCoord, lerp(emissive, result, result.a));
}
*/

//------------------------------------------------------------------------------
/**
*/
vec2
RayTrace2D(in vec3 startDir, in float startDepth, in vec2 UV)
{
	float depth = 0;
	vec2 hit = UV;
	vec3 samplePos = vec3(UV, 0);
	
	for (int i = 0; i < MaxSteps; i++)
	{
		samplePos += startDir;
		depth += startDir.z * startDepth;
		float sampleDepth = textureLod(DepthBuffer, samplePos.xy, 0).r;
		if (depth > sampleDepth)
		{
			float delta = abs(depth - sampleDepth);
			if (delta < 0.3f)
			{
				hit = samplePos.xy;
				break;
			}
		}
	}
	return hit;
}

//------------------------------------------------------------------------------
/**
*/
/*
[localsizex] = 32
[localsizey] = 32
shader
void
csMain()
{
	ivec2 PixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 UV = PixelCoord * InvResolution;
	
	float Depth = textureLod(DepthBuffer, UV, 0).r;
	vec4 Spec = textureLod(SpecularBuffer, UV, 0);
	vec3 viewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, UV, 0));
	vec3 viewVec = vec3(UV * FocalLength.xy, -1);
	//vec3 surfacePos = viewVec * Depth;
	//vec3 worldNormal = (InvView * vec4(viewSpaceNormal, 0)).xyz;
	//vec3 reflection = reflect(vec3(surfacePos - EyePos.xyz), worldNormal);
	vec3 reflection = normalize(reflect(vec3(0, 0, -1), viewSpaceNormal));
	vec2 coord = RayTrace2D(reflection * MinRayStep, Depth, UV);
	//float modulate = saturate(dot(-reflection, viewVec));
	float modulate = 1.0f;
	vec4 color = textureLod(ColorBuffer, coord, 0) * Spec;
	
	vec4 emissive = imageLoad(EmissiveBuffer, PixelCoord);
	imageStore(EmissiveBuffer, PixelCoord, lerp(emissive, color, modulate));
}
*/

//------------------------------------------------------------------------------
/**
*/
program SSR [ string Mask = "Alt0"; ]
{
	ComputeShader = csMain();
};
