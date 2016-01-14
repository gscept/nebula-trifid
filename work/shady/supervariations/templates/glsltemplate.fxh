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
vec4 GetWorldOffset(in vec4 position)
{
	return vec4(position.xyz, 1);
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

//------------------------------------------------------------------------------
/**
*/
shader
void
vertexShader(
	in vec3 vertexshader_input_position,
	in vec3 vertexshader_input_normal,
	in vec2 vertexshader_input_uv,
	in vec3 vertexshader_input_tangent,
	in vec3 vertexshader_input_binormal,
	#if USE_VERTEX_COLOR
	[slot=5] in vec4 vertexshader_input_color,
	#endif
	#if USE_SECONDARY_UV
	[slot=6] in vec2 vertexshader_input_uv2,
	#endif
	#if USE_SKINNING
	[slot=7] in vec4 vertexshader_input_weights,
	[slot=8] in uvec4 vertexshader_input_indices,
	#endif
	out PixelShaderParameters pixel) 
{
	// write uv
	pixel.uv = vertex.uv;
	
	// write vertex colors if present
#ifdef USE_VERTEX_COLOR
	pixel.color = vertexshader_input_color;
#endif
	
	// write secondary uv
#ifdef USE_SECONDARY_UV
	pixel.uv2 = vertexshader_input_uv2;
#endif
	
    mat4 modelView = View * Model;
	
#ifdef USE_SKINNING
	vec4 offsetted			= GetWorldOffset(vertexshader_input_position);
	offsetted				= SkinnedPosition(offsetted, vertexshader_input_weights, vertexshader_input_indices);
	vec4 skinnedNormal		= SkinnedNormal(vertexshader_input_normal, vertexshader_input_weights, vertexshader_input_indices);
	vec4 skinnedTangent		= SkinnedNormal(vertexshader_input_tangent, vertexshader_input_weights, vertexshader_input_indices);
	vec4 skinnedBinormal	= SkinnedNormal(vertexshader_input_binormal, vertexshader_input_weights, vertexshader_input_indices);
	
	// write outputs
	pixel.viewSpacePos 		= (modelView * vec4(offsetted, 1)).xyz;
	pixel.normal 			= (modelView * vec4(skinnedNormal, 0)).xyz;
	pixel.tangent 			= (modelView * vec4(skinnedTangent, 0)).xyz;
	pixel.binormal 			= (modelView * vec4(skinnedBinormal, 0)).xyz;
	
// no skin
#else
	vec4 offsetted = GetWorldOffset(vertex);
	
    pixel.viewSpacePos 		= (modelView * vec4(offsetted, 1)).xyz;
	pixel.normal 			= (modelView * vec4(vertexshader_input_normal, 0)).xyz;
	pixel.tangent 			= (modelView * vec4(vertexshader_input_tangent, 0)).xyz;	
	pixel.binormal 			= (modelView * vec4(vertexshader_input_binormal, 0)).xyz;
#endif
	offsetted 				= Model * offsetted;
	pixel.worldViewVec		= offsetted.xyz - EyePos.xyz;
	gl_Position 			= ViewProjection * offsetted;
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
			[color4] out vec4 Emissive
			#endif
			)
{
// unlit material
#ifdef UNLIT_SHADING
	vec4 diffuse 			= GetDiffuse(params);
	
	#if ALPHA_TEST
		if (diffuse.a < AlphaSensitivity) discard;
	#endif
	
	Albedo 					= EncodeHDR(diffuse);
#else
	// default to lit material
	vec4 diffuse 			= GetDiffuse(params);
	
	#ifdef ALPHA_TEST
		if (diffuse.a < AlphaSensitivity) discard;
	#endif
		
	// normal map calculation
	vec3 normal 			= GetNormal(params);
	mat3 tangentViewMatrix 	= mat3(normalize(params.tangent), normalize(params.binormal), normalize(params.normal));        
	vec3 tNormal 			= vec3(0,0,0);
	tNormal.xy 				= (texture(NormalMap, UV).ag * 2.0f) - 1.0f;
	tNormal.z 				= saturate(sqrt(1.0f - dot(tNormal.xy, tNormal.xy)));
		
	// specular
	vec4 specular 			= GetSpecular(params);
	float roughness 		= GetRoughness(params);
	
	// get emissive
	vec4 emissive 			= GetEmissive(params);
	
	// write outputs
	#if USE_PBR_REFLECTIONS
	mat2x3 env 				= PBR(specular, params.normal, params.viewSpacePos, params.worldViewVec, InvView, roughness);
	Emissive 				= vec4(diffuse.rgb * env[0] + env[1], 0) + emissive;
	#else
	Emissive 				= emissive;
	#endif
	
	// save outputs which isn't associated with the PBR method
	Specular 				= vec4(specular.rgb, roughness);
	Depth 					= length(ViewSpacePos);	
	Normals 				= PackViewSpaceNormal(tangentViewMatrix * tNormal);
	
	#if USE_PBR_REFLECTIONS
	Albedo 					= vec4(diffuse.rgb * (1 - spec.rgb), diffuse.a);
	#else
	Albedo 					= diffuse;
	#endif
	
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