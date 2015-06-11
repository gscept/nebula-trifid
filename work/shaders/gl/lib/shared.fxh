//------------------------------------------------------------------------------
//  shared.fxh
//  (C) 2011 LTU Skellefteå
//------------------------------------------------------------------------------

#ifndef SHARED_FXH
#define SHARED_FXH

#include "lib/std.fxh"


// define how many objects we can render simultaneously 
#define MAX_BATCH_SIZE 256

// instancing transforms
shared buffers=256 varblock PerBatch
{
	mat4 ModelArray[MAX_BATCH_SIZE];
};

shared buffers=32 varblock PerFrame
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
};

#define FLT_MAX     3.40282347E+38F
#define FLT_MIN     -3.40282347E+38F

#define MAX_NUM_LIGHTS 16
shared varblock ForwardLights
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
mat4 LightViewProjection;
sampler2D	LightShadowTexture;

vec4 GlobalLightDir;
vec4 GlobalLightColor;
vec4 GlobalBackLightColor;
vec4 GlobalAmbientLightColor;
float GlobalBackLightOffset;
	
// the smartest thing to do is to make the buffer count equal to the number of draw calls we want per frame
// also tagged as nosync, which limits us to doing 8192 draw calls per frame (which should be FINE but one never knows)
shared buffers=4096 varblock PerObject
{
	mat4 Model;
	mat4 InvModel;
	mat4 ModelViewProjection;
	mat4 ModelView;
	int ObjectId;
};

/*
shared varbuffer PerObject
{
	mat4 Model;
	mat4 InvModel;
	mat4 ModelViewProjection;
	mat4 ModelView;
	int ObjectId;
};
*/


mat4 EmitterTransform;
vec4 RenderTargetDimensions;	// x and y holds 1 / render target size, z and w holds render target size

vec4 MatDiffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
float LightMapIntensity = 0.0f;
float FresnelPower = 0.0f;
float FresnelStrength = 0.0f;

// material properties
float AlphaSensitivity = 1.0f;
float AlphaBlendFactor = 0.0f;
vec4 MatAlbedoIntensity = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 MatSpecularIntensity = vec4(1.0f, 1.0f, 1.0f, 1.0f);	
vec4 MatEmissiveIntensity = vec4(1.0f, 1.0f, 1.0f, 1.0f);	
vec4 MatFresnelIntensity = vec4(1.0f, 1.0f, 1.0f, 1.0f);
float MatRoughnessIntensity = 0.0f;

float TessellationFactor = 1.0f;
float MaxDistance = 250.0f;
float MinDistance = 20.0f;
float HeightScale = 0.0f;
float SceneScale = 1.0f;

vec2 AnimationDirection;
float AnimationAngle;
float AnimationLinearSpeed;
float AnimationAngularSpeed;
int NumXTiles = 1;
int NumYTiles = 1;

vec4 WindDirection = vec4(0.0f,0.0f,0.0f,1.0f);
float WindWaveSize = 1.0f;
float WindSpeed = 0.0f;
float WindIntensity = 0.0f;
float WindForce = 0.0f;

// The number of CSM cascades 
#ifndef CASCADE_COUNT_FLAG
#define CASCADE_COUNT_FLAG 4
#endif

// the matrix array can either hold all views of a point light (6) or split matrices for CSM (4)
shared buffers=32 varblock PerShadowFrame
{
	mat4 ViewMatrixArray[6];
};

#endif // SHARED_H
