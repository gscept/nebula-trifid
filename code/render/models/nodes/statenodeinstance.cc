//------------------------------------------------------------------------------
//  materailstatenodeinstance.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/nodes/statenodeinstance.h"
#include "models/nodes/statenode.h"
#include "materials/materialvariable.h"
#include "materials/materialvariableinstance.h"
#include "coregraphics/shader.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariableinstance.h"
#include "coregraphics/transformdevice.h"
#include "models/modelnodeinstance.h"
#include "coregraphics/shadersemantics.h"
#include "models/modelinstance.h"
#include "graphics/modelentity.h"
#include "frame/frameserver.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"

namespace Models
{
__ImplementClass(Models::StateNodeInstance, 'MTNI', Models::TransformNodeInstance);

using namespace Util;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Materials;
using namespace Frame;
using namespace Resources;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
StateNodeInstance::StateNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StateNodeInstance::~StateNodeInstance()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::Setup( const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst )
{
	// setup parent class
	TransformNodeInstance::Setup(inst, node, parentNodeInst);

	// setup material
	Ptr<StateNode> stateNode = node.downcast<StateNode>();
	Ptr<MaterialInstance> material = stateNode->GetMaterialInstance();

	if (material->HasVariableByName(NEBULA3_SEMANTIC_OBJECTID))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_OBJECTID);
		this->globalVariables.Add(NEBULA3_SEMANTIC_OBJECTID, varInst);
	}
	if (material->HasVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTDIR))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTDIR);
		this->globalVariables.Add(NEBULA3_SEMANTIC_GLOBALLIGHTDIR, varInst);
	}
	if (material->HasVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR);
		this->globalVariables.Add(NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR, varInst);
	}
    if (material->HasVariableByName(NEBULA3_SEMANTIC_SHADOW))
    {
        const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_SHADOW);
        this->globalVariables.Add(NEBULA3_SEMANTIC_SHADOW, varInst);
    }
    if (material->HasVariableByName(NEBULA3_SEMANTIC_SHADOWPROJMAP))
    {
        const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_SHADOWPROJMAP);
        this->globalVariables.Add(NEBULA3_SEMANTIC_SHADOWPROJMAP, varInst);
    }
	if (material->HasVariableByName(NEBULA3_SEMANTIC_ENVIRONMENT))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_ENVIRONMENT);
		this->globalVariables.Add(NEBULA3_SEMANTIC_ENVIRONMENT, varInst);
	}
	if (material->HasVariableByName(NEBULA3_SEMANTIC_IRRADIANCE))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_IRRADIANCE);
		this->globalVariables.Add(NEBULA3_SEMANTIC_IRRADIANCE, varInst);
	}
	if (material->HasVariableByName(NEBULA3_SEMANTIC_NUMENVMIPS))
	{
		const Ptr<MaterialVariable>& varInst = material->GetVariableByName(NEBULA3_SEMANTIC_NUMENVMIPS);
		this->globalVariables.Add(NEBULA3_SEMANTIC_NUMENVMIPS, varInst);
	}

#ifdef STATE_NODE_USE_PER_OBJECT_BUFFER
	// setup buffer
	this->perObjectBuffer = ShaderBuffer::Create();
	this->perObjectBuffer->SetSize(sizeof(PerObject));
	this->perObjectBuffer->Setup();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::Discard()
{
	IndexT i;
	for (i = 0; i < this->materialVariableInstances.Size(); i++)
	{
		this->materialVariableInstances.ValueAtIndex(i)->Discard();
	}
    this->materialVariableInstances.Clear();
	this->globalVariables.Clear();

#ifdef STATE_NODE_USE_PER_OBJECT_BUFFER
	this->perObjectBuffer->Discard();
#endif

    TransformNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::ApplyState()
{
	TransformNodeInstance::ApplyState();

	IndexT i;
	for (i = 0; i < this->materialVariableInstances.Size(); i++)
	{
		this->materialVariableInstances.ValueAtIndex(i)->Apply();
	}	

	// apply any needed model transform state to shader
	const Ptr<TransformDevice>& transformDevice = TransformDevice::Instance();
	const Ptr<ShaderServer>& shaderServer = ShaderServer::Instance();

	// get active shader
	const Ptr<ShaderInstance>& shader = shaderServer->GetActiveShaderInstance();

	// apply global variables
	this->ApplyGlobalVariables();

#ifdef STATE_NODE_USE_PER_OBJECT_BUFFER
	// update buffer
	this->perObject.model = transformDevice->GetModelTransform();
	this->perObject.invModel = transformDevice->GetInvModelTransform();
	this->perObject.modelView = transformDevice->GetModelViewTransform();
	this->perObject.mvp = transformDevice->GetModelViewProjTransform();
	this->perObject.objectId = this->GetModelInstance()->GetPickingId();
	this->perObjectBuffer->UpdateBuffer(&this->perObject, 0, sizeof(PerObject));
	if (shader->HasVariableByName(NEBULA3_SEMANTIC_PEROBJECT))
	{
		shader->GetVariableByName(NEBULA3_SEMANTIC_PEROBJECT)->SetBufferHandle(this->perObjectBuffer->GetHandle());
	}
#else
	// apply transform attributes
	transformDevice->ApplyModelTransforms(shader);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::ApplyGlobalVariables()
{
	const Ptr<ModelInstance>& modelInstance = this->GetModelInstance();
	const Ptr<ModelEntity>& entity = modelInstance->GetModelEntity();
	IndexT i;

	bool useLocalReflection = false;
	for (i = 0; i < this->globalVariables.Size(); i++)
	{
		Ptr<MaterialVariable> var = this->globalVariables.ValueAtIndex(i);
		Materials::MaterialVariable::Name varName = this->globalVariables.KeyAtIndex(i);
		if (varName == NEBULA3_SEMANTIC_OBJECTID)
		{
			var->SetInt(modelInstance->GetPickingId());
		}
		else if (varName == NEBULA3_SEMANTIC_GLOBALLIGHTDIR)
		{
			float4 lightDir = Lighting::LightServer::Instance()->GetGlobalLight()->GetLightDirection();
			//matrix44 view = TransformDevice::Instance()->GetViewTransform();
			//lightDir = float4::normalize(matrix44::transform(lightDir, view));
			var->SetFloat4(lightDir);
		}
		else if (varName == NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR)
		{
			float4 lightColor = Lighting::LightServer::Instance()->GetGlobalLight()->GetColor();
			var->SetFloat4(lightColor);
		}
        else if (varName == NEBULA3_SEMANTIC_SHADOW)
        {
            const matrix44& invView = TransformDevice::Instance()->GetInvViewTransform();
            matrix44 shadowView = *Lighting::ShadowServer::Instance()->GetShadowView();
            shadowView = matrix44::multiply(invView, shadowView);
            var->SetMatrix(shadowView);
        }
        else if (varName == NEBULA3_SEMANTIC_SHADOWPROJMAP)
        {
            var->SetTexture(Lighting::ShadowServer::Instance()->GetGlobalLightShadowBufferTexture());
        }
		else if (varName == NEBULA3_SEMANTIC_ENVIRONMENT)
		{			
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetReflectionMap()->GetTexture());
		}
		else if (varName == NEBULA3_SEMANTIC_IRRADIANCE)
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetIrradianceMap()->GetTexture());			
		}
		else if (varName == NEBULA3_SEMANTIC_NUMENVMIPS)
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetInt(probe->GetReflectionMap()->GetTexture()->GetNumMipLevels());
		}
		var->Apply();
	}
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Materials::MaterialVariableInstance> 
StateNodeInstance::CreateMaterialVariableInstance( const Materials::MaterialVariable::Name& name )
{
	n_assert(!this->materialVariableInstances.Contains(name));

	// get the shader from my StateNode
	n_assert(this->modelNode->IsA(StateNode::RTTI));
	const Ptr<MaterialInstance>& materialInstance = this->modelNode.downcast<StateNode>()->GetMaterialInstance();
	n_assert(materialInstance.isvalid());

	// create new shader variable instance
	n_assert(materialInstance->HasVariableByName(name));
	const Ptr<MaterialVariable>& var = materialInstance->GetVariableByName(name);
	Ptr<MaterialVariableInstance> varInst = var->CreateInstance();
	this->materialVariableInstances.Add(name, varInst);
	return varInst;
}

//------------------------------------------------------------------------------
/**
*/
bool 
StateNodeInstance::HasMaterialVariableInstance( const Materials::MaterialVariable::Name& name ) const
{
	return this->materialVariableInstances.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Materials::MaterialVariableInstance>& 
StateNodeInstance::GetMaterialVariableInstance( const Materials::MaterialVariable::Name& name ) const
{
	n_assert(this->materialVariableInstances.Contains(name));
	return this->materialVariableInstances[name];
}

//------------------------------------------------------------------------------
/**
*/
bool 
StateNodeInstance::HasMaterialVariable( const Materials::MaterialVariable::Name& name ) const
{
	const Ptr<MaterialInstance>& materialInstance = this->modelNode.downcast<StateNode>()->GetMaterialInstance();
	return materialInstance->HasVariableByName(name);
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::DiscardMaterialVariableInstance(Ptr<Materials::MaterialVariableInstance>& var)
{
	StringAtom name = var->GetMaterialVariable()->GetName();
	n_assert(this->materialVariableInstances.Contains(name));
	var->Discard();
	this->materialVariableInstances.Erase(name);
}

} // namespace Models
