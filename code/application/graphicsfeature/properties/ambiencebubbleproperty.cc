//------------------------------------------------------------------------------
//  ambiencebubbleproperty.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/ambiencebubbleproperty.h"
#include "game/entity.h"
#include "posteffect/posteffectmanager.h"
#include "properties/lightproperty.h"
//#include "msg/EnableAmbienceBubble.h"
//#include "msg/attributesupdated.h"
//#include "gfx2/ngfxserver2.h"
//#include "managers/debugmanager.h"
#include "basegamefeature/basegameprotocol.h"
#include "math/polar.h"
#include "graphicsfeatureunit.h"
#include "posteffectprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "posteffectregistry.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::AmbienceBubbleProperty, 'AMBP', Game::Property);

using namespace BaseGameFeature;
using namespace Math;
//------------------------------------------------------------------------------
/**
*/
AmbienceBubbleProperty::AmbienceBubbleProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AmbienceBubbleProperty::~AmbienceBubbleProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, RenderDebug);
}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::OnActivate()
{
    Property::OnActivate();

    // create a posteffect entity and register with the post effect server
    this->postEffectEntity = PostEffect::PostEffectEntity::Create();
    this->postEffectEntity->SetDefaultEntity(this->entity->GetBool(Attr::PEDefaultEntity));
    this->postEffectEntity->SetShapeType(PostEffect::PostEffectEntity::StringToShapeType(this->entity->GetString(Attr::PEShapeType)));    
    this->postEffectEntity->SetTransform(this->entity->GetMatrix44(Attr::Transform));
    this->postEffectEntity->SetEnabled(this->entity->GetBool(Attr::AmbienceBubbleEnabled));
    this->postEffectEntity->SetPriority(this->entity->GetInt(Attr::AmbienceBubblePriority));
    this->UpdatePostEffectEntityFromAttrs();	
    PostEffect::PostEffectManager::Instance()->AttachEntity(this->postEffectEntity);

}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::OnDeactivate()
{
    PostEffect::PostEffectManager::Instance()->RemoveEntity(this->postEffectEntity);
    this->postEffectEntity = 0;
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(BaseGameFeature::UpdateTransform::Id);
    this->RegisterMessage(AttributesUpdated::Id);
    this->RegisterMessage(BaseGameFeature::EnableAmbienceBubble::Id);
    Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg);
    if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
    {
        const Ptr<BaseGameFeature::UpdateTransform>& updTransform = msg.cast<BaseGameFeature::UpdateTransform>();
		if(this->postEffectEntity.isvalid())
		{			
			this->postEffectEntity->SetTransform(updTransform->GetMatrix());			
		}
	}
    else if (msg->CheckId(AttributesUpdated::Id))
    {   		
		// this is usually coming only from the level editor
		this->UpdatePostEffectEntityFromAttrs();          
    }
    
    else if (msg->CheckId(BaseGameFeature::EnableAmbienceBubble::Id))
    { 
        const Ptr<BaseGameFeature::EnableAmbienceBubble>& enable = msg.cast<BaseGameFeature::EnableAmbienceBubble>();
        this->GetEntity()->SetBool(Attr::AmbienceBubbleEnabled, enable->GetEnabled());
		
		if(postEffectEntity.isvalid())
		{
			this->postEffectEntity->SetEnabled(enable->GetEnabled());
		}
    }
    else
    {
        Game::Property::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    This is only executes at activation or when an AttributesUpdated
    message is coming in.
*/
void
AmbienceBubbleProperty::UpdatePostEffectEntityFromAttrs()
{
	PostEffect::PostEffectRegistry::Instance()->ApplySettings(this->entity->GetString(Attr::PostEffectPreset), this->postEffectEntity);
    
}

//------------------------------------------------------------------------------
/**
*/
void
AmbienceBubbleProperty::OnRenderDebug()
{
	
	float4 colorEnabled(0.4f, 0.88f, 1.0f, 0.3f); 
	float4 colorDisabled(0.1f, 0.0f, 0.0f, 0.08f);

	// check if sphere shape or box
	matrix44 transform = this->GetEntity()->GetMatrix44(Attr::Transform);
	float4 color = colorDisabled;
	if (this->entity->GetBool(Attr::AmbienceBubbleEnabled))
	{
		color = colorEnabled;
	}
	switch(PostEffect::PostEffectEntity::StringToShapeType(this->entity->GetString(Attr::PEShapeType)))
	{
		case PostEffect::PostEffectEntity::Sphere:	
			Debug::DebugShapeRenderer::Instance()->DrawSphere(transform, color);   
		break;
		
		case PostEffect::PostEffectEntity::Box:
			Debug::DebugShapeRenderer::Instance()->DrawBox(transform, color);   
	}
}

} // namespace Properties

