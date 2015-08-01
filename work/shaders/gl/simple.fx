//------------------------------------------------------------------------------
//  simple.fx
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/shared.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"

mat4 ShapeModel;
vec4 MatDiffuse;
state WireframeState
{
	CullMode = None;	
	BlendEnabled[0] = true;	
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	FillMode = Line;
	MultisampleEnabled = true;
};

state DepthEnabledState
{
	CullMode = None;	
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;	
	DstBlend[0] = OneMinusSrcAlpha;
	DepthEnabled = true;
	DepthWrite = true;
	MultisampleEnabled = true;
};

state DepthDisabledState
{
	CullMode = None;	
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	DepthEnabled = false;
	DepthWrite = false;
	MultisampleEnabled = true;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(in vec4 position, [slot=5] in vec4 color, out vec4 Color)  
{
	gl_Position = ViewProjection * ShapeModel * position;
	Color = color;
}
	
//------------------------------------------------------------------------------
/**
*/
shader
void
psMain(in vec4 color, [color0] out vec4 Color) 
{
	Color = color * MatDiffuse;
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Default, "Static", vsMain(), psMain(), DepthDisabledState);
SimpleTechnique(Depth, "Static|Alt0", vsMain(), psMain(), DepthEnabledState);
SimpleTechnique(Wireframe, "Static|Alt1", vsMain(), psMain(), WireframeState);