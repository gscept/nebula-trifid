//------------------------------------------------------------------------------
//  geometrybase.fxh
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/skinning.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/defaultsamplers.fxh"

//#define PN_TRIANGLES

// textures
samplerCube EnvironmentMap;

// samplers
samplerstate GeometryTextureSampler
{
	Samplers = { SpecularMap, EmissiveMap, NormalMap, DiffuseMap, DisplacementMap, RoughnessMap };
	Filter = MinMagMipLinear;
	AddressU = Wrap;
	AddressV = Wrap;
};


samplerstate EnvironmentSampler
{
	Samplers = { EnvironmentMap };
	Filter = MinMagMipLinear;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};


state StandardState
{
};

state StandardNoCullState
{
	CullMode = None;
};

state AlphaState
{
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	DepthWrite = false;
};
	
// declare commonly used prototypes
prototype vec3 CalculateBump(in vec3 tangent, in vec3 binormal, in vec3 normal, in vec4 bump);
prototype float CalculateDepth(in vec3 viewSpacePosition);
prototype vec4 CalculateEmissive(in vec4 emissiveColor, in float emissiveIntensity);
prototype vec4 CalculateSpecular(in vec3 specularColor, in float specularIntensity, in vec3 environmentColor, in float roughness);
prototype vec3 CalculateEnvironment(in vec3 specularColor, in samplerCube environmentMap, in vec3 viewSpaceNormal, in vec3 viewSpacePos, in vec4 eyePos, in mat4 invView, in float roughness);
prototype vec4 CalculateColor(in vec4 albedoColor, in vec3 environmentColor, in vec4 color, in float roughness, in float alphaBlend);

// declare commonly used functions
subroutine (CalculateBump) vec3 NormalMapFunctor(
	in vec3 tangent, 
	in vec3 binormal, 
	in vec3 normal, 
	in vec4 bumpData)
{
	mat3 tangentViewMatrix = mat3(normalize(tangent.xyz), normalize(binormal.xyz), normalize(normal.xyz));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (bumpData.ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	return tangentViewMatrix * tNormal;
}

subroutine (CalculateDepth) float ViewSpaceDepthFunctor(in vec3 viewSpacePosition)
{
	return length(viewSpacePosition);
}

subroutine (CalculateEmissive) vec4 IntensityEmissiveFunctor(in vec4 color, in float intensity)
{
	return color * intensity;
}

subroutine (CalculateSpecular) vec4 NonReflectiveSpecularFunctor(
	in vec3 specularColor, 
	in float specularIntensity, 
	in vec3 environmentColor, 
	in float roughness)
{
	return vec4(specularColor * specularIntensity, roughness);
}

subroutine (CalculateSpecular) vec4 ReflectiveSpecularFunctor(
	in vec3 specularColor, 
	in float specularIntensity, 
	in vec3 environmentColor, 
	in float roughness)
{
	return vec4(specularColor * specularIntensity * environmentColor, roughness);
}

subroutine (CalculateEnvironment) vec3 EnvironmentPBR(
	in vec3 specularColor, 
	in samplerCube environmentMap, 
	in vec3 viewSpaceNormal, 
	in vec3 viewSpacePos, 
	in vec4 eyePos, 
	in mat4 invView, 
	in float roughness)
{
	vec3 worldViewVec = normalize(eyePos.xyz - (invView * vec4(viewSpacePos, 1)).xyz);	
	vec3 reflectVec = reflect(-worldViewVec, (invView * vec4(viewSpaceNormal, 0)).xyz);
	float x = dot(viewSpaceNormal, normalize(viewSpacePos.xyz));
	vec3 rim = specularColor + (1 - specularColor) * (pow((1 - x), 5) / (4 - 3 * roughness));
	vec3 envColor = textureLod(environmentMap, reflectVec, (1 - roughness) * 9).rgb * saturate(rim);
	return envColor;	
}

subroutine (CalculateEnvironment) vec3 NoEnvironment(
	in vec3 specularColor, 
	in samplerCube environmentMap, 
	in vec3 viewSpaceNormal, 
	in vec3 viewSpacePos, 
	in vec4 eyePos, 
	in mat4 invView, 
	in float roughness)
{
	return vec3(1);
}

subroutine (CalculateColor) vec4 SimpleColor(
	in vec4 albedoColor, 
	in vec3 environmentColor, 
	in vec4 color, 
	in float roughness, 
	in float alphaBlend) 
{
	return albedoColor;
}

subroutine (CalculateColor) vec4 SimpleColorMultiply(
	in vec4 albedoColor, 
	in vec3 environmentColor, 
	in vec4 color, 
	in float roughness, 
	in float alphaBlend) 
{
	return albedoColor * color;
}

subroutine (CalculateColor) vec4 AlphaColor(
	in vec4 albedoColor, 
	in vec3 environmentColor, 
	in vec4 color, 
	in float roughness, 
	in float alphaBlend) 
{
	return albedoColor * alphaBlend;
}

subroutine (CalculateColor) vec4 AlphaColorMultiply(
	in vec4 albedoColor, 
	in vec3 environmentColor, 
	in vec4 color, 
	in float roughness, 
	in float alphaBlend) 
{
	return albedoColor * color * alphaBlend;
}

subroutine (CalculateColor) vec4 EnvironmentMapColor(
	in vec4 albedoColor, 
	in vec3 environmentColor, 
	in vec4 color, 
	in float roughness, 
	in float alphaBlend) 
{
	vec4 result = albedoColor;
	result.rgb = result.rgb * lerp(vec3(1), environmentColor, roughness);
	return result;
}

// declare subroutine variables
CalculateBump calculateBump;
CalculateDepth calculateDepth;
CalculateEmissive calculateEmissive;
CalculateSpecular calculateSpecular;
CalculateEnvironment calculateEnvironment;
CalculateColor calculateColor;

//------------------------------------------------------------------------------
/**
				STATIC GEOMETRY
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
shader
void
vsStatic(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV) 
{
    gl_Position = ViewProjection * Model * vec4(position, 1);
    UV = uv;
	
    mat4 modelView = View * Model;
    ViewSpacePos = (modelView * vec4(position, 1)).xyz;
	Tangent 	= (modelView * vec4(tangent, 0)).xyz;
	Normal 		= (modelView * vec4(normal, 0)).xyz;
	Binormal 	= (modelView * vec4(binormal, 0)).xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsStaticInstanced(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,
	out vec2 UV) 
{
    gl_Position = ViewProjection * ModelArray[gl_InstanceID] * vec4(position, 1);
    UV = uv;
	
	mat4 modelView = View * ModelArray[gl_InstanceID];
    ViewSpacePos = (modelView * vec4(position, 1)).xyz;
	Tangent = (modelView * vec4(tangent, 0)).xyz;
	Normal = (modelView * vec4(normal, 0)).xyz;
	Binormal = (modelView * vec4(binormal, 0)).xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsStaticTessellated(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	out vec3 Tangent,
	out vec3 Normal,
	out vec4 Position,
	out vec3 Binormal,
	out vec2 UV,
	out float Distance) 
{
    Position = Model * vec4(position, 1);
    UV = uv;    
    
	Tangent = (Model * vec4(tangent, 0)).xyz;
	Normal = (Model * vec4(normal, 0)).xyz;
	Binormal = (Model * vec4(binormal, 0)).xyz;
	
	float vertexDistance = distance( Position.xyz, EyePos.xyz );
	Distance = 1.0 - clamp( ( (vertexDistance - MinDistance) / (MaxDistance - MinDistance) ), 0.0, 1.0 - 1.0/TessellationFactor);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsStaticColored(in vec3 position,
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
	out vec4 Color) 
{
	gl_Position = ViewProjection * Model * vec4(position, 1);
	UV = uv;
	Color = color;

	mat4 modelView = View * Model;
	ViewSpacePos = (modelView * vec4(position, 1)).xyz;
	Tangent 	= (modelView * vec4(tangent, 0)).xyz;
	Normal 		= (modelView * vec4(normal, 0)).xyz;
	Binormal 	= (modelView * vec4(binormal, 0)).xyz;
}


//------------------------------------------------------------------------------
/**
				SKINNED GEOMETRY
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
shader
void
vsSkinned(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	in vec4 weights,
	in uvec4 indices,
	out vec3 ViewSpacePos,
	out vec3 Tangent,
	out vec3 Normal,
	out vec3 Binormal,	
	out vec2 UV)	
{	
	vec4 skinnedPos      = SkinnedPosition(position, weights, indices);
	vec4 skinnedNormal   = SkinnedNormal(normal, weights, indices);
	vec4 skinnedTangent  = SkinnedNormal(tangent, weights, indices);
	vec4 skinnedBinormal = SkinnedNormal(binormal, weights, indices);
	
	gl_Position = ViewProjection * Model * skinnedPos;
	UV = uv;
	
	mat4 modelView = mul(Model, View);
    ViewSpacePos = (modelView * skinnedPos).xyz;
	Tangent = (modelView * skinnedTangent).xyz;
	Normal = (modelView * skinnedNormal).xyz;
	Binormal = (modelView * skinnedBinormal).xyz;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsSkinnedTessellated(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	in vec4 weights,
	in uvec4 indices,
	out vec3 Tangent,
	out vec3 Normal,
	out vec4 Position,
	out vec3 Binormal,
	out vec2 UV,
	out float Distance) 
{
	vec4 skinnedPos      = SkinnedPosition(position, weights, indices);
	vec4 skinnedNormal   = SkinnedNormal(normal, weights, indices);
	vec4 skinnedTangent  = SkinnedNormal(tangent, weights, indices);
	vec4 skinnedBinormal = SkinnedNormal(binormal, weights, indices);
	
	Position = Model * skinnedPos;
	UV = uv;    
	
	Tangent = (Model * skinnedTangent).xyz;
	Normal = (Model * skinnedNormal).xyz;
	Binormal = (Model * skinnedBinormal).xyz;
	
	float vertexDistance = distance( Position.xyz, EyePos.xyz );
	Distance = 1.0 - clamp( ( (vertexDistance - MinDistance) / (MaxDistance - MinDistance) ), 0.0, 1.0 - 1.0/TessellationFactor);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
vsBillboard(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	in vec3 tangent,
	in vec3 binormal,
	out vec2 UV)
{
	gl_Position = ViewProjection * Model * vec4(position, 0);
	UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
[inputvertices] = 3
[outputvertices] = 6
shader
void 
hsDefault(in vec3 tangent[],
		  in vec3 normal[],
		  in vec4 position[],
		  in vec3 binormal[],
		  in vec2 uv[],
		  in float distance[],
		  out vec3 Tangent[],
		  out vec3 Normal[],
		  out vec4 Position[],
		  out vec3 Binormal[],
		  out vec2 UV[]
#if PN_TRIANGLES
,
		  patch out vec3 f3B210,
		  patch out vec3 f3B120,
		  patch out vec3 f3B021,
		  patch out vec3 f3B012,
		  patch out vec3 f3B102,
		  patch out vec3 f3B201,
		  patch out vec3 f3B111
#endif
		  )
{
	Tangent[gl_InvocationID] 	= tangent[gl_InvocationID];
	Normal[gl_InvocationID] 	= normal[gl_InvocationID];
	Position[gl_InvocationID]	= position[gl_InvocationID];
	Binormal[gl_InvocationID] 	= binormal[gl_InvocationID];
	UV[gl_InvocationID]			= uv[gl_InvocationID];
	
	// perform per-patch operation
	if (gl_InvocationID == 0)
	{
		vec4 EdgeTessFactors;
		EdgeTessFactors.x = 0.5 * (distance[1] + distance[2]);
		EdgeTessFactors.y = 0.5 * (distance[2] + distance[0]);
		EdgeTessFactors.z = 0.5 * (distance[0] + distance[1]);
		EdgeTessFactors *= TessellationFactor;
	
#if PN_TRIANGLES
		// compute the cubic geometry control points
		// edge control points
		f3B210 = ( ( 2.0f * position[0] ) + position[1] - ( dot( ( position[1] - position[0] ), normal[0] ) * normal[0] ) ) / 3.0f;
		f3B120 = ( ( 2.0f * position[1] ) + position[0] - ( dot( ( position[0] - position[1] ), normal[1] ) * normal[1] ) ) / 3.0f;
		f3B021 = ( ( 2.0f * position[1] ) + position[2] - ( dot( ( position[2] - position[1] ), normal[1] ) * normal[1] ) ) / 3.0f;
		f3B012 = ( ( 2.0f * position[2] ) + position[1] - ( dot( ( position[1] - position[2] ), normal[2] ) * normal[2] ) ) / 3.0f;
		f3B102 = ( ( 2.0f * position[2] ) + position[0] - ( dot( ( position[0] - position[2] ), normal[2] ) * normal[2] ) ) / 3.0f;
		f3B201 = ( ( 2.0f * position[0] ) + position[2] - ( dot( ( position[2] - position[0] ), normal[0] ) * normal[0] ) ) / 3.0f;
		// center control point
		vec3 f3E = ( f3B210 + f3B120 + f3B021 + f3B012 + f3B102 + f3B201 ) / 6.0f;
		vec3 f3V = ( indata[0].Position + indata[1].Position + indata[2].Position ) / 3.0f;
		f3B111 = f3E + ( ( f3E - f3V ) / 2.0f );
#endif

		gl_TessLevelOuter[0] = EdgeTessFactors.x;
		gl_TessLevelOuter[1] = EdgeTessFactors.y;
		gl_TessLevelOuter[2] = EdgeTessFactors.z;
		gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) / 3;
	}
}

//------------------------------------------------------------------------------
/**
*/
[inputvertices] = 6
[winding] = ccw
[topology] = triangle
[partition] = odd
shader
void
dsDefault(
	in vec3 tangent[],
	in vec3 normal[],
	in vec4 position[],
	in vec3 binormal[],
	in vec2 uv[],
	out vec3 ViewSpacePos, 
	out vec3 Tangent, 
	out vec3 Normal, 
	out vec3 Binormal, 
	out vec2 UV
#if PN_TRIANGLES
	,
	patch in vec3 f3B210,
	patch in vec3 f3B120,
	patch in vec3 f3B021,
	patch in vec3 f3B012,
	patch in vec3 f3B102,
	patch in vec3 f3B201,
	patch in vec3 f3B111
#endif
	)
{
	// The barycentric coordinates
	float fU = gl_TessCoord.z;
	float fV = gl_TessCoord.x;
	float fW = gl_TessCoord.y;
	
	// Precompute squares and squares * 3 
	float fUU = fU * fU;
	float fVV = fV * fV;
	float fWW = fW * fW;
	float fUU3 = fUU * 3.0f;
	float fVV3 = fVV * 3.0f;
	float fWW3 = fWW * 3.0f;
	
#ifdef PN_TRIANGLES
	// Compute position from cubic control points and barycentric coords
	vec3 Position = position[0] * fWW * fW + position[1] * fUU * fU + position[2] * fVV * fV +
					  f3B210 * fWW3 * fU + f3B120 * fW * fUU3 + f3B201 * fWW3 * fV + f3B021 * fUU3 * fV +
					  f3B102 * fW * fVV3 + f3B012 * fU * fVV3 + f3B111 * 6.0f * fW * fU * fV;
#else
	vec3 Position = gl_TessCoord.x * position[0].xyz + gl_TessCoord.y * position[1].xyz + gl_TessCoord.z * position[2].xyz;
#endif

	UV = gl_TessCoord.x * uv[0] + gl_TessCoord.y * uv[1] + gl_TessCoord.z * uv[2];
	float Height = 2.0f * length(texture(DisplacementMap, UV)) - 1.0f;
	Normal = gl_TessCoord.x * normal[0] + gl_TessCoord.y * normal[1] + gl_TessCoord.z * normal[2];
	vec3 VectorNormalized = normalize( Normal );
	
	Position += VectorNormalized.xyz * HeightScale * SceneScale * Height;
	
	gl_Position = vec4(Position, 1);
	ViewSpacePos = (View * gl_Position).xyz;
	gl_Position = ViewProjection * gl_Position;
	
	Normal = (View * vec4(Normal, 0)).xyz;
	Binormal = gl_TessCoord.x * binormal[0] + gl_TessCoord.y * binormal[1] + gl_TessCoord.z * binormal[2];
	Binormal = (View * vec4(Binormal, 0)).xyz;
	Tangent = gl_TessCoord.x * tangent[0] + gl_TessCoord.y * tangent[1] + gl_TessCoord.z * tangent[2];
	Tangent = (View * vec4(Tangent, 0)).xyz;
}

//------------------------------------------------------------------------------
/**
	Ubershader for standard geometry
*/
shader
void
psUber(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;
	vec4 normals = texture(NormalMap, UV);
		
	vec3 bumpNormal = calculateBump(Tangent, Binormal, Normal, normals);
	Normals = PackViewSpaceNormal(bumpNormal);
		
	vec3 environment = calculateEnvironment(specColor.rgb, EnvironmentMap, bumpNormal, ViewSpacePos, EyePos, InvView, roughness);
	Emissive = EncodeHDR(calculateEmissive(emsvColor, MatEmissiveIntensity));
	Specular = calculateSpecular(specColor.rgb, MatSpecularIntensity, environment, roughness);
	Depth = calculateDepth(ViewSpacePos);	
	
	Albedo = calculateColor(diffColor, environment, vec4(1), roughness, AlphaBlendFactor);
	Unshaded = vec4(0,0,0,1);
}

//------------------------------------------------------------------------------
/**
	Ubershader for standard geometry.
	Tests for alpha clipping
*/
shader
void
psUberAlphaTest(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;
	vec4 normals = texture(NormalMap, UV);
	
	vec3 bumpNormal = calculateBump(Tangent, Binormal, Normal, normals);
	
	vec3 environment = calculateEnvironment(specColor.rgb, EnvironmentMap, bumpNormal, ViewSpacePos, EyePos, InvView, roughness);
	vec4 color = calculateColor(diffColor, environment, vec4(1), roughness, AlphaBlendFactor);
	
	if (color.a < AlphaSensitivty) discard;
	Albedo = color;
	
	Emissive = EncodeHDR(calculateEmissive(emsvColor, MatEmissiveIntensity));
	Specular = calculateSpecular(specColor.rgb, MatSpecularIntensity, environment, roughness);
	Depth = calculateDepth(ViewSpacePos);	
	Normals = PackViewSpaceNormal(bumpNormal);
	
	Unshaded = vec4(0,0,0,1);
}

//------------------------------------------------------------------------------
/**
	Ubershader for standard geometry
*/
shader
void
psUberVertexColor(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	in vec4 Color,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;
	vec4 normals = texture(NormalMap, UV);
		
	vec3 bumpNormal = calculateBump(Tangent, Binormal, Normal, normals);
	Normals = PackViewSpaceNormal(bumpNormal);
		
	vec3 environment = calculateEnvironment(specColor.rgb, EnvironmentMap, bumpNormal, ViewSpacePos, EyePos, InvView, roughness);
	Emissive = EncodeHDR(calculateEmissive(emsvColor, MatEmissiveIntensity));
	Specular = calculateSpecular(specColor.rgb, MatSpecularIntensity, environment, roughness);
	Depth = calculateDepth(ViewSpacePos);	
			
	Albedo = calculateColor(diffColor, environment, Color, roughness, AlphaBlendFactor);
	Unshaded = vec4(0,0,0,1);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psDefault(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;

	Emissive = EncodeHDR(emsvColor * MatEmissiveIntensity);
	Specular = vec4(specColor.rgb * MatSpecularIntensity, roughness);
	Albedo = diffColor;
	Unshaded = vec4(0,0,0,1);
	Depth = length(ViewSpacePos);
	mat3 tangentViewMatrix = mat3(normalize(Tangent), normalize(Binormal), normalize(Normal));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	
	Normals = PackViewSpaceNormal(tangentViewMatrix * tNormal);

	float alpha = Albedo.a;
	if (alpha < AlphaSensitivity) discard;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psTransulcent(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	float alpha = diffColor.a;
	if (alpha < AlphaSensitivity) discard;
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;

	Emissive = EncodeHDR(emsvColor * MatEmissiveIntensity);
	Specular = vec4(specColor.rgb * MatSpecularIntensity, roughness);
	Albedo = diffColor;
	Unshaded = vec4(0,0,0,1);
	Depth = length(ViewSpacePos);
	
	vec3 normal = Normal;
	if (!gl_FrontFacing) normal.z = -normal.z;
	mat3 tangentViewMatrix = mat3(normalize(Tangent), normalize(Binormal), normalize(normal));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	
	
	Normals = PackViewSpaceNormal(tangentViewMatrix * tNormal);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psAlpha(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;

	Emissive = EncodeHDR(emsvColor * MatEmissiveIntensity);
	Specular = vec4(specColor.rgb * MatSpecularIntensity, roughness);
	Albedo = vec4(diffColor.rgb, diffColor.a * AlphaBlendFactor);
	Unshaded = vec4(0,0,0,1);
	Depth = length(ViewSpacePos);
	mat3 tangentViewMatrix = mat3(normalize(Tangent.xyz), normalize(Binormal.xyz), normalize(Normal.xyz));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	
	Normals = PackViewSpaceNormal((tangentViewMatrix * tNormal).xyz);

	float alpha = Albedo.a;
	if (alpha < AlphaSensitivity) discard;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psEnvironment(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;

	Emissive = EncodeHDR(emsvColor * MatEmissiveIntensity);

	Depth = length(ViewSpacePos.xyz);
	mat3 tangentViewMatrix = mat3(normalize(Tangent.xyz), normalize(Binormal.xyz), normalize(Normal.xyz));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	
	vec3 viewSpaceNormal = (tangentViewMatrix * tNormal).xyz;
	vec3 worldViewVec = normalize(EyePos.xyz - (InvView * vec4(ViewSpacePos, 1)).xyz);	
	vec3 reflectVec = reflect(-worldViewVec, (InvView * vec4(viewSpaceNormal, 0)).xyz);
	float x = dot(viewSpaceNormal, normalize(ViewSpacePos.xyz));
	vec4 rim = vec4(specColor.rgb + (1 - specColor.rgb) * (pow((1 - x), 5) / (4 - 3 * roughness)), 1) * 1000;
	vec4 envColor = textureLod(EnvironmentMap, reflectVec, (1 - roughness) * 9) * saturate(rim);
	
	Specular = vec4(specColor.rgb * MatSpecularIntensity * envColor.rgb, roughness);
	Albedo = diffColor * lerp(vec4(1), envColor, roughness);
	Unshaded = vec4(0,0,0,1);
	Normals = PackViewSpaceNormal(viewSpaceNormal.xyz);

	float alpha = Albedo.a;
	if (alpha < AlphaSensitivity) discard;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psVertexColor(in vec3 ViewSpacePos,
	in vec3 Tangent,
	in vec3 Normal,
	in vec3 Binormal,
	in vec2 UV,
	in vec4 Color,
	[color0] out vec4 Albedo,
	[color1] out vec4 Normals,
	[color2] out float Depth,	
	[color3] out vec4 Specular,
	[color4] out vec4 Emissive,
	[color5] out vec4 Unshaded) 
{
	vec4 diffColor = texture(DiffuseMap, UV);
	vec4 emsvColor = texture(EmissiveMap, UV);
	vec4 specColor = texture(SpecularMap, UV);
	float roughness = texture(RoughnessMap, UV).r;
	
	mat3 tangentViewMatrix = mat3(normalize(Tangent.xyz), normalize(Binormal.xyz), normalize(Normal.xyz));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	Normals = PackViewSpaceNormal(tNormal);
	
	vec3 worldViewVec = normalize(EyePos.xyz - (InvView * vec4(ViewSpacePos, 1)).xyz);	
	vec3 reflectVec = reflect(-worldViewVec, (InvView * vec4(tNormal, 0)).xyz);
	float x = dot(tNormal, normalize(ViewSpacePos.xyz));
	vec3 rim = specColor.rgb + (1 - specColor.rgb) * (pow((1 - x), 5) / (4 - 3 * roughness));
	vec3 environment = textureLod(EnvironmentMap, reflectVec, (1 - roughness) * 9).rgb * saturate(rim);

	Emissive = EncodeHDR(emsvColor * MatEmissiveIntensity);
	Specular = vec4(specColor.rgb * MatSpecularIntensity * environment, roughness);
	Depth = length(ViewSpacePos);	
	
	Albedo = vec4(diffColor.rgb * lerp(vec3(1), environment, roughness), diffColor.a);
	Unshaded = vec4(0,0,0,1);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psBillboard(in vec2 UV,
			[color0] out vec4 Albedo)
{
	// get diffcolor
	vec4 diffColor = texture(DiffuseMap, UV);
	
	Albedo = diffColor;
}
