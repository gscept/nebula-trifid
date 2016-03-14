//------------------------------------------------------------------------------
//  audioemitterproperty.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audioemitterproperty.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "faudio/audiodevice.h"
#include "faudio/audioprotocol.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "basegamefeature/basegameprotocol.h"

namespace FAudio
{
__ImplementClass(FAudio::AudioEmitterProperty, 'AUEP', Game::Property);
	

//------------------------------------------------------------------------------
/**
*/
AudioEmitterProperty::AudioEmitterProperty():
wasPlaying(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AudioEmitterProperty::~AudioEmitterProperty()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::OnStart()
{
	Property::OnStart();
	EventId id = this->entity->GetString(Attr::AudioEvent);
	if (id.IsValid())
	{
		this->eventInstance = AudioDevice::Instance()->CreateEvent(id);
		this->Update3DParameters();
		if (this->entity->GetBool(Attr::AutoPlay))
		{
			this->eventInstance->Play();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::OnDeactivate()
{
	if (this->eventInstance.isvalid())
	{
		AudioDevice::Instance()->DiscardEvent(this->eventInstance);
		this->eventInstance = 0;
	}
	Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::SetupCallbacks()
{	
	this->entity->RegisterPropertyCallback(this, LoseActivity);
	this->entity->RegisterPropertyCallback(this, GainActivity);
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::SetupAcceptedMessages()
{
	Property::RegisterMessage(EmitterPlay::Id);
	Property::RegisterMessage(PlayEvent::Id);
	Property::RegisterMessage(BaseGameFeature::UpdateTransform::Id);	
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	n_assert(msg != 0);

	if (msg->CheckId(EmitterPlay::Id))
	{
		if (this->eventInstance.isvalid())
		{
			Ptr<EmitterPlay> pmsg = msg.cast<EmitterPlay>();
			if (pmsg->GetEnable())
			{
				this->eventInstance->Play();
			}
			else
			{
				this->eventInstance->Stop();
			}
		}
		else
		{
			n_warning("Trying to play/stop invalid audio event instance\n");
		}
	}
	else if (msg->CheckId(PlayEvent::Id))
	{
		Ptr<PlayEvent> pmsg = msg.cast<PlayEvent>();
		EventId newTrack = pmsg->GetEvent();
		float volume = pmsg->GetVolume();
		Math::vector vel(0);
		if (this->entity->HasAttr(Attr::VelocityVector))
		{
			vel = this->entity->GetFloat4(Attr::VelocityVector);
		}
		AudioDevice::Instance()->EventPlayFireAndForget3D(newTrack, this->entity->GetMatrix44(Attr::Transform), vel, volume);
	}
	else if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
	{		
		this->Update3DParameters();
	}
	Game::Property::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::OnLoseActivity()
{
	if (this->eventInstance->GetState() == EventPlaying)
	{
		this->wasPlaying = true;
		this->eventInstance->Pause();
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::OnGainActivity()
{
	if (this->wasPlaying)
	{
		this->eventInstance->Resume();
		this->wasPlaying = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEmitterProperty::Update3DParameters()
{
    if (this->eventInstance.isvalid())
    {
        const Math::matrix44 trans = this->entity->GetMatrix44(Attr::Transform);
        Math::vector velocity(0);
        if (this->entity->HasAttr(Attr::VelocityVector))
        {
            velocity = this->entity->GetFloat4(Attr::VelocityVector);
        }
        Math::vector forward = trans.get_zaxis();
        this->eventInstance->Set3DAttributes(trans.get_position(), velocity, &forward);
    }
}

}; // namespace Game
