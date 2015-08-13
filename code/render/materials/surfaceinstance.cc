//------------------------------------------------------------------------------
//  surfaceinstance.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "surfaceinstance.h"
#include "material.h"
#include "surface.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shaderinstance.h"
#include "surfaceconstant.h"

using namespace CoreGraphics;
namespace Materials
{
__ImplementClass(Materials::SurfaceInstance, 'SUIN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SurfaceInstance::SurfaceInstance() :
    originalSurface(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SurfaceInstance::~SurfaceInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceInstance::Setup(const Ptr<Surface>& surface)
{
    // set original surface
    this->originalSurface = surface;

    // create temporary dictionary mapping between shaders and their variables
    Util::Dictionary<Util::StringAtom, Util::Array<Ptr<CoreGraphics::ShaderInstance>>> variableToShaderMap;
    Util::Dictionary<Util::StringAtom, Util::Array<Frame::BatchGroup::Code>> variableToCodeMap;
    const Ptr<Material>& materialTemplate = this->originalSurface->materialTemplate;

    // get parameters from material
    const Util::Dictionary<Util::StringAtom, Material::MaterialParameter>& parameters = materialTemplate->GetParameters();
    this->code = this->originalSurface->code;

    SizeT numPasses = materialTemplate->GetNumPasses();
    IndexT passIndex;
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        // get indexed data from material
        const Frame::BatchGroup::Code& code = materialTemplate->GetBatchGroup(passIndex);
        const Ptr<Shader>& shader = materialTemplate->GetShaderByBatchGroup(code);
        const Ptr<ShaderInstance>& shdInst = shader->CreateShaderInstance();

        // go through our materials parameter list and set them up
        IndexT paramIndex;
        for (paramIndex = 0; paramIndex < parameters.Size(); paramIndex++)
        {
            // get parameter name
            const Util::StringAtom& paramName = parameters.KeyAtIndex(paramIndex);
            const Material::MaterialParameter& param = parameters.ValueAtIndex(paramIndex);

            // get variable with equal name from shader
            if (shader->HasVariableByName(paramName))
            {
                // setup variable with value, if it exists
                if (!variableToShaderMap.Contains(paramName)) variableToShaderMap.Add(paramName, Util::Array<Ptr<CoreGraphics::ShaderInstance>>());
                variableToShaderMap[paramName].Append(shdInst);

                if (!variableToCodeMap.Contains(paramName)) variableToCodeMap.Add(paramName, Util::Array<Frame::BatchGroup::Code>());
                variableToCodeMap[paramName].Append(code);
            }
        }

        // add to dictionary
        this->shaderInstances.Append(shdInst);
        this->shaderInstancesByCode.Add(code, shdInst);
    }

    // go through mappings
    IndexT mappingIndex;
    for (mappingIndex = 0; mappingIndex < variableToShaderMap.Size(); mappingIndex++)
    {
        const Util::StringAtom& paramName = variableToShaderMap.KeyAtIndex(mappingIndex);
        const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& shaders = variableToShaderMap.ValueAtIndex(mappingIndex);
        const Util::Array<Frame::BatchGroup::Code>& codes = variableToCodeMap.ValueAtIndex(mappingIndex);

        // create a new multi-shader variable container (or surface constant)
        Ptr<SurfaceConstant> constant = SurfaceConstant::Create();

        // get parameter by name (which is used for its default value)
        const Material::MaterialParameter& param = parameters[paramName];

        // get the value defined in the surface resource
        Surface::SurfaceValueBinding& val = this->originalSurface->staticValues[paramName];
        
        // specially handle default values which are strings
        if (val.value.GetType() == Util::Variant::String)
        {
            Ptr<Resources::ManagedTexture> tex = Resources::ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, val.value.GetString() + NEBULA3_TEXTURE_EXTENSION, NULL, true).downcast<Resources::ManagedTexture>();
            //this->SetTexture(paramName, tex);
            val.value.SetType(Util::Variant::Object);
            val.value.SetObject(tex->GetTexture());
        }

        // setup constant
        constant->SetValue(val.value);
        constant->Setup(paramName, shaders, codes);
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
SurfaceInstance::Discard()
{
    this->originalSurface->DiscardInstance(this);
    this->originalSurface = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceInstance::Cleanup()
{
    IndexT i;
    for (i = 0; i < this->shaderInstances.Size(); i++)
    {
        this->shaderInstances[i]->Discard();
    }
    this->shaderInstances.Clear();
    this->shaderInstancesByCode.Clear();

    for (i = 0; i < this->constants.Size(); i++)
    {
        this->constants[i]->Discard();
    }
    this->constants.Clear();
    this->constantsByName.Clear();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<CoreGraphics::ShaderInstance>&
SurfaceInstance::GetShaderInstance(const Frame::BatchGroup::Code& pass)
{
    return this->shaderInstancesByCode[pass];
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceInstance::SetValue(const Util::StringAtom& param, const Util::Variant& value)
{
    n_assert(param.IsValid());
    n_assert(this->constantsByName.Contains(param));
    this->constantsByName[param]->SetValue(value);
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceInstance::SetTexture(const Util::StringAtom& param, const Ptr<CoreGraphics::Texture>& tex)
{
    n_assert(param.IsValid());
    n_assert(this->constantsByName.Contains(param));
    this->constantsByName[param]->SetTexture(tex);
}

//------------------------------------------------------------------------------
/**
*/
void
SurfaceInstance::SetTexture(const Util::StringAtom& param, const Ptr<Resources::ManagedTexture>& tex)
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
SurfaceInstance::Apply(const Frame::BatchGroup::Code& group)
{
    IndexT i;

    // 'touch' textures which may not have been loaded when the material was first set up
    for (i = 0; i < this->managedTextures.Size(); i++)
    {
        const DeferredTextureBinding& bind = this->managedTextures[i];
        if (!bind.tex->IsPlaceholder())
        {
            this->constantsByName[bind.var]->SetTexture(bind.tex->GetTexture());
            this->managedTextures.EraseIndex(i);
            i--;
        }
    }

    for (i = 0; i < this->constants.Size(); i++)
    {
        this->constants[i]->Apply(group);
    }

    // get shader instance by code
    //this->shaderInstancesByCode[group]->Apply();
    this->shaderInstancesByCode[group]->Commit();

    /*
    // apply values
    for (i = 0; i < this->constants.Size(); i++)
    {
        this->constants[i]->Apply(group);
    }
    */
}

} // namespace Materials