//------------------------------------------------------------------------------
//  shared.fxh
//  (C) 2011 LTU Skellefteå
//------------------------------------------------------------------------------

#ifndef SHARED_FXH
#define SHARED_FXH

#include "lib/std.fxh"
#include "lib/util.fxh"



#define MAX_2D_TEXTURES 2048
#define MAX_2D_MS_TEXTURES 64
#define MAX_2D_ARRAY_TEXTURES 1
#define MAX_CUBE_TEXTURES 128
#define MAX_3D_TEXTURES 128

#define MAX_2D_IMAGES 64
#define MAX_CUBE_IMAGES 64
#define MAX_3D_IMAGES 64


group(TICK_GROUP) texture2D			Textures2D[MAX_2D_TEXTURES];
group(TICK_GROUP) texture2DMS			Textures2DMS[MAX_2D_MS_TEXTURES];
group(TICK_GROUP) textureCube			TexturesCube[MAX_CUBE_TEXTURES];
group(TICK_GROUP) texture3D			Textures3D[MAX_3D_TEXTURES];
group(TICK_GROUP) texture2DArray		Textures2DArray[MAX_2D_ARRAY_TEXTURES];
group(TICK_GROUP) samplerstate	Basic2DSampler {};
group(TICK_GROUP) samplerstate	PosteffectSampler { Filter = Point; };

group(TICK_GROUP) shared varblock WindParams [ bool System = true; ]
{
	vec4 WindDirection = vec4(0.0f,0.0f,0.0f,1.0f);
	float WindWaveSize = 1.0f;
	float WindSpeed = 0.0f;
	float WindIntensity = 0.0f;
	float WindForce = 0.0f;
};

#define sample2D(handle, sampler, uv)			texture(sampler2D(Textures2D[handle], sampler), uv)
#define sample2DLod(handle, sampler, uv, lod)		textureLod(sampler2D(Textures2D[handle], sampler), uv, lod)
#define sample2DMS(handle, sampler, uv)			texture(sampler2DMS(Textures2DMS[handle], sampler), uv)
#define sample2DMSLod(handle, sampler, uv, lod)		textureLod(sampler2DMS(Textures2DMS[handle], sampler), uv, lod)
#define sampleCube(handle, sampler, uvw)		texture(samplerCube(TexturesCube[handle], sampler), uvw)
#define sampleCubeLod(handle, sampler, uvw, lod)	textureLod(samplerCube(TexturesCube[handle], sampler), uvw, lod)
#define sample3D(handle, sampler, uvw)			texture(sampler3D(Textures3D[handle], sampler), uvw)
#define sample3DLod(handle, sampler, uvw, lod)		textureLod(sampler3D(Textures3D[handle], sampler), uvw, lod)

#define fetch2D(handle, sampler, uv, lod)		texelFetch(sampler2D(Textures2D[handle], sampler), uv, lod)
#define fetch2DMS(handle, sampler, uv, lod)		texelFetch(sampler2DMS(Textures2DMS[handle], sampler), uv, lod)
#define fetchCube(handle, sampler, uvw, lod)		texelFetch(sampler2DArray(Textures2DArray[handle], sampler), uvw, lod)
#define fetch3D(handle, sampler, uvw, lod)		texelFetch(sampler3D(Textures3D[handle], sampler), uvw, lod)


// contains the state of the camera (and time)
group(FRAME_GROUP) shared varblock CameraBlock
{
	mat4 View;
	mat4 InvView;
	mat4 ViewProjection;
	mat4 Projection;
	mat4 InvProjection;
	mat4 InvViewProjection;
	vec4 EyePos;	
	vec4 FocalLength;
	vec4 TimeAndRandom;
	
	mat4 ViewMatrixArray[6];
};

group(FRAME_GROUP) shared varblock RenderTargetBlock
{
	textureHandle NormalBuffer;
	textureHandle DepthBuffer;
	textureHandle SpecularBuffer;
	textureHandle AlbedoBuffer;
	textureHandle EmissiveBuffer;
	textureHandle LightBuffer;
};

// constains the state of a global light
group(LIGHT_GROUP) shared varblock GlobalLightBlock [ bool System = true; ]
{
	vec4 GlobalLightDir;
	vec4 GlobalLightColor;
	vec4 GlobalBackLightColor;
	vec4 GlobalAmbientLightColor;
	float GlobalBackLightOffset;
	mat4 CSMShadowMatrix;
	textureHandle GlobalLightShadowBuffer;
};

#define FLT_MAX     3.40282347E+38F
#define FLT_MIN     -3.40282347E+38F

#define MAX_NUM_LIGHTS 16

// contains the state of all the lights used for forward shading
group(LIGHT_GROUP) shared varblock LightForwardBlock [ bool System = true; ]
{
	int			NumActiveLights = 0;
	vec4		LightPositionsArray[MAX_NUM_LIGHTS];
	mat4		LightProjTransformArray[MAX_NUM_LIGHTS];
	vec4		LightColorArray[MAX_NUM_LIGHTS];
	vec4		LightProjMapOffsetArray[MAX_NUM_LIGHTS];
	vec4		LightShadowMapOffsetArray[MAX_NUM_LIGHTS];
	vec4		LightShadowSizeArray[MAX_NUM_LIGHTS];
	float		LightInvRangeArray[MAX_NUM_LIGHTS];
	int			LightTypeArray[MAX_NUM_LIGHTS];
	bool		LightCastsShadowsArray[MAX_NUM_LIGHTS];
};

// The number of CSM cascades 
#ifndef CASCADE_COUNT_FLAG
#define CASCADE_COUNT_FLAG 4
#endif

group(LIGHT_GROUP) shared varblock CSMParamBlock [ bool System = true; ]
{
	vec4 CascadeOffset[CASCADE_COUNT_FLAG];
	vec4 CascadeScale[CASCADE_COUNT_FLAG];
	float MinBorderPadding;     
	float MaxBorderPadding;
	float ShadowPartitionSize; 
	float GlobalLightShadowBias = 0.0f;
};

group(LIGHT_GROUP) sampler2D LightShadowTexture;
group(LIGHT_GROUP) sampler2D ShadowProjMap;            

// contains variables which are guaranteed to be unique per object.
group(OBJECT_GROUP) shared varblock ObjectBlock
{
	mat4 Model;
	mat4 InvModel;
	mat4 ModelViewProjection;
	mat4 ModelView;
	int ObjectId;
};

group(PASS_GROUP) inputAttachment InputAttachments[32];
group(PASS_GROUP) shared varblock PassBlock [ bool System = true; ]
{
	// render target dimensions are size (xy) inversed size (zw)
	vec4 RenderTargetDimensions[32];
};


#endif // SHARED_H
