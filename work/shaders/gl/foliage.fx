//------------------------------------------------------------------------------
//  geometrybase.fxh
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/geometrybase.fxh"
#include "lib/shadowbase.fxh"
#include "lib/techniques.fxh"
#include "lib/lightmapbase.fxh"

state FoliageState
{
	CullMode = None;
	//AlphaToCoverageEnabled = true;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsTree(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec4 color,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV,
	out vec3 WorldViewVec)	
{
	UV = uv;
	
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	float len = length(position);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = len / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength * color.r;
	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	vec4 modelSpace = Model * finalPos;
	gl_Position = ViewProjection * modelSpace;

	mat4 modelView = View * Model;
	ViewSpacePos = (modelView * finalPos).xyz;
    
	Tangent  = (modelView * vec4(tangent, 0)).xyz;
	Normal   = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
	WorldViewVec = modelSpace.xyz - EyePos.xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsTreeInstanced(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec4 color,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV,
	out vec3 WorldViewVec)	
{
	UV = uv;
	
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + gl_InstanceID);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength * color.r;
	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	vec4 modelSpace = ModelArray[gl_InstanceID] * finalPos;
	gl_Position = ViewProjection * modelSpace;

	mat4 modelView = View * ModelArray[gl_InstanceID];
	ViewSpacePos = (modelView * finalPos).xyz;
	    
	Tangent  = (modelView * vec4(tangent, 0)).xyz;
	Normal   = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
	WorldViewVec = modelSpace.xyz - EyePos.xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsGrass(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec4 color,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV,
	out vec3 WorldViewVec)	
{
	UV = uv;
	
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength * color.r;
	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	vec4 modelSpace = Model * finalPos;
	gl_Position = ViewProjection * modelSpace;
	
	mat4 modelView = View * Model;
	ViewSpacePos = (modelView * finalPos).xyz;
    
	Tangent  = (modelView * vec4(tangent, 0)).xyz;
	Normal   = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
	WorldViewVec = modelSpace.xyz - EyePos.xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsGrassInstanced(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec4 color,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV,
	out vec3 WorldViewVec)	
{
	UV = uv;
	
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + gl_InstanceID);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength * color.r;
	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	vec4 modelSpace = ModelArray[gl_InstanceID] * finalPos;
	gl_Position = ViewProjection * modelSpace;
	
	mat4 modelView = View * ModelArray[gl_InstanceID];
	ViewSpacePos = (modelView * finalPos).xyz;
    
	Tangent  = (modelView * vec4(tangent, 0)).xyz;
	Normal   = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
	WorldViewVec = modelSpace.xyz - EyePos.xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsTreeShadow(vec4 position,
	vec3 normal,
	vec2 uv,
	vec3 tangent,
	vec3 binormal,
	out vec2 UV,
	out vec4 ProjPos)
{
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength;
	gl_Position = position + finalOffset;
	gl_Position = LightViewProjection * Model * gl_Position;
	ProjPos = gl_Position;
	UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsGrassShadow(vec4 position,
	vec3 normal,
	vec2 uv,
	vec4 color,
	vec3 tangent,
	vec3 binormal,
	out vec2 UV,
	out vec4 ProjPos) 
{
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength;
	gl_Position = position + finalOffset * color;
	gl_Position = LightViewProjection * Model * gl_Position;
	ProjPos = gl_Position;
	UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsTreeShadowCSM(vec4 position,
	vec3 normal,
	vec2 uv,
	vec3 tangent,
	vec3 binormal,
	out vec2 UV,
	out vec4 ProjPos,
	out int Instance) 
{
	vec4 dir = vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
		
	vec4 finalOffset = windDir * windStrength;
	finalOffset = position + finalOffset;
	ProjPos = ViewMatrixArray[gl_InstanceID] * Model * finalOffset;
	UV = uv;
	Instance = gl_InstanceID;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsGrassShadowCSM(vec4 position,
	vec3 normal,
	vec2 uv,
	vec4 color,
	vec3 tangent,
	vec3 binormal,
	out vec2 UV,
	out vec4 ProjPos,
	out int Instance) 
{
	vec4 dir = vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength;
	finalOffset = position + finalOffset;
	ProjPos = ViewMatrixArray[gl_InstanceID] * Model * finalOffset;
	UV = uv;
	Instance = gl_InstanceID;
}

//------------------------------------------------------------------------------
/**
	Used for lightmapped 
*/
shader
void
vsTreeLightmapped(in vec3 position,
	in vec3 normal,
	in vec2 uv1,
	in vec2 uv2,
	in vec4 color,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV1,
	out vec2 UV2)
{
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	float len = length(position);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = len / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
	
	vec4 finalOffset = windDir * windStrength * color.a;
	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	
	gl_Position = ViewProjection * Model * finalPos;
	UV1 = uv1;
	UV2 = uv2;
	mat4 modelView = View * Model;

	ViewSpacePos = (modelView * finalPos).xyz;
	Tangent = (modelView * vec4(tangent, 0)).xyz;
	Normal = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;	
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psPicking(in vec2 UV,
		in vec4 ProjPos,		
		[color0] out float Id) 
{
	Id = float(ObjectId);
}

//------------------------------------------------------------------------------
//	Coloring methods
//------------------------------------------------------------------------------
SimpleTechnique(
	Tree, 
	"Static", 
	vsTree(), 
	psUberAlphaTest(
		calcColor = SimpleColor,
		calcBump = NormalMapFunctor,
		calcSpec = NonReflectiveSpecularFunctor,
		calcDepth = ViewSpaceDepthFunctor,
		calcEnv = IrradianceOnly
	),
	FoliageState);
	
SimpleTechnique(
	TreeInstanced, 
	"Static|Instanced", 
	vsTreeInstanced(), 
	psUberAlphaTest(
		calcColor = SimpleColor,
		calcBump = NormalMapFunctor,
		calcSpec = NonReflectiveSpecularFunctor,
		calcDepth = ViewSpaceDepthFunctor,
		calcEnv = IrradianceOnly
	),
	FoliageState);
	
SimpleTechnique(
	Grass, 
	"Static|Colored", 
	vsGrass(), 
	psUberAlphaTest(
		calcColor = SimpleColor,
		calcBump = NormalMapFunctor,
		calcSpec = NonReflectiveSpecularFunctor,
		calcDepth = ViewSpaceDepthFunctor,
		calcEnv = IrradianceOnly
	),
	FoliageState);
	
SimpleTechnique(
	GrassInstanced, 
	"Static|Colored|Instanced", 
	vsGrassInstanced(), 
	psUberAlphaTest(
		calcColor = SimpleColor,
		calcBump = NormalMapFunctor,
		calcSpec = NonReflectiveSpecularFunctor,
		calcDepth = ViewSpaceDepthFunctor,
		calcEnv = IrradianceOnly
	),
	FoliageState);

//------------------------------------------------------------------------------
//	Shadowing methods
//------------------------------------------------------------------------------
SimpleTechnique(DefaultShadow, "Static|Spot", vsTreeShadow(), psShadowAlpha(), FoliageState);
GeometryTechnique(CSMShadow, "Static|Global", vsTreeShadowCSM(), psVSMAlpha(), gsCSM(), FoliageState);
GeometryTechnique(PointlightShadow, "Static|Point", vsTreeShadowCSM(), psVSMAlpha(), gsPoint(), FoliageState);
SimpleTechnique(Picking, "Static|Picking", vsTreeShadow(), psPicking(), FoliageState);

//------------------------------------------------------------------------------
//	Lightmapped methods
//------------------------------------------------------------------------------
SimpleTechnique(LitFoliage, "Static|Lightmapped", vsTreeLightmapped(), psLightmappedLit(), FoliageState);
SimpleTechnique(UnlitFoliage, "Static|Unlit|Lightmapped", vsTreeLightmapped(), psLightmappedUnlit(), FoliageState);

//------------------------------------------------------------------------------
/**

shader
void
vsFoliageUber(in vec3 position,
	in vec3 normal,
	in vec2 uv,
#ifdef USE_LIGHTMAPPING
	in vec2 lightmapUv,
#endif
#ifdef USE_WEIGHTED_VERTICES
	in vec4 color,
#endif
	in vec3 tangent,
	in vec3 binormal
#ifndef USE_SHADOW_RENDERING
	,
	out vec3 ViewSpacePos
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV,
	out vec3 WorldViewVec
	#ifdef USE_LIGHTMAPPING
		,
		out vec2 LightmapUV
	#endif
#else
	,
	out vec4 ProjPos
#endif
)	
{
	UV = uv;
	
	vec4 dir = InvModel * vec4(WindDirection.xyz, 0);
	vec4 windDir = WindForce * normalize(dir);
	
	float windSpeed = WindSpeed * (TimeAndRandom.x + ObjectId);
	float windAmplitude = length(position) / WindWaveSize;
	float windStrength = sin(windSpeed + windAmplitude);
		
#ifdef WEIGHTED_VERTICES
	vec4 finalOffset = windDir * windStrength * color.r;
#else
	vec4 finalOffset = windDir * windStrength;
#endif

	vec4 finalPos = vec4(position + finalOffset.xyz, 1);
	
#ifdef USE_INSTANCING
	vec4 modelSpace = ModelArray[gl_InstanceID] * finalPos;
#else
	vec4 modelSpace = Model * finalPos;
#endif
	gl_Position = ViewProjection * modelSpace;
	    
#ifndef USE_SHADOW_RENDERING
	mat4 modelView = View * Model;
	ViewSpacePos = (modelView * finalPos).xyz;
	Tangent  = (modelView * vec4(tangent, 0)).xyz;
	Normal   = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
	WorldViewVec = modelSpace.xyz - EyePos.xyz;
	
	#ifdef USE_LIGHTMAPPING
		LightmapUV = lightmapUv;
	#endif
#else
	#if USE_CSM_SHADOWS
		ProjPos = ViewMatrixArray[gl_InstanceID] * Model * finalOffset;
	#else
		ProjPos = gl_Position;
	#endif
#endif
}

program FlagTest
{
	VertexShader = vsFoliageUber();
	PixelShader = psUberAlphaTest();
	RenderState = FoliageState;
	CompileFlags = "USE_INSTANCING|USE_WEIGHTED_VERTICES|USE_SHADOW_RENDERING|USE_CSM_SHADOWS";
};
*/