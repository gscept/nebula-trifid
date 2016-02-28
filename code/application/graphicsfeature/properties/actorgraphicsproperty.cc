//------------------------------------------------------------------------------
//  actorgraphicsproperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/actorgraphicsproperty.h"
#include "graphics/graphicsprotocol.h"
#include "graphicsfeatureunit.h"
#include "multiplayer/networkentity.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::ActorGraphicsProperty, 'AGPR', GraphicsFeature::GraphicsProperty);

using namespace Util;
using namespace Graphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::SetupAcceptedMessages()
{
    GraphicsProperty::SetupAcceptedMessages();
    this->RegisterMessage(Graphics::AnimPlayClip::Id);
    this->RegisterMessage(Graphics::AnimStopTrack::Id);
    this->RegisterMessage(Graphics::AnimStopAllTracks::Id);
    this->RegisterMessage(Graphics::AnimIsClipPlaying::Id);
    this->RegisterMessage(Graphics::AnimModifyBlendWeight::Id);
    this->RegisterMessage(Graphics::AnimModifyTimeFactor::Id);
    this->RegisterMessage(Graphics::ApplySkinList::Id);
    this->RegisterMessage(Graphics::ShowSkin::Id);
    this->RegisterMessage(Graphics::HideSkin::Id);	
}

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg);
    if (msg->IsA(GraphicsEntityMessage::RTTI))
    {
        this->modelEntity->HandleMessage(msg);
		__DistributeNetworkMessage(this->entity, msg);
    }
    else
    {
        GraphicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorGraphicsProperty::SetupGraphics()
{
    const String& resName = this->GetGraphicsResource();
    const Ptr<Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    const matrix44& worldMatrix = this->entity->GetMatrix44(Attr::Transform);
    const String& skinList = this->entity->GetString(Attr::CharacterSet);

    String resId;
    resId.Format("mdl:%s.n3", resName.AsCharPtr());

    this->modelEntity = ModelEntity::Create();
    this->modelEntity->SetResourceId(resId);
    this->modelEntity->SetTransform(worldMatrix);
    this->modelEntity->SetLoadSynced(this->loadSync);
	this->modelEntity->SetPickingId(this->GetEntity()->GetUniqueId());
    stage->AttachEntity(this->modelEntity.upcast<GraphicsEntity>());
    
    // set start animation to immediately start a valid animation
    const Util::String& startAnimName = GetEntity()->GetString(Attr::StartAnimation);
	if(!startAnimName.IsEmpty())
	{
		Ptr<Graphics::AnimPlayClip> startAnimMsg = Graphics::AnimPlayClip::Create();
		startAnimMsg->SetClipName(startAnimName);
		startAnimMsg->SetTrackIndex(0);
		if (this->entity->GetBool(Attr::LoopStartAnimation))
		{
			startAnimMsg->SetLoopCount(0.0f);
		}
		else
		{
			startAnimMsg->SetLoopCount(1.0f);
		}
		startAnimMsg->SetFadeInTime(200);
		startAnimMsg->SetFadeOutTime(200);
		this->modelEntity->HandleMessage(startAnimMsg.cast<Messaging::Message>());
	}

    Ptr<Graphics::ApplySkinList> applySkinList = Graphics::ApplySkinList::Create();
    applySkinList->SetSkinList(skinList);
    this->modelEntity->HandleMessage(applySkinList.cast<Messaging::Message>());
    this->SetupAnimEvents(this->modelEntity);
}

} // namespace GraphicsFeature