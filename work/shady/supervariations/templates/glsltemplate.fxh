//------------------------------------------------------------------------------
//  glsltemplate.fxh
//
//	Base template for AnyFX based shaders
//
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "lib/shared.fxh"
#include "lib/util.fxh"

#if ALPHA_TEST
float AlphaSensitivity = 0.0f;
#endif

#ifndef USE_CUSTOM_WORLD_OFFSET
vec4 GetWorldOffset(VertexShaderParameters params)
{
	return vec4(params.pos.xyz, 1);
}
#endif

#ifndef USE_CUSTOM_DIFFUSE
vec4 GetDiffuse(PixelShaderParameters params)
{
	return vec3(0, 0, 0);
}
#endif

#ifndef USE_CUSTOM_NORMAL
vec3 GetNormal(PixelShaderParameters param)
{
	return params.normal;
}
#endif

#ifndef USE_CUSTOM_SPECULAR
vec3 GetSpecular(PixelShaderParams params)
{
	return vec4(0);
}
#endif

#ifndef USE_CUSTOM_EMISSIVE
vec4 GetEmissive(PixelShaderParams params)
{
	return vec4(0);
}
#endif

#ifndef USE_CUSTOM_ROUGHNESS
float GetRoughness(PixelShaderParams params)
{
	return 0.f;
}
#endif

#ifndef USE_CUSTOM_GODRAYS
vec4 GetGodrays(PixelShaderParams params)
{
	return vec4(0);
}
#endif

//------------------------------------------------------------------------------
/**
*/
shader
void
vertexShader(in VertexShaderParameters vertex, out PixelShaderParameters pixel) 
{
	// write uv
	pixel.uv = vertex.uv;
	
	// write vertex colors if present
#ifdef VERTEX_COLOR
	pixel.color = vertex.color;
#endif
	
	// write secondary uv
#ifdef SECONDARY_UV
	pixel.uv2 = vertex.uv2;
#endif
	
    mat4 modelView = View * Model;
	
#ifdef SKINNED
	vec4 offsetted			= GetWorldOffset(vertex);
	vec4 skinnedPos			= SkinnedPosition(offsetted, vertex.weights, vertex.indices);
	vec4 skinnedNormal		= SkinnedNormal(vertex.normal, vertex.weights, vertex.indices);
	vec4 skinnedTangent		= SkinnedNormal(vertex.tangent, vertex.weights, vertex.indices);
	vec4 skinnedBinormal	= SkinnedNormal(vertex.binormal, vertex.weights, vertex.indices);
	skinnedPos = ViewProjection * Model * skinnedPos;
	
	pixel.viewSpacePos 		= (modelView * vec4(skinnedPos, 1)).xyz;
	pixel.tangent 			= (modelView * vec4(skinnedTangent, 0)).xyz;
	pixel.normal 			= (modelView * vec4(skinnedNormal, 0)).xyz;
	pixel.binormal 			= (modelView * vec4(skinnedBinormal, 0)).xyz;
	
// no skin
#else
	vec4 offsetted = GetWorldOffset(vertex);
	offsetted = ViewProjection * Model * vec4(offsetted, 1);
	
    pixel.viewSpacePos 		= (modelView * vec4(offsetted, 1)).xyz;
	pixel.tangent 			= (modelView * vec4(vertex.tangent, 0)).xyz;
	pixel.normal 			= (modelView * vec4(vertex.normal, 0)).xyz;
	pixel.binormal 			= (modelView * vec4(vertex.binormal, 0)).xyz;
#endif
	
	gl_Position = offsetted;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
pixelShader(PixelShaderParams params,
			#ifdef UNLIT_SHADING
			[color0] out vec4 Albedo
			#else
			[color0] out vec4 Albedo,
			[color1] out vec4 Normals,
			[color2] out float Depth,	
			[color3] out vec4 Specular,
			[color4] out vec4 Emissive,
			[color5] out vec4 Unshaded
			#endif
			)
{
// unlit material
#ifdef UNLIT_SHADING
	vec4 diffuse = GetDiffuse(params);
	
	#if ALPHA_TEST
		if (diffuse.a < AlphaSensitivity) discard;
	#endif
	
	Albedo = EncodeHDR(diffuse);
#else
	// default to lit material
	vec4 diffuse = GetDiffuse(params);
	
	#ifdef ALPHA_TEST
		if (diffuse.a < AlphaSensitivity) discard;
	#endif
	
	Albedo = diffuse;
	
	// normal map calculation
	vec3 normal = GetNormal(params);
	mat3 tangentViewMatrix = mat3(normalize(params.tangent), normalize(params.binormal), normalize(params.normal));        
	vec3 tNormal = vec3(0,0,0);
	tNormal.xy = (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z = saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
	Normals = PackViewSpaceNormal(tangentViewMatrix * tNormal);
	
	// emissive
	vec4 emissive = GetEmissive(params);
	Emissive = EncodeHDR(emissive);
	
	// specular
	vec4 specular = GetSpecular(params);
	float roughness = GetRoughness(params);
	Specular = vec4(specular.rgb, roughness);

	// depth
	Depth = length(ViewSpacePos);
	
	// godrays
	vec4 godrays = GetGodrays(params);
	Unshaded = godrays;
#endif
}

// define a standard state
// ideally, we want to generate the draw state from some settings, but this will work as a placeholder
#ifndef USE_CUSTOM_STATE
state StandardState
{
};
#endif

#if DEFAULT_PROGRAM
SimpleTechnique(Main, "Generated", vertexShader, pixelShader, StandardState);
#elif GEOMETRY_PROGRAM
GeometryTechnique(Main, "Generated", vertexShader, pixelShader, geometryShader, StandardState);
#elif TESSELLATION_PROGRAM
TessellationTechnique(Main, "Generated", vertexShader, pixelShader, hullShader, domainShader, StandardState);
#elif GEOMETRY_TESSELLATION_PROGRAM
FullTechnique(Main, "Generated", vertexShader, pixelShader, hullShader, domainShader, geometryShader, StandardState);
#endif