//------------------------------------------------------------------------------
//  staticgraphicshandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "frame/frameserver.h"
#include "animation/animeventserver.h"
#include "coregraphics/mouserenderdevice.h"
#include "framesync/framesynctimer.h"
#include "lighting/shadowserver.h"
#include "resources/resourcemanager.h"
#include "particles/particleserver.h"
#include "models/nodes/statenode.h"
#include "models/modelserver.h"
#include "materials/materialserver.h"
#include "materials/materialvariableinstance.h"
#include "renderutil/nodelookuputil.h"
#include "resources/managedmesh.h"
#include "particles/particlesystemnode.h"
#include "resources/resourcemanager.h"
#include "picking/pickingserver.h"
#include "lighting/lightserver.h"
#include "environment/environmentserver.h"
#include "input/inputserver.h"

using namespace Util;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Graphics;
using namespace Resources;
using namespace Frame;
using namespace Lighting;
using namespace Models;
using namespace Materials;
using namespace Picking;
using namespace Environment;

namespace Messaging
{

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(AdapterExists)
{
    msg->SetResult(DisplayDevice::Instance()->AdapterExists(msg->GetAdapter()));
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(GetAvailableDisplayModes)
{
    Adapter::Code adapter = msg->GetAdapter();
    PixelFormat::Code pixelFormat = msg->GetPixelFormat();
    msg->SetResult(DisplayDevice::Instance()->GetAvailableDisplayModes(adapter, pixelFormat));
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(GetCurrentAdapterDisplayMode)
{
    Adapter::Code adapter = msg->GetAdapter();
    msg->SetResult(DisplayDevice::Instance()->GetCurrentAdapterDisplayMode(adapter));
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(GetAdapterInfo)
{
    Adapter::Code adapter = msg->GetAdapter();
    msg->SetResult(DisplayDevice::Instance()->GetAdapterInfo(adapter));
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(AttachDisplayEventHandler)
{
    DisplayDevice::Instance()->AttachEventHandler(msg->GetHandler().upcast<DisplayEventHandler>());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(RemoveDisplayEventHandler)
{
    DisplayDevice::Instance()->RemoveEventHandler(msg->GetHandler().upcast<DisplayEventHandler>());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(AttachRenderEventHandler)
{
    RenderDevice::Instance()->AttachEventHandler(msg->GetHandler().upcast<RenderEventHandler>());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(RemoveRenderEventHandler)
{
    RenderDevice::Instance()->RemoveEventHandler(msg->GetHandler().upcast<RenderEventHandler>());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(CreateGraphicsStage)
{
    const StringAtom& name = msg->GetName();
    Ptr<Stage> stage = GraphicsServer::Instance()->CreateStage(name, msg->GetVisibleSystems());
    msg->GetObjectRef()->Validate<Stage>(stage.get());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(DiscardGraphicsStage)
{
    Ptr<Stage> stage = msg->GetObjectRef()->Ref<Stage>();
    GraphicsServer::Instance()->DiscardStage(stage);

    // important to invalidate object ref, so that object not released in the main thread!
    msg->GetObjectRef()->Invalidate();
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(EnableResourcesAsync)
{
	Ptr<ResourceManager> resManager = ResourceManager::Instance();
	resManager->SetMappersAsync(msg->GetEnabled());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(ReloadResource)
{
	Ptr<ResourceManager> resManager = ResourceManager::Instance();
	Ptr<Resources::Resource> res = resManager->LookupResource(msg->GetResourceName());
	if (res.isvalid())
	{
		res->Reload();
	}
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(ReloadResourceIfExists)
{
	Ptr<ResourceManager> resManager = ResourceManager::Instance();
	if (resManager->HasResource(msg->GetResourceName()))
	{
		Ptr<Resources::Resource> res = resManager->LookupResource(msg->GetResourceName());
		if (res.isvalid())
		{
			res->Reload();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(EnablePicking)
{
	n_assert(PickingServer::HasInstance());
	PickingServer::Instance()->SetEnabled(msg->GetEnabled());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(ItemAtPosition)
{
	n_assert(PickingServer::HasInstance());
	IndexT item = PickingServer::Instance()->FetchIndex(msg->GetPosition());
	msg->SetItem(item);
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(ItemsAtPosition)
{
	n_assert(PickingServer::HasInstance());
	Util::Array<IndexT> items;
	Util::Array<IndexT> edgeItems;
	PickingServer::Instance()->FetchSquare(msg->GetRectangle(), items, edgeItems);
	msg->SetItems(items);
	msg->SetEdgeItems(edgeItems);
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(DepthAtPosition)
{
	n_assert(PickingServer::HasInstance());
	float depth = PickingServer::Instance()->FetchDepth(msg->GetPosition());
	msg->SetDepth(depth);
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(NormalAtPosition)
{
	n_assert(PickingServer::HasInstance());
	Math::float4 normal = PickingServer::Instance()->FetchNormal(msg->GetPosition());
	msg->SetNormal(normal);
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(EnableWireframe)
{
	n_assert(RenderDevice::HasInstance());
	RenderDevice::Instance()->SetRenderWireframe(msg->GetEnabled());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(GetRenderMaterials)
{
	MaterialServer* materialServer = Materials::MaterialServer::Instance();
	msg->SetMaterials(materialServer->GetMaterials());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(SetModelNodeTexture)
{
    /*
	ModelServer* modelServer = ModelServer::Instance();
	ResourceManager* resManager = ResourceManager::Instance();
	Ptr<ManagedModel> model = modelServer->LookupManagedModel(msg->GetModelName());
	Ptr<ModelNode> modelNode = model->GetModel()->LookupNode(msg->GetModelNodeName());

	Ptr<StateNode> matNode = modelNode.cast<StateNode>();
	if (matNode && matNode->GetMaterialInstance()->HasVariableByName(msg->GetTextureName()))
	{
		Ptr<MaterialVariable> var = matNode->GetMaterialInstance()->GetVariableByName(msg->GetTextureName());

		Ptr<ManagedTexture> texture;
		if (resManager->IsResourceUnmanaged(msg->GetTextureResource()))
		{
			var->SetTexture(resManager->LookupResource(msg->GetTextureResource()).downcast<Texture>());
		}
		else
		{
			texture = resManager->CreateManagedResource(Texture::RTTI, msg->GetTextureResource()).downcast<ManagedTexture>();
		}

		if (texture.isvalid())
		{
			matNode->SetManagedTextureVariable(var->GetName(), texture);
		}

	}
    */
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(SetModelNodeVariable)
{
    /*
	ModelServer* modelServer = ModelServer::Instance();
	Ptr<ManagedModel> model = modelServer->LookupManagedModel(msg->GetModelName());
	Ptr<ModelNode> modelNode = model->GetModel()->LookupNode(msg->GetModelNodeName());

	Ptr<StateNode> matNode = modelNode.cast<StateNode>();
	if (matNode->GetMaterialInstance()->HasVariableByName(msg->GetVariableName()))
	{
		Ptr<MaterialVariable> var = matNode->GetMaterialInstance()->GetVariableByName(msg->GetVariableName());
		Util::Array<Ptr<MaterialVariableInstance> > varInstances = var->GetInstances();

		Util::Variant value = msg->GetVariableValue();
		int Index;
		switch (value.GetType())
		{
		case Variant::Int:
			var->SetInt(value.GetInt());
			for (Index = 0; Index < varInstances.Size(); Index++)
			{
				varInstances[Index]->SetInt(value.GetInt());
			}
			break;
		case Variant::Float:
			var->SetFloat(value.GetFloat());
			for (Index = 0; Index < varInstances.Size(); Index++)
			{
				varInstances[Index]->SetFloat(value.GetFloat());
			}
			break;
		case Variant::Float4:
			var->SetFloat4(value.GetFloat4());
			for (Index = 0; Index < varInstances.Size(); Index++)
			{
				varInstances[Index]->SetFloat4(value.GetFloat4());
			}
			break;
		case Variant::Bool:
			var->SetBool(value.GetBool());
			for (Index = 0; Index < varInstances.Size(); Index++)
			{
				varInstances[Index]->SetBool(value.GetBool());
			}
			break;
		}
	}
    */
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(CreateGraphicsView)
{
    const Core::Rtti* viewClass = msg->GetViewClass();
    n_assert(viewClass && viewClass->IsDerivedFrom(View::RTTI));
    const StringAtom& viewName = msg->GetName();
    const StringAtom& stageName = msg->GetStageName();
    const ResourceId& frameShaderName = msg->GetFrameShaderName();
    bool isDefaultView = msg->GetDefaultView();

    Ptr<FrameShader> frameShader = FrameServer::Instance()->LookupFrameShader(frameShaderName);
    const Ptr<Stage>& stage = GraphicsServer::Instance()->GetStageByName(stageName);
    Ptr<View> view = GraphicsServer::Instance()->CreateView(*viewClass, viewName, isDefaultView);
	if (msg->GetUseResolveRect())
	{
		view->SetResolveRect(msg->GetResolveRect());
	}
    view->SetStage(stage);
    view->SetFrameShader(frameShader);

    msg->GetObjectRef()->Validate<View>(view.get());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(DiscardGraphicsView)
{
    Ptr<View> view = msg->GetObjectRef()->Ref<View>();
    GraphicsServer::Instance()->DiscardView(view);

    // important to invalidate object ref, so that object not released in the main thread!
    msg->GetObjectRef()->Invalidate();
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(SetDefaultGraphicsView)
{
    Ptr<View> view = msg->GetObjectRef()->Ref<View>();
    GraphicsServer::Instance()->SetDefaultView(view);
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(RegisterAnimEventHandler)
{
    Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(msg->GetAnimEventHandler());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(UnregisterAnimEventHandler)
{
    Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(msg->GetCategoryName());  
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(PreloadMousePointerTextures)
{
    MouseRenderDevice::Instance()->PreloadTextures(msg->GetResourceIds());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(UpdateMousePointers)
{
    MouseRenderDevice::Instance()->UpdatePointers(msg->GetPointers());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(RegisterRTPlugin)
{
    RenderModules::RTPluginRegistry::Instance()->RegisterRTPlugin(msg->GetType());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(UnregisterRTPlugin)
{
    RenderModules::RTPluginRegistry::Instance()->UnregisterRTPlugin(msg->GetType());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(SetTimeFactor)
{
    FrameSync::FrameSyncTimer::Instance()->SetTimeFactor(msg->GetFactor());

    // @todo: what's this??
#if !(__PS3__)
    Particles::ParticleSystem::ModulateStepTime(msg->GetFactor());
#endif
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(UpdateSharedShaderVariable)
{
    const Ptr<Shader>& sharedShader = ShaderServer::Instance()->GetSharedShader();
    const Ptr<ShaderVariable>& shaderVar = sharedShader->GetVariableByName(msg->GetName());
    switch (msg->GetValue().GetType())
    {
        case Variant::Float:
            shaderVar->SetFloat(msg->GetValue().GetFloat());
            break;
        case Variant::Float4:
            shaderVar->SetFloat4(msg->GetValue().GetFloat4());
            break;
        case Variant::Matrix44:
            shaderVar->SetMatrix(msg->GetValue().GetMatrix44());
            break;   
        case Variant::FloatArray:
            shaderVar->SetFloatArray(msg->GetValue().GetFloatArray().Begin(), msg->GetValue().GetFloatArray().Size());
            break;
        case Variant::Float4Array: 
            shaderVar->SetFloat4Array(msg->GetValue().GetFloat4Array().Begin(), msg->GetValue().GetFloat4Array().Size());
            break;
        case Variant::Matrix44Array:
            shaderVar->SetMatrixArray(msg->GetValue().GetMatrix44Array().Begin(), msg->GetValue().GetMatrix44Array().Size());
            break;

        case Variant::Void:
        case Variant::Int:
        case Variant::Bool:        
        case Variant::String:        
        case Variant::Blob:
        case Variant::Guid:
        case Variant::Object:
        case Variant::IntArray:
        case Variant::BoolArray:
        case Variant::BlobArray:
        case Variant::StringArray:
        case Variant::GuidArray:
            n_error("OnUpdateSharedShaderVariable: type not supported!");
            break;                                                    
    }
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(SetShadowPointOfInterest)
{
    ShadowServer::Instance()->SetPointOfInterest(msg->GetPoi());
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(HoldSharedResources)
{
    // @todo: remove this message and related stuff as new resource management won't need this (at least it shouldn't ...)
    ResourceManager::Instance()->HoldResources();
}

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(ReleaseSharedResources)
{
    // @todo: remove this message and related stuff as new resource management won't need this (at least it shouldn't ...)
    ResourceManager::Instance()->ReleaseResources();
}

__StaticHandler(ShowSystemCursor)
{
    Input::InputServer::Instance()->SetCursorVisible(msg->GetVisible());
}


} // namespace Messaging

