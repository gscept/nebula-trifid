#pragma once
//------------------------------------------------------------------------------
/**
	@class FAudio::AudioEmitterProperty

	Simple audio emitter that contains an eventinstance and will track its position.
	Supports play/stop

	(C) 2015 Individual contributors, see AUTHORS file
*/

#include "game/property.h"
#include "faudio/eventinstance.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class AudioEmitterProperty : public Game::Property
{
	__DeclareClass(AudioEmitterProperty);
	__SetupExternalAttributes();
public:
	/// constructor
	AudioEmitterProperty();
	/// destructor
	virtual ~AudioEmitterProperty();

	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
	/// called from Entity::DeactivateProperties()
	virtual void OnDeactivate();
	/// called when entity moves out of activity zone
	virtual void OnLoseActivity();
	/// called when entity moves inside activity zone
	virtual void OnGainActivity();
	/// setup callbacks for this property, call by entity in OnActivate()
	virtual void SetupCallbacks();
	/// override to register accepted messages
	virtual void SetupAcceptedMessages();
	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
	/// update 3d parameters
	void Update3DParameters();

	Ptr<FAudio::EventInstance> eventInstance;
	bool wasPlaying;
};
	__RegisterClass(AudioEmitterProperty);
}; // namespace FAudio