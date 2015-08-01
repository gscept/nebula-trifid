//------------------------------------------------------------------------------
//  lights.fx
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/CSM.fxh"
#include "lib/techniques.fxh"
#include "lib/shadowbase.fxh"

const float specPower = float(32.0f);
const float rimLighting = float(0.2f);
const float exaggerateSpec = float(1.8f);
const vec3 luminanceValue = vec3(0.299f, 0.587f, 0.114f);

vec4 LightColor;
vec4 LightPosRange;
float LightShadowBias;
mat4 LightProjTransform;

vec4 CameraPosition;
vec4 ShadowOffsetScale = vec4(0.0f, 0.0f, 0.0f, 0.0f);
vec4 ShadowConstants = vec4(100.0f, 100.0f, 0.003f, 1024.0f);
mat4 ShadowProjTransform;
float ShadowIntensity = 1.0f;
	
/// Declaring used textures
sampler2D NormalBuffer;
sampler2D DepthBuffer;
sampler2D SpecularBuffer;
sampler2D AlbedoBuffer;
sampler2D LightProjMap;
samplerCube LightProjCube;

samplerCube ShadowProjCube;

samplerstate GeometrySampler
{
	Samplers = { NormalBuffer, DepthBuffer, SpecularBuffer };
	Filter = Point;
};

samplerstate SpotlightTextureSampler
{
	Samplers = { LightProjMap, LightProjCube };
	Filter = MinMagLinearMipPoint;
	AddressU = Border;
	AddressV = Border;
	BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f };
};

#define SPECULAR_SCALE 10
#define ROUGHNESS_TO_SPECPOWER(x) exp2(SPECULAR_SCALE * x + 1)

//---------------------------------------------------------------------------------------------------------------------------
//											GLOBAL LIGHT
//---------------------------------------------------------------------------------------------------------------------------

state GlobalLightState
{
	CullMode = Back;
	DepthEnabled = false;
	DepthWrite = false;
	DepthClamp = false;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsGlob(in vec3 position,
	[slot=2] in vec2 uv,
	out vec3 ViewSpacePosition,
	out vec2 UV) 
{
    gl_Position = vec4(position, 1);
    UV = FlipY(uv);
    ViewSpacePosition = vec3(position.xy * FocalLength.xy, -1);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psGlob(in vec3 ViewSpacePosition,
	in vec2 UV,
	[color0] out vec4 Color) 
{
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(texture(NormalBuffer, UV));
	float Depth = texture(DepthBuffer, UV).r;
	
	vec4 albedoColor = texture(AlbedoBuffer, UV);	
	if (Depth < 0) { Color = EncodeHDR(albedoColor); return; };
	
	float NL = saturate(dot(GlobalLightDir.xyz, ViewSpaceNormal));
	
	vec3 diff = GlobalAmbientLightColor.xyz;
	diff += GlobalLightColor.xyz * saturate(NL);
	diff += GlobalBackLightColor.xyz * saturate(-NL + GlobalBackLightOffset); 

	vec4 specColor = texture(SpecularBuffer, UV);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	
	
	vec3 viewVec = normalize(ViewSpacePosition);
	vec3 H = normalize(GlobalLightDir.xyz - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, GlobalLightDir.xyz));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	
	Color = EncodeHDR(vec4(final, 1));
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psGlobShadow(in vec3 ViewSpacePosition,
	in vec2 UV,
	[color0] out vec4 Color) 
{
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(texture(NormalBuffer, UV));
	float Depth = texture(DepthBuffer, UV).r;
		
	vec4 albedoColor = texture(AlbedoBuffer, UV);
	if (Depth < 0) { Color = EncodeHDR(albedoColor); return; };
	
	float NL = saturate(dot(GlobalLightDir.xyz, ViewSpaceNormal));
	float shadowFactor = 1.0f;
	vec3 viewVec = normalize(ViewSpacePosition);
	vec4 debug;
		
	vec4 worldPos = vec4(viewVec * Depth, 1);
	vec4 texShadow;		
	CSMConvert(worldPos, texShadow);
	shadowFactor = CSMPS(texShadow,
						UV,
						debug);		
	shadowFactor = lerp(1.0f, shadowFactor, ShadowIntensity);

	// multiply specular with power of shadow factor, this makes shadowed areas not reflect specular light
	vec4 specColor = texture(SpecularBuffer, UV);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	

	vec3 diff = GlobalAmbientLightColor.xyz;
	diff += GlobalLightColor.xyz * NL;
	diff += GlobalBackLightColor.xyz * saturate(-NL + GlobalBackLightOffset);   
	
	vec3 H = normalize(GlobalLightDir.xyz - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, GlobalLightDir.xyz));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	Color = EncodeHDR(vec4(final * saturate(shadowFactor), 1));
}

//---------------------------------------------------------------------------------------------------------------------------
//											SPOT LIGHT
//---------------------------------------------------------------------------------------------------------------------------

state SpotLightState
{
	BlendEnabled[0] = true;
	SrcBlend[0] = One;
	DstBlend[0] = One;
	CullMode = Front;
	DepthEnabled = true;
	DepthWrite = false;
	DepthFunc = Greater;
};

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
float 
GetInvertedOcclusionSpotLight(float receiverDepthInLightSpace,
                     vec2 lightSpaceUv)
{

    // offset and scale shadow lookup tex coordinates
	vec2 shadowUv = lightSpaceUv;
    shadowUv.xy *= ShadowOffsetScale.zw;
    shadowUv.xy += ShadowOffsetScale.xy;
	
	// calculate average of 4 closest pixels
	vec2 shadowSample = texture(ShadowProjMap, shadowUv).rg;
	
	// get pixel size of shadow projection texture	
	return ChebyshevUpperBound(shadowSample, receiverDepthInLightSpace, 0.000001f);
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
float 
GetInvertedOcclusionPointLight(float receiverDepthInLightSpace,
                     vec3 lightSpaceUv)
{

    // offset and scale shadow lookup tex coordinates
	vec3 shadowUv = lightSpaceUv;
	
	// get pixel size of shadow projection texture
	vec2 shadowSample = texture(ShadowProjCube, shadowUv).rg;
	return ChebyshevUpperBound(shadowSample, receiverDepthInLightSpace, 0.001f);
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
vsSpot(in vec3 position,
	out vec3 ViewSpacePosition) 
{
	gl_Position = ViewProjection * Model * vec4(position, 1);
	ViewSpacePosition = (View * Model * vec4(position, 1)).xyz;
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
psSpot(in vec3 ViewSpacePosition,
	[color0] out vec4 Color) 
{
	vec2 pixelSize = GetPixelSize(DepthBuffer);
	vec2 screenUV = psComputeScreenCoord(gl_FragCoord.xy, pixelSize.xy);
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, screenUV, 0));
	float Depth = textureLod(DepthBuffer, screenUV, 0).r;
	
	vec3 viewVec = normalize(ViewSpacePosition);
	vec3 surfacePos = viewVec * Depth;    
	vec3 lightDir = (LightPosRange.xyz - surfacePos);
	
	float att = saturate(1.0 - length(lightDir) * LightPosRange.w);    
	if (att - 0.004 < 0) discard;
	lightDir = normalize(lightDir);
	
	vec4 projLightPos = LightProjTransform * vec4(surfacePos, 1.0f);
	if (projLightPos.z - 0.001 < 0) discard;
	float mipSelect = 0;
	vec2 lightSpaceUv = vec2(((projLightPos.xy / projLightPos.ww) * vec2(0.5f, -0.5f)) + 0.5f);
	
	vec4 lightModColor = textureLod(LightProjMap, lightSpaceUv, mipSelect);
	vec4 specColor = texture(SpecularBuffer, screenUV, 0);
	vec4 albedoColor = texture(AlbedoBuffer, screenUV, 0);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	
	
	float NL = dot(lightDir, ViewSpaceNormal);
	vec3 diff = LightColor.xyz * saturate(NL) * att;
	
	vec3 H = normalize(lightDir - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, lightDir));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	
	vec4 oColor = vec4(lightModColor.rgb * final, lightModColor.a);
	
	Color = EncodeHDR(oColor);
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
psSpotShadow(in vec3 ViewSpacePosition,
	[color0] out vec4 Color) 
{
	vec2 pixelSize = GetPixelSize(DepthBuffer);
	vec2 screenUV = psComputeScreenCoord(gl_FragCoord.xy, pixelSize.xy);
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, screenUV, 0));
	float Depth = textureLod(DepthBuffer, screenUV, 0).r;
	
	vec3 viewVec = normalize(ViewSpacePosition);
	vec3 surfacePos = viewVec * Depth;    
	vec3 lightDir = (LightPosRange.xyz - surfacePos);
	
	float att = saturate(1.0 - length(lightDir) * LightPosRange.w);    
	if (att - 0.004 < 0) discard;
	lightDir = normalize(lightDir);
	
	vec4 projLightPos = LightProjTransform * vec4(surfacePos, 1.0f);
	if(projLightPos.z - 0.001 < 0) discard;
	float mipSelect = 0;
	vec2 lightSpaceUv = (projLightPos.xy / projLightPos.ww) * vec2(0.5f, -0.5f) + 0.5f;
	
	vec4 lightModColor = textureLod(LightProjMap, lightSpaceUv, mipSelect);
	vec4 specColor = texture(SpecularBuffer, screenUV, 0);
	vec4 albedoColor = texture(AlbedoBuffer, screenUV, 0);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	
	
	float NL = dot(lightDir, ViewSpaceNormal);
	vec3 diff = LightColor.xyz * saturate(NL) * att;
	
	vec3 H = normalize(lightDir - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, lightDir));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	vec4 oColor = vec4(lightModColor.rgb * final, lightModColor.a);
	
	// shadows
	vec4 shadowProjLightPos = ShadowProjTransform * vec4(surfacePos, 1.0f);
	vec2 shadowLookup = (shadowProjLightPos.xy / shadowProjLightPos.ww) * vec2(0.5f, -0.5f) + 0.5f; 
	shadowLookup.y = 1 - shadowLookup.y;
	float receiverDepth = projLightPos.z / projLightPos.w;
	float shadowFactor = GetInvertedOcclusionSpotLight(receiverDepth, shadowLookup);	
	//shadowFactor = smoothstep(0.0000001f, 1, shadowFactor);
	shadowFactor = saturate(lerp(1.0f, saturate(shadowFactor), ShadowIntensity));
	
	
	Color = EncodeHDR(oColor * shadowFactor);
}
//---------------------------------------------------------------------------------------------------------------------------
//											POINT LIGHT
//---------------------------------------------------------------------------------------------------------------------------

state PointLightStateStandard
{
	BlendEnabled[0] = true;
	SrcBlend[0] = One;
	DstBlend[0] = One;
	CullMode = Front;
	DepthEnabled = true;
	DepthWrite = false;
	DepthFunc = Greater;
};

state PointLightStateShadow
{
	BlendEnabled[0] = true;
	SrcBlend[0] = One;
	DstBlend[0] = One;
	CullMode = Front;
	DepthEnabled = true;
	DepthWrite = false;
	DepthFunc = Greater;
};

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
vsPoint(in vec3 position,
	out vec3 ViewSpacePosition,
	out vec3 WorldPosition) 
{
	vec4 modelSpace = Model * vec4(position, 1);
	gl_Position = ViewProjection * modelSpace;
	WorldPosition = modelSpace.xyz;
	ViewSpacePosition = (View * modelSpace).xyz;
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
psPoint(in vec3 ViewSpacePosition,	
	in vec3 WorldPosition,
	[color0] out vec4 Color) 
{
	vec2 pixelSize = GetPixelSize(DepthBuffer);
	vec2 screenUV = psComputeScreenCoord(gl_FragCoord.xy, pixelSize.xy);
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, screenUV, 0));
	float Depth = textureLod(DepthBuffer, screenUV, 0).r;
	
	vec3 viewVec = normalize(ViewSpacePosition);
	vec3 surfacePos = viewVec * Depth;
	vec3 lightDir = (LightPosRange.xyz - surfacePos);
	vec3 projDir = (InvView * vec4(-lightDir, 0)).xyz;
	vec4 lightModColor = textureLod(LightProjCube, projDir, 0);
	vec4 specColor = texture(SpecularBuffer, screenUV, 0);
	vec4 albedoColor = texture(AlbedoBuffer, screenUV, 0);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	// magic formulae to calculate specular power from color in the range [0..1]
	
	float att = saturate(1.0 - length(lightDir) * LightPosRange.w);
	att *= att;
	lightDir = normalize(lightDir);
	
	float NL = dot(lightDir, ViewSpaceNormal);
	vec3 diff = LightColor.xyz * saturate(NL) * att;
	
	vec3 H = normalize(lightDir - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, lightDir));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	
	vec4 oColor = vec4(lightModColor.rgb * final, lightModColor.a);
	              
	Color = EncodeHDR(oColor);
}

//---------------------------------------------------------------------------------------------------------------------------
/**
*/
shader
void
psPointShadow(in vec3 ViewSpacePosition,	
	in vec3 WorldPosition,
	[color0] out vec4 Color) 
{
	vec2 pixelSize = GetPixelSize(DepthBuffer);
	vec2 screenUV = psComputeScreenCoord(gl_FragCoord.xy, pixelSize.xy);
	vec3 ViewSpaceNormal = UnpackViewSpaceNormal(textureLod(NormalBuffer, screenUV, 0));
	float Depth = textureLod(DepthBuffer, screenUV, 0).r;
	
	vec3 viewVec = normalize(ViewSpacePosition);
	vec3 surfacePos = viewVec * Depth;
	vec3 lightDir = (LightPosRange.xyz - surfacePos);
	vec3 projDir = (InvView * vec4(-lightDir, 0)).xyz;
	vec4 lightModColor = textureLod(LightProjCube, projDir, 0);
	vec4 specColor = texture(SpecularBuffer, screenUV, 0);
	vec4 albedoColor = texture(AlbedoBuffer, screenUV, 0);
	float specPower = ROUGHNESS_TO_SPECPOWER(specColor.a);	
	
	float att = saturate(1.0 - length(lightDir) * LightPosRange.w);
	att *= att;
	lightDir = normalize(lightDir);
	
	float NL = dot(lightDir, ViewSpaceNormal);
	vec3 diff = LightColor.xyz * saturate(NL) * att;
	
	vec3 H = normalize(lightDir - viewVec);
	float NH = saturate(dot(ViewSpaceNormal, H));
	float NV = saturate(dot(ViewSpaceNormal, -viewVec));
	float HL = saturate(dot(H, lightDir));
	vec3 spec;
	BRDFLighting(NH, NL, NV, HL, specPower, specColor.rgb, spec);
	vec3 final = (albedoColor.rgb + spec) * diff;
	
	vec4 oColor = vec4(lightModColor.rgb * final, lightModColor.a);
	
	// shadows
	float shadowFactor = 1.0f;	
	vec4 shadowProjLightPos = ShadowProjTransform * vec4(surfacePos, 1.0f);
	vec3 shadowLookup = (shadowProjLightPos.xyz / shadowProjLightPos.www) * vec3(0.5f, -0.5f, 1.0f) + vec3(0.5f, 0.5f, 0.0f);
	float receiverDepth = shadowProjLightPos.z / shadowProjLightPos.w;
	shadowFactor = GetInvertedOcclusionPointLight(projDir.z,
						projDir);	
	shadowFactor = saturate(lerp(1.0f, saturate(shadowFactor), ShadowIntensity) * att);             
	
	
	Color = EncodeHDR(oColor * shadowFactor);
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(GlobalLight, "Global", vsGlob(), psGlob(), GlobalLightState);
SimpleTechnique(GlobalLightShadow, "Global|Alt0", vsGlob(), psGlobShadow(), GlobalLightState);
SimpleTechnique(SpotLight, "Spot", vsSpot(), psSpot(), SpotLightState);
SimpleTechnique(SpotLightShadow, "Spot|Alt0", vsSpot(), psSpotShadow(), SpotLightState);
SimpleTechnique(PointLight, "Point", vsPoint(), psPoint(), PointLightStateStandard);
SimpleTechnique(PointLightShadow, "Point|Alt0", vsPoint(), psPointShadow(), PointLightStateShadow);
