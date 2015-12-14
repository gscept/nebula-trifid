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
#include "resources/resourcemanager.h"
#include "materials/surfaceconstantinstance.h"

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

static const Util::StringAtom SharedVariableNames[] =
{
    NEBULA3_SEMANTIC_OBJECTID,
    NEBULA3_SEMANTIC_SHADOWPROJMAP,
    NEBULA3_SEMANTIC_ENVIRONMENT,
    NEBULA3_SEMANTIC_IRRADIANCE,
    NEBULA3_SEMANTIC_NUMENVMIPS
};

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
StateNodeInstance::Setup(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	// setup parent class
	TransformNodeInstance::Setup(inst, node, parentNodeInst);

	// setup material
	Ptr<StateNode> stateNode = node.downcast<StateNode>();
    this->surfaceInstance = stateNode->GetMaterial()->CreateInstance();

    // setup the constants in the material which is set by the system (so changing the constants is safe)
    IndexT i;
    for (i = 0; i < sizeof(SharedVariableNames) / sizeof(Util::StringAtom*); i++)
    {
        const Util::StringAtom& name = SharedVariableNames[i];
        if (this->surfaceInstance->HasConstant(name))
        {
            const Ptr<Materials::SurfaceConstant>& var = this->surfaceInstance->GetConstant(name);
            this->sharedConstants.Add(name, var);
        }
    }

#if SHADER_MODEL_5
    ShaderServer* shdServer = ShaderServer::Instance();
    this->shader = shdServer->GetSharedShader();
    this->objectBuffer = ConstantBuffer::Create();
    this->objectBuffer->SetupFromBlockInShader(this->shader, "ObjectBlock");
    this->modelShaderVar = this->objectBuffer->GetVariableByName(NEBULA3_SEMANTIC_MODEL);
    this->invModelShaderVar = this->objectBuffer->GetVariableByName(NEBULA3_SEMANTIC_INVMODEL);
    this->modelViewProjShaderVar = this->objectBuffer->GetVariableByName(NEBULA3_SEMANTIC_MODELVIEWPROJECTION);
    this->modelViewShaderVar = this->objectBuffer->GetVariableByName(NEBULA3_SEMANTIC_MODELVIEW);
    this->objectIdShaderVar = this->objectBuffer->GetVariableByName(NEBULA3_SEMANTIC_OBJECTID);
    this->objectBlockShaderVar = this->shader->GetVariableByName("ObjectBlock");
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::Discard()
{
	this->sharedConstants.Clear();

#if SHADER_MODEL_5
    this->shader = 0;

    this->surfaceInstance->Discard();
    this->surfaceInstance = 0;
    this->objectBuffer->Discard();
    this->objectBuffer = 0;
    this->objectBlockShaderVar = 0;

    this->modelShaderVar = 0;
    this->invModelShaderVar = 0;
    this->modelViewProjShaderVar = 0;
    this->modelViewShaderVar = 0;
    this->objectIdShaderVar = 0;
#endif

    TransformNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::ApplyState(IndexT frameIndex, const Frame::BatchGroup::Code& group, const Ptr<CoreGraphics::Shader>& shader)
{
	TransformNodeInstance::ApplyState(frameIndex, group, shader);

    // apply any instance unique variables, layer 3 (apply per-instance surface properties, unavoidable)
    /*
    IndexT i;
    for (i = 0; i < this->surfaceConstantInstanceByName.Size(); i++)
    {
        this->surfaceConstantInstanceByName.ValueAtIndex(i)->Apply(shader);
    }
    */

	// apply any needed model transform state to shader
	const Ptr<TransformDevice>& transformDevice = TransformDevice::Instance();
	const Ptr<ShaderServer>& shaderServer = ShaderServer::Instance();

#if SHADER_MODEL_5
    // avoid shuffling buffers if we are in the same frame
    if (this->objectBufferUpdateIndex != frameIndex)
    {
        // apply transforms
        this->objectBuffer->CycleBuffers();
        this->modelShaderVar->SetMatrix(transformDevice->GetModelTransform());
        this->invModelShaderVar->SetMatrix(transformDevice->GetInvModelTransform());
        this->modelViewProjShaderVar->SetMatrix(transformDevice->GetModelViewProjTransform());
        this->modelViewShaderVar->SetMatrix(transformDevice->GetModelViewTransform());
        this->objectIdShaderVar->SetInt(this->GetModelInstance()->GetPickingId());
        this->objectBufferUpdateIndex = frameIndex;
    }
    this->objectBlockShaderVar->SetBufferHandle(this->objectBuffer->GetHandle());
#else
	// apply transform attributes, layer 4 (applies transforms, so basically a piece of layer 3, also unavoidable)
	transformDevice->ApplyModelTransforms(shader);
#endif

	// apply global variables, layer 1 (this should be moved to a per-frame variable buffer and not set per object)
	this->ApplySharedVariables();

    // apply this surface material instance if we have a valid batch group
	if (group != Frame::BatchGroup::InvalidBatchGroup) this->surfaceInstance->Apply(group);
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::ApplySharedVariables()
{
	const Ptr<ModelInstance>& modelInstance = this->GetModelInstance();
	const Ptr<ModelEntity>& entity = modelInstance->GetModelEntity();
	IndexT i;

	bool useLocalReflection = false;
	for (i = 0; i < this->sharedConstants.Size(); i++)
	{
		const Ptr<Materials::SurfaceConstant>& var = this->sharedConstants.ValueAtIndex(i);
        const Util::StringAtom& varName = this->sharedConstants.KeyAtIndex(i);
		if (varName == SharedVariableNames[0])
		{
			//var->SetValue(modelInstance->GetPickingId());
		}
        else if (varName == SharedVariableNames[1])
        {
            var->SetTexture(Lighting::ShadowServer::Instance()->GetGlobalLightShadowBufferTexture());
        }
        else if (varName == SharedVariableNames[2])
		{			
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetReflectionMap()->GetTexture());
			//this->surfaceInstance->GetConstant(varName)->SetTexture(probe->GetReflectionMap()->GetTexture());
		}
        else if (varName == SharedVariableNames[3])
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetIrradianceMap()->GetTexture());
			//this->surfaceInstance->GetConstant(varName)->SetTexture(probe->GetIrradianceMap()->GetTexture());
		}
        else if (varName == SharedVariableNames[4])
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetValue(probe->GetReflectionMap()->GetTexture()->GetNumMipLevels());
			//this->surfaceInstance->GetConstant(varName)->SetValue(probe->GetReflectionMap()->GetTexture()->GetNumMipLevels());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::SetSurfaceInstance(const Ptr<Materials::SurfaceInstance>& material)
{
    n_assert(material.isvalid());
	this->sharedConstants.Clear();
	this->surfaceInstance->Discard();
    this->surfaceInstance = material;

	// setup the constants in the material which is set by the system (so changing the constants is safe)
	IndexT i;
	for (i = 0; i < sizeof(SharedVariableNames) / sizeof(Util::StringAtom*); i++)
	{
		const Util::StringAtom& name = SharedVariableNames[i];
		if (this->surfaceInstance->HasConstant(name))
		{
			const Ptr<Materials::SurfaceConstant>& var = this->surfaceInstance->GetConstant(name);
			this->sharedConstants.Add(name, var);
		}
	}
}

} // namespace Models
