#pragma once
//------------------------------------------------------------------------------
/**
	@class EffectsFeature::EffectsFeatureUnit

    (C) 2013 Gustav Sterbrant
*/
#include "game/featureunit.h"
#include "game/entity.h"
#include "animeventregistry.h"
#include "vibration/vibrationplayer.h"
#include "effects/effect.h"
#include "mixer/effectmixer.h"
#include "graphicsfeature/managers/attachmentmanager.h"
#include "resources/resourceid.h"

namespace EffectsFeature
{
class EffectsFeatureUnit : public Game::FeatureUnit

{
__DeclareClass(EffectsFeatureUnit);
__DeclareSingleton(EffectsFeatureUnit); 
public:
    /// Constructor
    EffectsFeatureUnit();
    /// Destructor
    ~EffectsFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();

    /// called on begin of frame
    virtual void OnBeginFrame();    

	/// called from within GameServer::NotifyBeforeCleanup() before shutting down a level
	virtual void OnBeforeCleanup();

	/// create shake effect
	void EmitCameraShakeEffect(const Math::vector& intensity,
							   const Math::vector& rotation,
							   Timing::Time duration,
							   float range);

	/// create and send a vibration effect
	void EmitVibrationEffect(float highFreqIntensity,
							 Timing::Time highFreqDuration,
							 float lowFreqIntensity,
							 Timing::Time lowFreqDuration,
							 IndexT playerIndex);

	/// create and send an audio effect
	void EmitAudioEffect(const Math::point& position,
						 float volume ,
						 const FAudio::EventId& sound,
						 Timing::Time duration);

	/// create and send a graphics effect
	void EmitGraphicsEffect(const Math::matrix44& transform,
							const Resources::ResourceId& model,
							Timing::Time duration);

	/// create and send event effect
	void EmitAttachmentEvent(const Ptr<Graphics::GraphicsEntity>& entity,
							const Resources::ResourceId& attachment,
							const Util::StringAtom& joint,
							Timing::Time duration,
							Timing::Time delay,
							GraphicsFeature::AttachmentManager::AttachmentRotation rotationMode);

	/// add a effect directly
	void AttachEffect(const Ptr<EffectsFeature::Effect> & effect);

	/// attach a camerashake mixer
	void SetupCameraShakeMixer(const Math::vector& maxDisplacement, 
							   const Math::vector& maxThumble);
private:

	/// flushes all effects
	void FlushAll();
	Timing::Time curTime;
	Ptr<AnimEventRegistry> animRegistry;
	Ptr<Vibration::VibrationPlayer> vibrationPlayer;
	Util::Array<Ptr<Effect> > activeEffects;
	Util::Array<Ptr<EffectMixer> > activeEffectMixer;
    
};
}; // namespace FxFeatureUnit