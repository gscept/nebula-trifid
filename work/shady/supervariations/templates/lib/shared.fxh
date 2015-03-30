//------------------------------------------------------------------------------
//  shared.fxh
//  (C) 2011 LTU Skellefteå
//------------------------------------------------------------------------------

#ifndef SHARED_H
#define SHARED_H

#include "lib/std.fxh"

// instancing transforms
mat4 ModelArray[128];

varblock PerFrame
{
	mat4 View;
	mat4 InvView;
	mat4 ViewProjection;
	mat4 Projection;
	mat4 InvProjection;
	mat4 InvViewProjection;
	vec4 EyePos;
	vec4 FocalLength;
};

#define MAX_NUM_LIGHTS 16
vec4		LightPositionsArray[MAX_NUM_LIGHTS];
mat4		LightProjTransformArray[MAX_NUM_LIGHTS];
vec4		LightColorArray[MAX_NUM_LIGHTS];
vec4		LightProjMapOffsetArray[MAX_NUM_LIGHTS];
vec4		LightShadowMapOffsetArray[MAX_NUM_LIGHTS];
vec4		LightShadowSizeArray[MAX_NUM_LIGHTS];
float		LightInvRangeArray[MAX_NUM_LIGHTS];
int			LightTypeArray[MAX_NUM_LIGHTS];
bool		LightCastsShadowsArray[MAX_NUM_LIGHTS];
sampler2D	LightShadowTexture;

vec4 GlobalBackLightColor;
vec4 GlobalLightColor;
vec4 GlobalAmbientLightColor;
vec4 GlobalLightDir;
float GlobalBackLightOffset;

// define how many objects we can render simultaneously 
#define MAX_BATCH_SIZE 128

mat4 Model;
mat4 InvModel;
mat4 EmitterTransform;
vec4 OcclusionConstants;
vec4 TextureRatio;

vec4 MatDiffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
float MatEmissiveIntensity = 0.0f;
float MatSpecularIntensity = 0.0f;
float AlphaSensitivity = 0.0f;
float AlphaBlendFactor = 0.0f;
float LightMapIntensity = 0.0f;

float FresnelPower = 0.0f;
float FresnelStrength = 0.0f;
int ObjectId;
	
float TessellationFactor = 1.0f;
float MaxDistance = 250.0f;
float MinDistance = 20.0f;
float HeightScale = 0.0f;
float SceneScale = 1.0f;

// animation variables
vec2 AnimationDirection;
float AnimationAngle;
float Time;
float Random;
float AnimationLinearSpeed;
float AnimationAngularSpeed;
int NumXTiles = 1;
int NumYTiles = 1;

float WindWaveSize = 1.0f;
float WindSpeed = 0.0f;
vec4 WindDirection = vec4(0.0f,0.0f,0.0f,1.0f);
float WindIntensity = 0.0f;
float WindForce = 0.0f;


// The number of CSM cascades 
#ifndef CASCADE_COUNT_FLAG
#define CASCADE_COUNT_FLAG 4
#endif

// the matrix array can either hold all views of a point light (6) or split matrices for CSM (4)
mat4 ViewMatrixArray[6];

const float ShadowConstant = 100.0f;



#endif // SHARED_H
