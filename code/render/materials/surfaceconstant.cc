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
SurfaceConstant::Setup(const StringAtom& name, const Util::Array<Material::MaterialPass>& passes, const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& shaders)
{
    this->name = name;

	IndexT passIndex;
	for (passIndex = 0; passIndex < passes.Size(); passIndex++)
    {
        // get shader and variable (the variable must exist in the shader!)
		const Ptr<CoreGraphics::ShaderInstance>& shader = shaders[passes[passIndex].index];
		bool activeVar = shader->GetOriginalShader()->HasVariableByName(name);

		// setup variable
		Ptr<CoreGraphics::ShaderVariableInstance> var = 0;

		// if the variable is active, create an instance and bind its value, otherwise use a null pointer
		if (activeVar)
		{
			var = shader->CreateVariableInstance(this->name);
			var->SetValue(this->value);
		}

		// setup constant, first we check if the shader even has this variable, it will be inactive otherwise
		this->bindingsByIndex.Append(ConstantBinding{ activeVar, var, shader });
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Discard()
{
	IndexT i;
	for (i = 0; i < this->bindingsByIndex.Size(); i++)
	{
		if (this->bindingsByIndex[i].active)
		{
			this->bindingsByIndex[i].shd->DiscardVariableInstance(this->bindingsByIndex[i].var);
		}
	}
	this->bindingsByIndex.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::Apply(const IndexT passIndex)
{
    // skip applying if this constant has no binding to said variable
	//if (!this->variablesByShader.Contains(group)) return;
	const ConstantBinding& binding = this->bindingsByIndex[passIndex];
	if (binding.active)
	{
		const Ptr<CoreGraphics::ShaderVariableInstance>& var = binding.var;
		var->Apply();
	}    
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceConstant::SetValue(const Util::Variant& value)
{
	this->value = value;
	IndexT i;
	for (i = 0; i < this->bindingsByIndex.Size(); i++)
	{
		const ConstantBinding& binding = this->bindingsByIndex[i];
		if (binding.active) binding.var->SetValue(value);
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
	for (i = 0; i < this->bindingsByIndex.Size(); i++)
	{
		const ConstantBinding& binding = this->bindingsByIndex[i];
		if (binding.active) binding.var->SetTexture(tex);
	}
}

} // namespace Materials