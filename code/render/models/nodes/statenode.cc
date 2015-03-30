//------------------------------------------------------------------------------
//  materialstatenode.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "models/nodes/statenode.h"
#include "models/nodes/statenodeinstance.h"
#include "models/modelnodematerial.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadervariable.h"
#include "materials/material.h"
#include "materials/materialserver.h"
#include "resources/resourcemanager.h"
#include "models/modelinstance.h"


namespace Models
{
__ImplementClass(Models::StateNode, 'STND', Models::TransformNode);


using namespace CoreGraphics;
using namespace Resources;
using namespace Materials;
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
StateNode::StateNode() :
	stateLoaded(Resources::Resource::Initial)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
StateNode::~StateNode()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
StateNode::CreateNodeInstance() const
{
	Ptr<ModelNodeInstance> newInst = (ModelNodeInstance*) StateNodeInstance::Create();
	return newInst;
}

//------------------------------------------------------------------------------
/**
*/
bool StateNode::ParseDataTag( const Util::FourCC& fourCC, const Ptr<IO::BinaryReader>& reader )
{
	bool retval = true;
	if (FourCC('MNMT') == fourCC)
	{
		// read material string, if the material doesn't exist anymore or has a faulty value, revert to the placeholder material
		const Ptr<MaterialServer>& matServer = MaterialServer::Instance();
		Util::String materialName = reader->ReadString();
		if (matServer->HasMaterial(materialName))
		{
			this->SetMaterial(ModelNodeMaterial::FromName(materialName));
		}
		else
		{
			this->SetMaterial(ModelNodeMaterial::FromName("Placeholder"));
		}		
	}
	else if (FourCC('STXT') == fourCC)
	{
		// ShaderTexture
		StringAtom paramName  = reader->ReadString();
		StringAtom paramValue = reader->ReadString();
		String fullTexResId = String(paramValue.AsString() + NEBULA3_TEXTURE_EXTENSION);
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(fullTexResId)));
	}
	else if (FourCC('SINT') == fourCC)
	{
		// ShaderInt
		StringAtom paramName = reader->ReadString();
		int paramValue = reader->ReadInt();
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('SFLT') == fourCC)
	{
		// ShaderFloat
		StringAtom paramName = reader->ReadString();
		float paramValue = reader->ReadFloat();
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('SBOO') == fourCC)
	{
		// ShaderBool
		StringAtom paramName = reader->ReadString();
		bool paramValue = reader->ReadBool();
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('SFV2') == fourCC)
	{
		// ShaderVector
		StringAtom paramName = reader->ReadString();
		float2 paramValue = reader->ReadFloat2();
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('SFV4') == fourCC)
	{
		// ShaderVector
		StringAtom paramName = reader->ReadString();
		float4 paramValue = reader->ReadFloat4();
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('STUS') == fourCC)
	{   
		// @todo: implement universal indexed shader parameters!
		// shaderparameter used by multilayered nodes
		int index = reader->ReadInt();
		float4 paramValue = reader->ReadFloat4();
		String paramName("MLPUVStretch");
		paramName.AppendInt(index);
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else if (FourCC('SSPI') == fourCC)
	{     
		// @todo: implement universal indexed shader parameters!
		// shaderparameter used by multilayered nodes
		int index = reader->ReadInt();
		float4 paramValue = reader->ReadFloat4();
		String paramName("MLPSpecIntensity");
		paramName.AppendInt(index);
		this->shaderParams.Append(KeyValuePair<StringAtom,Variant>(paramName, Variant(paramValue)));
	}
	else
	{
		retval = TransformNode::ParseDataTag(fourCC, reader);
	}
	return retval;
}


//------------------------------------------------------------------------------
/**
*/
void 
StateNode::LoadResources(bool sync)
{
	n_assert(!this->materialInstance.isvalid());
	n_assert(this->materialCode != ModelNodeMaterial::InvalidModelNodeMaterial);
	ShaderServer* shdServer = ShaderServer::Instance();
	MaterialServer* matServer = MaterialServer::Instance();

    // load material instance, this should must always be valid
	Models::ModelNodeMaterial::Name materialName = ModelNodeMaterial::ToName(this->materialCode);
	const Ptr<Material>& material = matServer->GetMaterialByName(materialName);
	n_assert(material.isvalid());
	this->materialInstance = material->CreateMaterialInstance();

	IndexT i;
	for (i = 0; i < this->shaderParams.Size(); i++)
	{
		const StringAtom& paramName = this->shaderParams[i].Key();
		const Variant& paramValue = this->shaderParams[i].Value();  

		// find the proper shader variable
		if (materialInstance->HasVariableByName(paramName))
		{
			const Ptr<MaterialVariable>& var = materialInstance->GetVariableByName(paramName);
			switch (var->GetType())
			{
			case ShaderVariable::IntType:
				var->SetInt(paramValue.GetInt());
				break;

			case ShaderVariable::FloatType:
				var->SetFloat(paramValue.GetFloat());
				break;

			case ShaderVariable::BoolType:
				var->SetBool(paramValue.GetBool());
				break;

			case ShaderVariable::VectorType:
				{
					if (paramValue.GetType() == Variant::Float4)
					{
						var->SetFloat4(paramValue.GetFloat4());
					}
					else if (paramValue.GetType() == Variant::Float2)
					{
						var->SetFloat2(paramValue.GetFloat2());
					}
				}
				break;

			case ShaderVariable::MatrixType:
				var->SetMatrix(paramValue.GetMatrix44());
				break;

			case ShaderVariable::TextureType:
				this->SetupManagedTextureVariable(Resources::ResourceId(paramValue.GetString()), var, sync);
				break;

			default:
				n_error("StateNode::LoadResources(): invalid shader variable type!\n");
				break;
			}
		}
	}

	TransformNode::LoadResources(sync);
}


//------------------------------------------------------------------------------
/**
*/
void 
StateNode::UnloadResources()
{
	n_assert(this->materialInstance->IsValid());

	// discard managed textures
	ResourceManager* resManager = ResourceManager::Instance();
	IndexT i;
	for (i = 0; i < this->managedTextureVariables.Size(); i++)
	{
		resManager->DiscardManagedResource(this->managedTextureVariables[i].managedTexture.upcast<ManagedResource>());
	}
	this->managedTextureVariables.Clear();

	this->materialInstance->Discard();
	this->materialInstance = 0;
	TransformNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Create a new managed texture resource and bind it to the provided
    shader variable.
*/
void
StateNode::SetupManagedTextureVariable(const ResourceId& texResId, const Ptr<MaterialVariable>& var, bool sync)
{
    // append texture extension dependent on platform
    Ptr<ManagedTexture> managedTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, texResId, 0, sync).downcast<ManagedTexture>();
    this->managedTextureVariables.Append(ManagedTextureVariable(managedTexture, var));
}

//------------------------------------------------------------------------------
/**
    This method transfers texture from our managed texture objects into
    their associated shader variable. This is necessary since
    the actual texture of a managed texture may change from frame to frame
    because of resource management.
*/
void
StateNode::UpdateManagedTextureVariables(IndexT frameIndex)
{
    // @todo: don't update if contained texture hasn't changed,
    // should be a method of ManagedResource
    IndexT i;
    for (i = 0; i < this->managedTextureVariables.Size(); i++)
    {
        const Ptr<ManagedTexture>& tex = this->managedTextureVariables[i].managedTexture;
        const Ptr<MaterialVariable>& var = this->managedTextureVariables[i].materialVariable;

		// update loaded state
		if (tex->GetState() > this->stateLoaded) this->stateLoaded = tex->GetState();
        tex->UpdateRenderStats(this->resourceStreamingLevelOfDetail);

        // need to "touch" used resources for the resource management system
        if (tex->IsPlaceholder())
        {
            ResourceManager::Instance()->RequestResourceForLoading(tex.downcast<ManagedResource>());
        }
        else
        {
            tex->SetFrameId(frameIndex);
        }

        // set texture (or placeholder) to shader variable
        var->SetTexture(tex->GetTexture());
    }
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Resources::ManagedTexture> 
StateNode::GetManagedTextureVariable(const Util::StringAtom& varName) const
{
	for (int i = 0; i < this->managedTextureVariables.Size(); i++)
	{
		if (this->managedTextureVariables[i].materialVariable->GetName() == varName)
		{
			return this->managedTextureVariables[i].managedTexture;			
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
Resources::Resource::State
StateNode::GetResourceState() const
{
	Resources::Resource::State loadedState = TransformNode::GetResourceState();
	if (loadedState > this->stateLoaded) loadedState = this->stateLoaded;
	return loadedState;
}

//------------------------------------------------------------------------------
/**
*/
void
StateNode::SetManagedTextureVariable( const Util::StringAtom& varName, const Ptr<Resources::ManagedTexture>& texture )
{
	bool varExists = false;
	for (int i = 0; i < this->managedTextureVariables.Size(); i++)
	{
		if (this->managedTextureVariables[i].materialVariable->GetName() == varName)
		{
			this->managedTextureVariables[i].managedTexture = texture;
			varExists = true;
		}
	}

	// if variable doesn't exist, create it!
	if (!varExists)
	{
		this->SetupManagedTextureVariable(texture->GetResourceId(), this->GetMaterialInstance()->GetVariableByName(varName), false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
StateNode::ApplySharedState(IndexT frameIndex)
{
	// up to parent class
	TransformNode::ApplySharedState(frameIndex);

	// apply managed textures (actually contained texture may have changed)
	this->UpdateManagedTextureVariables(frameIndex);

	// apply material settings
	this->materialInstance->Apply();
}



} // namespace Models
