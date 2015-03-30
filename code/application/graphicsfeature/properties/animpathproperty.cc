//------------------------------------------------------------------------------
//  animpathproperty.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animpathproperty.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "io/xmlreader.h"
#include "io/ioserver.h"
#include "resources/resourcemanager.h"
#include "animpath/pathanimation.h"
#include "framesync/framesynctimer.h"

using namespace Math;
using namespace BaseGameFeature;
using namespace IO;
namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::AnimPathProperty, 'AMPP', Game::Property);

//------------------------------------------------------------------------------
/**
*/
AnimPathProperty::AnimPathProperty() :
	currentTrack(NULL),
	lastSample(0.0f),
	paused(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnimPathProperty::~AnimPathProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::SetupCallbacks()
{
	Game::Property::SetupCallbacks();
	this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::SetupAcceptedMessages()
{
	Game::Property::SetupAcceptedMessages();
	this->RegisterMessage(AnimPlay::Id);
	this->RegisterMessage(AnimStop::Id);
	this->RegisterMessage(AnimRewind::Id);
	this->RegisterMessage(AnimTrack::Id);
	this->RegisterMessage(SetTransform::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::OnActivate()
{
	Game::Property::OnActivate();
	this->pos = this->entity->GetMatrix44(Attr::Transform).get_position();
	
	// get resource from entity and load resource
	// TODO: handle failed load? must it be synced?
	Util::String resource = this->entity->GetString(Attr::AnimPath);
	this->animPath = Resources::ResourceManager::Instance()->CreateManagedResource(PathAnimation::RTTI, resource, NULL, true).downcast<ManagedPathAnimation>();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::OnDeactivate()
{
	Resources::ResourceManager::Instance()->DiscardManagedResource(this->animPath.upcast<Resources::ManagedResource>());
	this->currentTrack = 0;
	this->animPath = 0;
	Game::Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::OnBeginFrame() 
{
	if (!this->paused && this->currentTrack.isvalid())
	{
		float currentSample = (float)FrameSync::FrameSyncTimer::Instance()->GetFrameTime();
		this->Update();

		Util::Array<Util::String> events = this->currentTrack->GetEvents(this->lastSample, this->lastSample + currentSample);
		IndexT i;
		for (i = 0; i < events.Size(); i++)
		{
			Ptr<AnimEventTriggered> animEventTriggered = AnimEventTriggered::Create();
			animEventTriggered->SetName(events[i]);
			__SendSync(this->entity, animEventTriggered);
		}
		
		// add one frame
		this->lastSample += currentSample;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(AnimPlay::Id))
	{
		this->paused = false;
	}
	else if (msg->CheckId(AnimRewind::Id))
	{
		this->lastSample = 0.0f;
	}
	else if (msg->CheckId(AnimStop::Id))
	{
		this->paused = true;
	}
	else if (msg->CheckId(AnimTrack::Id))
	{
		// if we change tracks we should also rewind
		Ptr<AnimTrack> rmsg = msg.downcast<AnimTrack>();
		Ptr<PathAnimationTrack> track = this->animPath->GetPathAnimation()->GetTrack(rmsg->GetTrack());

		// only switch and rewind if we actually change track
		if (track != this->currentTrack)
		{
			this->currentTrack = track;
			this->lastSample = 0.0f;
			this->Update();
		}		
	}
	else if (msg->CheckId(SetTransform::Id))
	{
		Ptr<SetTransform> rmsg = msg.downcast<SetTransform>();
		matrix44 trans = rmsg->GetMatrix();
		this->pos = trans.get_position();
	}
	else
	{
		Game::Property::HandleMessage(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AnimPathProperty::Update()
{
	Math::float4 translation = this->currentTrack->translation.Sample(this->lastSample);
	Math::float4 rotation = this->currentTrack->rotation.Sample(this->lastSample);
	Math::float4 scale = this->currentTrack->scaling.Sample(this->lastSample);

	matrix44 transform = matrix44::scaling(scale);
	transform = matrix44::multiply(transform, matrix44::rotationyawpitchroll(rotation.y(), rotation.x(), rotation.z()));
	transform.set_position(translation + this->pos);

	Ptr<UpdateTransform> msg = UpdateTransform::Create();
	msg->SetMatrix(transform);
	__SendSync(this->entity, msg);
	//this->entity->SetMatrix44(Attr::Transform, transform);
}

} // namespace GraphicsFeature