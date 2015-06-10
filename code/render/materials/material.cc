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
	n_assert(this->shadersByBatchGroup.IsEmpty());
}

//------------------------------------------------------------------------------
/**
	Load inherited by walking through all shaders, parameters and features, add if non existent, or replace if inherited material defines it again
*/
void
Material::LoadInherited(const Ptr<Material>& material)
{
	IndexT i;

	// add/replace shaders
	for (i = 0; i < material->shadersByBatchGroup.Size(); i++)
	{
        const Frame::BatchGroup::Code& key = material->shadersByBatchGroup.KeyAtIndex(i);
		const Ptr<CoreGraphics::ShaderInstance>& value = material->shadersByBatchGroup.ValueAtIndex(i);
		if (this->shadersByBatchGroup.Contains(key))
		{
            value->Discard();
			this->shadersByBatchGroup[key] = value;
		}
		else
		{
			this->shadersByBatchGroup.Add(key, value);
		}
	}

	// add/replace parameters
	for (i = 0; i < material->parametersByName.Size(); i++)
	{
		const Util::StringAtom& key = material->parametersByName.KeyAtIndex(i);
		const MaterialParameter& value = material->parametersByName.ValueAtIndex(i);
		if (this->parametersByName.Contains(key))
		{
			this->parametersByName[key] = value;
		}
		else
		{
			this->parametersByName.Add(key, value);
		}
	}

	// add/replace features
	for (i = 0; i < material->featuresByBatchGroup.Size(); i++)
	{
        const Frame::BatchGroup::Code& key = material->featuresByBatchGroup.KeyAtIndex(i);
		const CoreGraphics::ShaderFeature::Mask& value = material->featuresByBatchGroup.ValueAtIndex(i);
		if (this->featuresByBatchGroup.Contains(key))
		{
			this->featuresByBatchGroup[key] = value;
		}
		else
		{
			this->featuresByBatchGroup.Add(key, value);
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
	this->shadersByBatchGroup.Clear();
	this->featuresByBatchGroup.Clear();
	this->parametersByName.Clear();
	this->inheritedMaterials.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::Discard()
{
	this->inheritedMaterials.Clear();

    IndexT i;
    for (i = 0; i < this->shaders.Size(); i++)
    {
        this->shaders[i]->Discard();
    }
    this->shaders.Clear();
    this->shadersByBatchGroup.Clear();
	this->Unload();
}

//------------------------------------------------------------------------------
/**
*/
void 
Material::AddPass(const Frame::BatchGroup::Code& code, const Ptr<CoreGraphics::Shader>& shader, const CoreGraphics::ShaderFeature::Mask& mask)
{
	n_assert(shader.isvalid());

    // if this pass is already defined and this material inherits another, remove the previous definition and override
	if (this->inheritedMaterials.Size() > 0)
	{
		if (this->shadersByBatchGroup.Contains(code))
		{
			this->shadersByBatchGroup.Erase(code);
			this->featuresByBatchGroup.Erase(code);
		}
	}

    // add shader and features
    n_assert(!this->shadersByBatchGroup.Contains(code));
    n_assert(!this->featuresByBatchGroup.Contains(code));
    Ptr<CoreGraphics::ShaderInstance> inst = shader->CreateShaderInstance();
    this->shadersByBatchGroup.Add(code, inst);
    this->shaders.Append(inst);
    this->featuresByBatchGroup.Add(code, mask);
}

//------------------------------------------------------------------------------
/**
*/
void
Material::AddParam(const Util::String& name, const Material::MaterialParameter& param)
{
    n_assert(!name.IsEmpty());

    // if this parameter is already defined and this material inherits another, remove the previous definition and override
	if (this->inheritedMaterials.Size() > 0)
    {
        if (this->parametersByName.Contains(name))
        {
            this->parametersByName.Erase(name);
        }
    }

    // add parameter
    n_assert(!this->parametersByName.Contains(name));
    this->parametersByName.Add(name, param);
}

} // namespace Materials