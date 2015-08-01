//------------------------------------------------------------------------------
//  properties/graphicsproperty.h
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/graphicsproperty.h"
#include "appgame/appconfig.h"
#include "graphicsfeatureunit.h"
#include "graphicsutil/segmentedgfxutil.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegameprotocol.h"
#include "debugrender/debugrender.h"
#include "effects/effectsfeatureunit.h"
#include "multiplayer/networkentity.h"
#include "graphicsutil/attachmentutil.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::GraphicsProperty, 'GFXP', Game::Property);

using namespace Graphics;
using namespace Game;
using namespace Math;
using namespace Util;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::GraphicsProperty() :
	nodesDirty(true),
    loadSync(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::~GraphicsProperty()
{
    n_assert(!this->modelEntity.isvalid());
}

//------------------------------------------------------------------------------
/** 
    Attach the property to a game entity. This will create and setup
    the required graphics entities.
*/
void
GraphicsProperty::OnActivate()
{
    Game::Property::OnActivate();
    this->SetupGraphics();
}

//------------------------------------------------------------------------------
/**    
    Remove the property from its game entity. This will release the
    graphics entities owned by the property.
*/
void
GraphicsProperty::OnDeactivate()
{
    this->DiscardGraphics();
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::SetupCallbacks()
{    
    this->entity->RegisterPropertyCallback(this, RenderDebug);
	this->entity->RegisterPropertyCallback(this, HandleDeferred);
	this->entity->RegisterPropertyCallback(this, MoveAfter);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::SetupGraphics()
{
    const Ptr<Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    this->modelEntity = ModelEntity::Create();
    Util::String res = this->entity->GetString(Attr::Graphics);
    res.Format("mdl:%s.n3", res.AsCharPtr());
    this->modelEntity->SetResourceId(res);
    this->modelEntity->SetTransform(this->entity->GetMatrix44(Attr::Transform));
    this->modelEntity->SetLoadSynced(this->entity->GetBool(Attr::LoadSynced));
    this->modelEntity->SetPickingId(this->entity->GetUniqueId());
	this->modelEntity->SetStatic(this->entity->GetBool(Attr::Static));
	this->modelEntity->SetCastsShadows(this->entity->GetBool(Attr::CastShadows));
    stage->AttachEntity(this->modelEntity.upcast<Graphics::GraphicsEntity>());

    // setup anim events
    this->SetupAnimEvents(this->modelEntity);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::DiscardGraphics()
{
    this->RemoveAnimEvents(this->modelEntity);
    const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    stage->RemoveEntity(this->modelEntity.upcast<GraphicsEntity>());
    this->modelEntity = 0;
}

//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::OnRenderDebug()
{    
    String category(this->entity->GetCategory());
    String guidTxt(this->entity->GetGuid(Attr::Guid).AsString());
    category.Append(": ");
    category.Append(guidTxt);
    Math::point pos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
	//_debug_text3D(category, pos, float4(0.5,1.0,0.5,1));
}

//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(BaseGameFeature::UpdateTransform::Id);
    this->RegisterMessage(GraphicsFeature::SetGraphicsVisible::Id);
    this->RegisterMessage(GraphicsFeature::GetModelEntity::Id);
    this->RegisterMessage(GraphicsFeature::SetOverwriteColor::Id);
    this->RegisterMessage(GraphicsFeature::SetShaderVariable::Id);
	this->RegisterMessage(GraphicsFeature::SetMaterialVariable::Id);
	this->RegisterMessage(GraphicsFeature::PlayAnimClip::Id);
	this->RegisterMessage(GraphicsFeature::PauseAllAnims::Id);
    this->RegisterMessage(GraphicsFeature::CreateGraphicsEffectUpVec::Id);
	this->RegisterMessage(GraphicsFeature::UpdateProbeInfluence::Id);
	this->RegisterMessage(GraphicsFeature::SetAttribute::Id);
	this->RegisterMessage(GraphicsFeature::AddGraphicsAttachment::Id);
	this->RegisterMessage(GraphicsFeature::AddGraphicsAttachmentOnJoint::Id);	
	this->RegisterMessage(GraphicsFeature::ClearAttachmentsOnEntity::Id);
	this->RegisterMessage(GraphicsFeature::ClearAttachmentsOnJoint::Id);
    Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg);
    if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
    {
		this->UpdateTransform((msg.cast<BaseGameFeature::UpdateTransform>())->GetMatrix(), false);
    }
    else if (msg->CheckId(GraphicsFeature::SetGraphicsVisible::Id))
    {
        this->SetVisible((msg.cast<GraphicsFeature::SetGraphicsVisible>())->GetVisible());
		__DistributeNetworkMessage(this->entity, msg);
    }
    else if (msg->CheckId(GraphicsFeature::GetModelEntity::Id))
    {
        (msg.cast<GetModelEntity>())->SetEntity(this->modelEntity);
    }
    else if (msg->CheckId(GraphicsFeature::SetOverwriteColor::Id))
    {
        this->OnSetOverwriteColor(msg.cast<SetOverwriteColor>());
		__DistributeNetworkMessage(this->entity, msg);
    }
    else if (msg->CheckId(GraphicsFeature::SetShaderVariable::Id))
    {
        this->OnSetShaderVariable(msg.cast<SetShaderVariable>());
    }
	else if (msg->CheckId(GraphicsFeature::SetMaterialVariable::Id))
	{
		this->OnSetMaterialVariable(msg.cast<SetMaterialVariable>());
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(GetModelNodes::Id))
	{
		this->OnReceiveModelNodes(msg.cast<GetModelNodes>());
	}
    else if (msg->CheckId(CreateGraphicsEffectUpVec::Id))
    {
        Ptr<CreateGraphicsEffectUpVec> pmsg = msg.cast<CreateGraphicsEffectUpVec>();
        matrix44 transform;
        transform = matrix44::lookatlh(pmsg->GetPoint(), pmsg->GetPoint() + this->entity->GetMatrix44(Attr::Transform).get_zaxis(), pmsg->GetUpVec());
        EffectsFeature::EffectsFeatureUnit::Instance()->EmitGraphicsEffect(transform, pmsg->GetResource(), pmsg->GetDuration());
		__DistributeNetworkMessage(this->entity, msg);
    }
	else if (msg->CheckId(SetSkinVisible::Id))
	{
		Ptr<SetSkinVisible> pmsg = msg.cast<SetSkinVisible>();
		if (pmsg->GetVisible())
		{
			Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
			showSkin->SetSkin(pmsg->GetSkin());
			this->modelEntity->HandleMessage(showSkin.upcast<Messaging::Message>());
		}
		else
		{
			Ptr<Graphics::HideSkin> showSkin = Graphics::HideSkin::Create();
			showSkin->SetSkin(pmsg->GetSkin());
			this->modelEntity->HandleMessage(showSkin.upcast<Messaging::Message>());
		}
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(PlayAnimClip::Id))
	{
		Ptr<PlayAnimClip> pmsg = msg.cast<PlayAnimClip>();
		Ptr<Graphics::AnimPlayClip> amsg = Graphics::AnimPlayClip::Create();
		
		amsg->SetClipName(pmsg->GetClip());
		amsg->SetLoopCount(pmsg->GetLoopCount());
		if(pmsg->GetQueue())
		{
			amsg->SetEnqueueMode(Animation::AnimJobEnqueueMode::Append);
		}
		else
		{
			amsg->SetEnqueueMode(Animation::AnimJobEnqueueMode::Intercept);
		}		
		this->modelEntity->HandleMessage(amsg.cast<Messaging::Message>());
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(PlayAnimClipFull::Id))
	{
		Ptr<PlayAnimClipFull> pmsg = msg.cast<PlayAnimClipFull>();
		Ptr<Graphics::AnimPlayClip> amsg = Graphics::AnimPlayClip::Create();

		amsg->SetClipName(pmsg->GetClipName());
		amsg->SetLoopCount(pmsg->GetLoopCount());
		amsg->SetStartTime(pmsg->GetStartTime());
		amsg->SetFadeInTime(pmsg->GetFadeInTime());
		amsg->SetFadeOutTime(pmsg->GetFadeOutTime());
		amsg->SetTimeOffset(pmsg->GetTimeOffset());
		amsg->SetTimeFactor(pmsg->GetTimeFactor());
		amsg->SetBlendWeight(pmsg->GetBlendWeight());
		const Util::String & queue = pmsg->GetEnqueueMode();
		if (queue == "Intercept")
		{
			amsg->SetEnqueueMode(Animation::AnimJobEnqueueMode::Intercept);
		}
		else if (queue == "Append")
		{
			amsg->SetEnqueueMode(Animation::AnimJobEnqueueMode::Append);
		}
		else if (queue == "IgnoreIfSame")
		{
			amsg->SetEnqueueMode(Animation::AnimJobEnqueueMode::IgnoreIfSameClipActive);
		}
		this->modelEntity->HandleMessage(amsg.cast<Messaging::Message>());
	}
	else if (msg->CheckId(PauseAllAnims::Id))
	{	
		Ptr<Graphics::AnimPauseAllTracks> amsg = Graphics::AnimPauseAllTracks::Create();		
		this->modelEntity->HandleMessage(amsg.cast<Messaging::Message>());
	}
	else if (msg->CheckId(UpdateProbeInfluence::Id))
	{
		Ptr<UpdateProbeInfluence> rmsg = msg.downcast<UpdateProbeInfluence>();
		this->modelEntity->SetEnvironmentProbe(rmsg->GetEnvironmentProbe());
	}
	else if (msg->CheckId(BaseGameFeature::SetAttribute::Id))
	{
		Ptr<BaseGameFeature::SetAttribute> gmsg = msg.cast<BaseGameFeature::SetAttribute>();
		const Attr::Attribute& attr = gmsg->GetAttr();
		if (this->entity->HasAttr(attr.GetAttrId())) this->entity->SetAttr(attr);

		this->modelEntity->SetStatic(this->entity->GetBool(Attr::Static));
		this->modelEntity->SetCastsShadows(this->entity->GetBool(Attr::CastShadows));
		this->modelEntity->SetInstanced(this->entity->GetBool(Attr::Instanced));
	}
	else if (msg->CheckId(AddGraphicsAttachment::Id))
	{
		Ptr<AddGraphicsAttachment> amsg = msg.cast<AddGraphicsAttachment>();
		GraphicsFeature::AttachmentUtil::AddAttachment(this->entity, amsg->GetResource(), amsg->GetOffset());
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(AddGraphicsAttachmentOnJoint::Id))
	{
		Ptr<AddGraphicsAttachmentOnJoint> amsg = msg.cast<AddGraphicsAttachmentOnJoint>();
		GraphicsFeature::AttachmentUtil::AddAttachment(this->entity, amsg->GetJoint(), amsg->GetResource(), amsg->GetOffset(), (GraphicsFeature::AttachmentManager::AttachmentRotation)amsg->GetRotation());
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(ClearAttachmentsOnEntity::Id))
	{
		Ptr<ClearAttachmentsOnEntity> amsg = msg.cast<ClearAttachmentsOnEntity>();
		GraphicsFeature::AttachmentManager::Instance()->ClearAttachmentsOnEntity(this->modelEntity.cast<Graphics::GraphicsEntity>());
		__DistributeNetworkMessage(this->entity, msg);
	}
	else if (msg->CheckId(ClearAttachmentsOnJoint::Id))
	{
		Ptr<ClearAttachmentsOnJoint> amsg = msg.cast<ClearAttachmentsOnJoint>();
		GraphicsFeature::AttachmentManager::Instance()->ClearAttachmentsOnJoint(amsg->GetJoint(), this->modelEntity.cast<Graphics::GraphicsEntity>());
		__DistributeNetworkMessage(this->entity, msg);
	}	
    else
    {
        Property::HandleMessage(msg);
    }
}
    
//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::OnSetOverwriteColor(const Ptr<SetOverwriteColor>& msg)
{
    n_assert(this->modelEntity.isvalid());    
    this->modelEntity->SetSurfaceConstant(msg->GetNodeName(), "MaterialColor", Util::Variant(msg->GetColor()));
}

//------------------------------------------------------------------------------
/**    
*/
void
GraphicsProperty::OnSetShaderVariable(const Ptr<SetShaderVariable>& msg)
{
    n_assert(this->modelEntity.isvalid());    
    this->modelEntity->SetSurfaceConstant(msg->GetNodeName(), msg->GetShaderVarName(), msg->GetValue());
}

//------------------------------------------------------------------------------
/**    
    Called to update the graphics entity's transform.
*/
void
GraphicsProperty::UpdateTransform(const matrix44& m, bool setDirectly)
{
    n_assert(this->modelEntity.isvalid());    
    this->modelEntity->SetTransform(m);
}

//------------------------------------------------------------------------------
/**    
    Shows or hides all attached graphics entities. 
*/
void 
GraphicsProperty::SetVisible(bool visible)
{
    n_assert(this->modelEntity.isvalid());    
    this->modelEntity->SetVisible(visible);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::OnSetMaterialVariable( const Ptr<SetMaterialVariable>& msg )
{
    n_assert(this->modelEntity.isvalid());    
    Ptr<UpdModelNodeInstanceSurfaceConstant> updateMsg = UpdModelNodeInstanceSurfaceConstant::Create();
    updateMsg->SetName(msg->GetMaterialVarName());
    updateMsg->SetModelNodeInstanceName(msg->GetNodeName());
    updateMsg->SetValue(msg->GetValue());
    this->modelEntity->HandleMessage(updateMsg.downcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::OnMoveAfter()
{
	if (this->nodesDirty)
	{
		Ptr<GetModelNodes> getGraphicsNodeMessage = GetModelNodes::Create();
		getGraphicsNodeMessage->SetModelName("mdl:"+this->GetGraphicsResource()+".n3");
		GraphicsInterface::Instance()->Send(getGraphicsNodeMessage.downcast<Messaging::Message>());
		this->AddDeferredMessage(getGraphicsNodeMessage.upcast<Messaging::Message>());

		this->nodesDirty = false;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::OnReceiveModelNodes(const Ptr<GetModelNodes>& msg)
{
	Util::Array<Util::String> nodeNames = msg->GetNodeList();
	if (msg->GetNodeList().IsEmpty())
	{
		msg->SetHandled(false);
		GraphicsInterface::Instance()->Send(msg.downcast<Messaging::Message>());
		this->AddDeferredMessage(msg.upcast<Messaging::Message>());
	}
	else if (nodeNames.FindIndex("root") != InvalidIndex)
	{
		nodeNames.EraseIndex(nodeNames.FindIndex("root"));
	}
	modelNodeNames = nodeNames;

}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::SetupAnimEvents( const Ptr<Graphics::ModelEntity>& entity )
{
    // setup anim event tracking
    if (this->entity->GetBool(Attr::AnimEvents))
    {
        entity->ConfigureAnimEventTracking(true, false);
        GraphicsFeatureUnit::Instance()->GetDefaultAnimEventHandler()->AttachEntity(entity->GetId(),this->entity);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsProperty::RemoveAnimEvents( const Ptr<Graphics::ModelEntity>& entity )
{
    // setup anim event tracking
    if (this->entity->GetBool(Attr::AnimEvents))
    {        
        GraphicsFeatureUnit::Instance()->GetDefaultAnimEventHandler()->DetachEntity(entity->GetId());
    }
}


}; // namespace GraphicsFeature
