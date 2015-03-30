#pragma once
//------------------------------------------------------------------------------
/**
    @file shadydefs
    
    Contains defines and enums global in Shady.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

namespace Shady
{
enum Shader
{
	VertexShader,
	HullShader,
	DomainShader,
	GeometryShader,
	PixelShader,
	ComputeShader,

	NumShaders,

	UnknownShader
};

enum Language
{
	GLSL,
	HLSL,
	PS3,
	WII,
    LUA,            // this together with javascript are probably only going to be used in order to visualize the process
    JS,

	NumLanguages,

	UnknownLanguage
};

enum OutputLanguage
{
    AnyFX_GLSL,
    AnyFX_HLSL,
    FX_HLSL,
    LUA_SCRIPT,
    JS_SCRIPT
};

}