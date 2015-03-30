//------------------------------------------------------------------------------
//  material.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materials/material.h"
#include "materials/materialinstance.h"

namespace Materials
{
__ImplementClass(Materials::Material, 'MATR', Resources::Resource);

//------------------------------------------------------------------------------
/**
*/
Material::Material() :
	isVirtual(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Material::~Material()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Material::Setup()
{
	n_assert(this->materialShaders.IsEmpty());
}

//------------------------------------------------------------------------------
/**
	Load inherited by walking through all shaders, parameters and features, add if non existant, or replace if inherited material defines it again
*/
void 
Material::LoadInherited( const Ptr<Material>& material )
{
	IndexT i;

	// add/replace shaders
	for (i = 0; i < material->materialShaders.Size(); i++)
	{
		const Models::ModelNodeType::Code& key = material->materialShaders.KeyAtIndex(i);
		const Ptr<CoreGraphics::Shader>& value = material->materialShaders.ValueAtIndex(i);
		if (this->materialShaders.Contains(key))
		{
			this->materialShaders[key] = value;
		}
		else
		{
			this->materialShaders.Add(key, value);
		}
	}

	// add/replace parameters
	for (i = 0; i < material->materialParameters.Size(); i++)
	{
		const Util::StringAtom& key = material->materialParameters.KeyAtIndex(i);
		const MaterialParameter& value = material->materialParameters.ValueAtIndex(i);
		if (this->materialParameters.Contains(key))
		{
			this->materialParameters[key] = value;
		}
		else
		{
			this->materialParameters.Add(key, value);
		}
	}

	// add/replace features
	for (i = 0; i < material->materialFeatures.Size(); i++)
	{
		const Models::ModelNodeType::Code& key = material->materialFeatures.KeyAtIndex(i);
		const CoreGraphics::ShaderFeature::Mask& value = material->materialFeatures.ValueAtIndex(i);
		if (this->materialFeatures.Contains(key))
		{
			this->materialFeatures[key] = value;
		}
		else
		{
			this->materialFeatures.Add(key, value);
		}
	}
	this->inheritedMaterials.Append(material);
}

//------------------------------------------------------------------------------
/**
*/
void
Material::Unload()
{
	n_assert(0 == this->materialInstances.Size());
	this->materialShaders.Clear();
	this->materialFeatures.Clear();
	this->materialParameters.Clear();
	this->inheritedMaterials.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::Discard()
{
	this->inheritedMaterials.Clear();
	for (int i = 0; i < this->materialInstances.Size(); i++)
	{
		this->materialInstances[i]->Cleanup();
	}
	this->materialInstances.Clear();
	this->Unload();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<MaterialInstance> 
Material::CreateMaterialInstance()
{
	Ptr<MaterialInstance> newInst = MaterialInstance::Create();
	Ptr<Material> thisPtr(this);
	newInst->Setup(thisPtr);
	this->materialInstances.Append(newInst);
	
	return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::DiscardMaterialInstance( const Ptr<MaterialInstance>& inst )
{
	inst->Cleanup();
	IndexT i = this->materialInstances.FindIndex(inst);
	n_assert(InvalidIndex != i);
	this->materialInstances.EraseIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::AddPass( const Models::ModelNodeType::Code& code, const Ptr<CoreGraphics::Shader>& shader, const CoreGraphics::ShaderFeature::Mask& mask )
{
	n_assert(shader.isvalid());

    // if this pass is already defined and this material inherits another, remove the previous definition and override
	if (this->inheritedMaterials.Size() > 0)
	{
		if (this->materialShaders.Contains(code))
		{
			this->materialShaders.Erase(code);
			this->materialFeatures.Erase(code);
		}
	}

    // add shader and features
    n_assert(!this->materialShaders.Contains(code));
    n_assert(!this->materialFeatures.Contains(code));
    this->materialShaders.Add(code, shader);
    this->materialFeatures.Add(code, mask);
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::AddParam( const Util::String& name, const Material::MaterialParameter& param )
{
    n_assert(!name.IsEmpty());

    // if this parameter is already defined and this material inherits another, remove the previous definition and override
	if (this->inheritedMaterials.Size() > 0)
    {
        if (this->materialParameters.Contains(name))
        {
            this->materialParameters.Erase(name);
        }
    }

    // add parameter
    n_assert(!this->materialParameters.Contains(name));
    this->materialParameters.Add(name, param);
}


} // namespace Materials