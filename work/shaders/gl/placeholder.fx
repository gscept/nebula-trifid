//------------------------------------------------------------------------------
//  placeholder.fx
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"
#include "lib/shared.fxh"

sampler2D DiffuseMap;

state PlaceholderState
{
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(in vec3 position,
	in vec3 normal,
	in vec2 uv,
	out vec2 UV) 
{
    gl_Position = ViewProjection * Model * vec4(position, 1);
    UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psMain(in vec2 uv,
	[color0] out vec4 Color)
{
	Color = texture(DiffuseMap, uv);
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Static, "Static", vsMain(), psMain(), PlaceholderState);

// add a skinned variation since the Character system automatically appends the Skinned feature string when rendering characters
SimpleTechnique(Skinned, "Skinned", vsMain(), psMain(), PlaceholderState);
