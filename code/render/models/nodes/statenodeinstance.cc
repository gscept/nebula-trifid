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
    NEBULA3_SEMANTIC_GLOBALLIGHTDIR,
    NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR,
    NEBULA3_SEMANTIC_SHADOW,
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
    this->material = stateNode->GetMaterial();

    // setup the constants in the material which is set by the system (so changing the constants is safe)
    IndexT i;
    for (i = 0; i < sizeof(SharedVariableNames) / sizeof(Util::StringAtom*); i++)
    {
        const Util::StringAtom& name = SharedVariableNames[i];
        if (this->material->HasConstant(name))
        {
            const Ptr<Materials::SurfaceConstant>& var = this->material->GetConstant(name);
            this->sharedConstants.Add(name, var);
        }
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
	this->sharedConstants.Clear();

#ifdef STATE_NODE_USE_PER_OBJECT_BUFFER
	this->perObjectBuffer->Discard();
#endif

    TransformNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
StateNodeInstance::ApplyState(const Ptr<CoreGraphics::ShaderInstance>& shader)
{
	TransformNodeInstance::ApplyState();

    // apply global variables, layer 1 (this should be moved to a per-frame variable buffer and not set per object)
    this->ApplySharedVariables();

    // apply surface, layer 2 (this will probably apply the same variables several unnecessary times, and should be handled by a frame batch!)
    this->material->Apply(shader);

    // apply any instance unique variables, layer 3 (apply per-instance surface properties, unavoidable)
    IndexT i;
    for (i = 0; i < this->surfaceConstantInstanceByName.Size(); i++)
    {
        this->surfaceConstantInstanceByName.ValueAtIndex(i)->Apply(shader);
    }

	// apply any needed model transform state to shader
	const Ptr<TransformDevice>& transformDevice = TransformDevice::Instance();
	const Ptr<ShaderServer>& shaderServer = ShaderServer::Instance();

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
	// apply transform attributes, layer 4 (applies transforms, so basically a piece of layer 3, also unavoidable)
	transformDevice->ApplyModelTransforms(shader);
#endif
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Materials::SurfaceConstantInstance>
StateNodeInstance::CreateSurfaceConstantInstance(const Util::StringAtom& name)
{
    n_assert(!this->surfaceConstantInstanceByName.Contains(name));
    const Ptr<SurfaceConstant>& constant = this->material->GetConstant(name);
    Ptr<SurfaceConstantInstance> instance = constant->CreateInstance();
    this->surfaceConstantInstanceByName.Add(name, instance);
    return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
StateNodeInstance::HasSurfaceConstantInstance(const Util::StringAtom& name)
{
    return this->surfaceConstantInstanceByName.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Materials::SurfaceConstantInstance>&
StateNodeInstance::GetSurfaceConstantInstance(const Util::StringAtom& name)
{
    n_assert(this->surfaceConstantInstanceByName.Contains(name));
    return this->surfaceConstantInstanceByName[name];
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::DiscardSurfaceConstantInstance(const Ptr<Materials::SurfaceConstantInstance>& var)
{
    IndexT index = this->surfaceConstantInstanceByName.ValuesAsArray().FindIndex(var);
    n_assert(index != InvalidIndex)
    var->Discard();
    this->surfaceConstantInstanceByName.EraseAtIndex(index);
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
			var->SetValue(modelInstance->GetPickingId());
		}
		else if (varName == SharedVariableNames[1])
		{
			float4 lightDir = Lighting::LightServer::Instance()->GetGlobalLight()->GetLightDirection();
            var->SetValue(lightDir);
		}
		else if (varName == SharedVariableNames[2])
		{
			float4 lightColor = Lighting::LightServer::Instance()->GetGlobalLight()->GetColor();
            var->SetValue(lightColor);
		}
        else if (varName == SharedVariableNames[3])
        {
            const matrix44& invView = TransformDevice::Instance()->GetInvViewTransform();
            matrix44 shadowView = *Lighting::ShadowServer::Instance()->GetShadowView();
            shadowView = matrix44::multiply(invView, shadowView);
            var->SetValue(shadowView);
        }
        else if (varName == SharedVariableNames[4])
        {
            var->SetTexture(Lighting::ShadowServer::Instance()->GetGlobalLightShadowBufferTexture());
        }
        else if (varName == SharedVariableNames[5])
		{			
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetReflectionMap()->GetTexture());
		}
        else if (varName == SharedVariableNames[6])
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetTexture(probe->GetIrradianceMap()->GetTexture());			
		}
        else if (varName == SharedVariableNames[7])
		{
			const Ptr<Lighting::EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
			var->SetValue(probe->GetReflectionMap()->GetTexture()->GetNumMipLevels());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
StateNodeInstance::SetMaterial(const Ptr<Materials::SurfaceMaterial>& material)
{
    n_assert(material.isvalid());
    this->material = material;

    // surface constant instances will be completely invalid now, so we must clear them
    IndexT i;
    for (i = 0; i < this->surfaceConstantInstanceByName.Size(); i++)
    {
        this->surfaceConstantInstanceByName.ValueAtIndex(i)->Discard();
    }
    this->surfaceConstantInstanceByName.Clear();
}

} // namespace Models
