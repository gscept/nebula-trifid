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

        // set value directly, this will effectively setup the constant
        var->SetValue(this->value);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Discard()
{
    IndexT i;
    for (i = 0; i < this->variablesByShader.Size(); i++)
    {
        this->variablesByShader.ValueAtIndex(i)->Discard();
    }
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
    var->SetValue(this->value);
    var->Apply();
}


} // namespace Materials