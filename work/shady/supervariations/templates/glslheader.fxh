//------------------------------------------------------------------------------
//  glslheader.fxh
//
//	Base header for AnyFX based shaders
//
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------

#define float2 vec2
#define float3 vec3
#define float4 vec4
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4
#define matrix3x2 mat3x2
#define matrix3x3 mat3x3
#define matrix3x4 mat3x4
#define matrix4x2 mat4x2
#define matrix4x3 mat4x3
#define matrix4x4 mat4x4

struct VertexShaderParameters
{
	vec3 pos;
	vec3 normal;
	
#if VERTEX_COLOR
	vec4 color;
#endif

	vec2 uv;
	
#if SECONDARY_UV
	vec2 uv2;
#endif
	vec3 tangent;
	vec3 binormal;
	
#if SKINNED
	vec4 skinWeights;
	uvec4 skinIndices;
#endif
};

struct PixelShaderParameters
{
	vec3 viewSpacePos;
	vec3 tangent;
	vec3 normal;
	vec3 binormal;
	vec2 uv;
	
#if SECONDARY_UV
	vec2 uv2;
#endif
	
#if VERTEX_COLOR
	vec4 color;
#endif
};

