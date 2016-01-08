//------------------------------------------------------------------------------
//  surfaceconstant.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfaceconstant.h"
#include "coregraphics/shaderinstance.h"
#include "resources/resourcemanager.h"

using namespace Util;
namespace Materials
{
__ImplementClass(Materials::SurfaceConstant, 'SUCO', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SurfaceConstant::SurfaceConstant() :
    system(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SurfaceConstant::~SurfaceConstant()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Sets up a surface constant.

	Since surface constants implements a series of shader variable instances, this function will set up the constant to be 
	applicable in every shader used by the surface.

	The value is set within the shader variable instances directly, however if the shader variable instance use a constant buffer,
	that constant buffer will be updated directly.

	If the value is a texture or buffer, the shader variable instance will apply it whenever we run the SurfaceConstant::Apply.
*/
void
SurfaceConstant::Setup(const StringAtom& name, const Array<Ptr<CoreGraphics::ShaderInstance>>& shaders, const Array<Frame::BatchGroup::Code>& batchCodes)
{
    this->name = name;

    IndexT shaderIndex;
    for (shaderIndex = 0; shaderIndex < shaders.Size(); shaderIndex++)
    {
        // get shader and variable (the variable must exist in the shader!)
        const Ptr<CoreGraphics::ShaderInstance>& shader = shaders[shaderIndex];
        const Ptr<CoreGraphics::ShaderVariableInstance>& var = shader->CreateVariableInstance(this->name);
        const Frame::BatchGroup::Code& code = batchCodes[shaderIndex];
        this->variablesByShader.Add(code, var);

        // set value
        var->SetValue(this->value);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Discard()
{
    this->variablesByShader.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Apply(const Frame::BatchGroup::Code& group)
{
    // skip applying if this constant has no binding to said variable
	if (!this->variablesByShader.Contains(group)) return;
    const Ptr<CoreGraphics::ShaderVariableInstance>& var = this->variablesByShader[group];
    var->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::SetValue(const Util::Variant& value)
{
	this->value = value;
	IndexT i;
	for (i = 0; i < this->variablesByShader.Size(); i++)
	{
		this->variablesByShader.ValueAtIndex(i)->SetValue(value);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::SetTexture(const Ptr<CoreGraphics::Texture>& tex)
{
	this->value.SetObject(tex);
	IndexT i;
	for (i = 0; i < this->variablesByShader.Size(); i++)
	{
		this->variablesByShader.ValueAtIndex(i)->SetTexture(tex);
	}
}

} // namespace Materials