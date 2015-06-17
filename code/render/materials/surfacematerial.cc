//------------------------------------------------------------------------------
//  surfacematerial.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfacematerial.h"
#include "coregraphics/shader.h"
#include "resources/resourcemanager.h"

using namespace CoreGraphics;
namespace Materials
{
__ImplementClass(Materials::SurfaceMaterial, 'SRMA', Resources::Resource);

//------------------------------------------------------------------------------
/**
*/
SurfaceMaterial::SurfaceMaterial()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SurfaceMaterial::~SurfaceMaterial()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::Unload()
{
    Resource::Unload();
    IndexT i;
    for (i = 0; i < this->constants.Size(); i++)
    {
        this->constants[i]->Discard();
    }
    this->constants.Clear();
    this->constantsByName.Clear();
    this->staticValues.Clear();
    this->materialTemplate = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::Setup(const Ptr<Material>& material)
{
    n_assert(material.isvalid());
    this->materialTemplate = material;

    // get parameters from material
    const Util::Dictionary<Util::StringAtom, Material::MaterialParameter>& parameters = this->materialTemplate->GetParameters();

    // create temporary dictionary mapping between shaders and their variables
    Util::Dictionary<Util::StringAtom, Util::Array<Ptr<CoreGraphics::ShaderInstance>>> variableToShaderMap;

    SizeT numPasses = this->materialTemplate->GetNumPasses();
    IndexT passIndex;
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        // get indexed data from material
        const Frame::BatchGroup::Code& code = this->materialTemplate->GetBatchGroup(passIndex);
        const Ptr<ShaderInstance>& shdInst = this->materialTemplate->GetShaderInstanceByBatchGroup(code);

        // go through our materials parameter list and set them up
        IndexT paramIndex;
        for (paramIndex = 0; paramIndex < parameters.Size(); paramIndex++)
        {
            // get parameter name
            Util::StringAtom paramName = parameters.KeyAtIndex(paramIndex);
            Material::MaterialParameter param = parameters.ValueAtIndex(paramIndex);

            // get variable with equal name from shader
            if (shdInst->HasVariableByName(paramName))
            {
                // setup variable with value, if it exists
                const Ptr<ShaderVariable>& var = shdInst->GetVariableByName(paramName);
                if (!variableToShaderMap.Contains(paramName)) variableToShaderMap.Add(paramName, Util::Array<Ptr<CoreGraphics::ShaderInstance>>());
                variableToShaderMap[paramName].Append(shdInst);
            }
        }
    }

    // go through mappings
    IndexT mappingIndex;
    for (mappingIndex = 0; mappingIndex < variableToShaderMap.Size(); mappingIndex++)
    {
        const Util::StringAtom& paramName = variableToShaderMap.KeyAtIndex(mappingIndex);
        const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& shaders = variableToShaderMap.ValueAtIndex(mappingIndex);

        // create a new multi-shader variable container (or surface constant)
        Ptr<SurfaceConstant> constant = SurfaceConstant::Create();

        // get parameter by name (which is used for its default value)
        Material::MaterialParameter param = parameters[paramName];

        // value can either by material template default, or the value defined in the surface
        Util::Variant val;
        if (this->staticValues.Contains(paramName)) val = this->staticValues[paramName];
        else                                        val = param.defaultVal;

        // specially handle default values which are strings
        if (val.GetType() == Util::Variant::String)
        {
            Ptr<Resources::ManagedTexture> tex = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, val.GetString(), NULL, true).downcast<Resources::ManagedTexture>();
            val.SetType(Util::Variant::Object);
            val.SetObject(tex->GetTexture());
        }

        // setup constant
        constant->SetValue(val);
        constant->Setup(paramName, shaders);
        constant->system = param.system;

        // add constants to variable lists
        this->constants.Append(constant);
        this->constantsByName.Add(paramName, constant);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::Discard()
{
    // this is a cloneable resource, so just decrease the use count when we discard one
    this->DecrUseCount();
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::SetValue(const Util::StringAtom& param, const Util::Variant& value)
{
    n_assert(param.IsValid());
    n_assert(this->constantsByName.Contains(param));
    this->constantsByName[param]->SetValue(value);
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::SetTexture(const Util::StringAtom& param, const Ptr<CoreGraphics::Texture>& tex)
{
    n_assert(param.IsValid());
    n_assert(this->constantsByName.Contains(param));
    this->constantsByName[param]->SetTexture(tex);
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::SetTexture(const Util::StringAtom& param, const Ptr<Resources::ManagedTexture>& tex)
{
    n_assert(param.IsValid());
    DeferredTextureBinding obj;
    obj.tex = tex;
    obj.var = param;
    this->managedTextures.Append(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceMaterial::Apply(const Ptr<CoreGraphics::ShaderInstance>& shader)
{
    IndexT i;

    // 'touch' textures which may not have been loaded when the material was first set up
    for (i = 0; i < this->managedTextures.Size(); i++)
    {
        const DeferredTextureBinding& bind = this->managedTextures[i];
        if (!bind.tex->IsPlaceholder())
        {
            this->constantsByName[bind.var]->SetTexture(bind.tex->GetTexture());
        }
        this->managedTextures.EraseIndex(i);
        i--;
    }

    // apply values
    for (i = 0; i < this->constants.Size(); i++)
    {
        this->constants[i]->Apply(shader);
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<SurfaceMaterial>
SurfaceMaterial::Clone() const
{
    Ptr<SurfaceMaterial> clone = SurfaceMaterial::Create();
    clone->resourceId = this->resourceId;
    clone->state = this->state;
    clone->staticValues = this->staticValues;
    clone->Setup(this->materialTemplate);    
    clone->IncrUseCount();
    return clone;
}

} // namespace Materials