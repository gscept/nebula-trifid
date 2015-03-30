//------------------------------------------------------------------------------
//  billboard.fx
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/shared.fxh"
#include "lib/techniques.fxh"
#include "lib/util.fxh"

sampler2D DiffuseMap;
vec4 Color = vec4(1,1,1,1);

samplerstate BillboardSampler
{
	Samplers = { DiffuseMap };
};

state BillboardState
{
	CullMode = None;
};


//------------------------------------------------------------------------------
/**
*/
shader
void
vsDefault(in vec2 position,
	in vec2 uv,
	out vec2 UV) 
{
    gl_Position = ViewProjection * Model * vec4(position, 0, 1);
    UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psDefault(in vec2 UV,
		[color0] out vec4 Albedo)
{
	// get diffcolor
	vec4 diffColor = texture(DiffuseMap, UV) * Color;
	
	float alpha = diffColor.a;
	if (alpha < 0.5f) discard;
	
	Albedo = EncodeHDR(diffColor);
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Render, "Static", vsDefault(), psDefault(), BillboardState);
