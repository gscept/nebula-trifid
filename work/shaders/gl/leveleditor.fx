//------------------------------------------------------------------------------
//  simple.fx
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/shared.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"


vec4 MatDiffuse;
state PickingState
{
	CullMode = None;
	DepthWrite = false;
	DepthEnabled = false;
	FillMode = Line;
};


//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(in vec4 position)
{
	gl_Position = ViewProjection * Model * position;
}

//------------------------------------------------------------------------------
/**
*/
[earlydepth]
shader
void
psPicking([color0] out float Id) 
{
	Id = float(ObjectId);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psColor([color0] out vec4 Color) 
{
	Color = MatDiffuse;
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Color, "Alt0", vsMain(), psColor(), PickingState);
SimpleTechnique(Picking, "Alt1", vsMain(), psPicking(), PickingState);