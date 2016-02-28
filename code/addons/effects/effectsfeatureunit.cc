//------------------------------------------------------------------------------
//  effectsfeatureunit.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effectsfeatureunit.h"
#include "framesync/framesynctimer.h"
#include "effects/camerashakeeffect.h"
#include "effects/vibrationeffect.h"
#include "effects/audioeffect.h"
#include "effects/graphicseffect.h"
#include "effects/attachmenteffect.h"
#include "mixer/camerashakemixer.h"
#include "graphics/graphicsprotocol.h"
#include "effects/postfxeffect.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::EffectsFeatureUnit, 'FXFU' , Game::FeatureUnit);
__ImplementSingleton(EffectsFeature::EffectsFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
EffectsFeatureUnit::EffectsFeatureUnit()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
EffectsFeatureUnit::~EffectsFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();

	this->animRegistry = AnimEventRegistry::Create();
	this->animRegistry->Setup();

	this->vibrationPlayer = Vibration::VibrationPlayer::Create();

	// setup default mixer
	this->SetupCameraShakeMixer(Math::vector(1,1,1), Math::vector(1,1,1));
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::OnDeactivate()
{
	this->FlushAll();
	// discard registry
	this->animRegistry->Discard();
	this->animRegistry = 0;

	this->vibrationPlayer = 0;

    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::OnBeginFrame()
{
	// get current time
	Timing::Time time = FrameSync::FrameSyncTimer::Instance()->GetTime();

	// set time
	this->curTime = time;

	// update active effects
	int i;
	int num = this->activeEffects.Size();
	for (i = 0; i < num; i++)
	{
		this->activeEffects[i]->OnFrame(this->curTime);
	}

	// finally optionally mix and apply
	if(0 < num)
	{
		for(i = 0; i < this->activeEffectMixer.Size(); i++)
		{
			this->activeEffectMixer[i]->Mix(this->activeEffects, time);
			this->activeEffectMixer[i]->Apply();
		}
	}

	// garbage collect expired effects
	Util::Array<Ptr<Effect> >::Iterator effectIter;
	for (effectIter = this->activeEffects.Begin(); effectIter != this->activeEffects.End();)
	{
		if ((*effectIter)->IsFinished())
		{
			effectIter = this->activeEffects.Erase(effectIter);
		}
		else
		{
			effectIter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::EmitCameraShakeEffect( const Math::vector& intensity, const Math::vector& rotation, Timing::Time duration, float range)
{
	Ptr<CameraShakeEffect> effect = CameraShakeEffect::Create();
	effect->SetIntensity(intensity);
	effect->SetRotation(rotation);
	effect->SetDuration(duration);
	effect->SetRange(range);	

	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::EmitVibrationEffect( float highFreqIntensity, Timing::Time highFreqDuration, float lowFreqIntensity, Timing::Time lowFreqDuration, IndexT playerIndex )
{
	Ptr<VibrationEffect> effect = VibrationEffect::Create();
	effect->SetHighFreqIntensity(highFreqIntensity);
	effect->SetHighFreqDuration(highFreqDuration);
	effect->SetLowFreqIntensity(lowFreqIntensity);
	effect->SetLowFreqDuration(lowFreqDuration);
	effect->SetPlayerIndex(playerIndex);

	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::EmitAudioEffect(const Math::point& position, float volume, const FAudio::EventId& sound, Timing::Time duration)
{
	Ptr<AudioEffect> effect = AudioEffect::Create();
	effect->SetEventResource(sound);
	effect->SetVolume(volume);
	Math::matrix44 trans;
	trans.set_position(position);
	effect->SetTransform(trans);
	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::EmitGraphicsEffect( const Math::matrix44& transform, const Resources::ResourceId& model, Timing::Time duration )
{
	Ptr<GraphicsEffect> effect = GraphicsEffect::Create();
	effect->SetResource(model);
	effect->SetDuration(duration);
	effect->SetTransform(transform);

	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::EmitAttachmentEvent( const Ptr<Graphics::GraphicsEntity>& entity,
									 const Resources::ResourceId& attachment, 
									 const Util::StringAtom& joint, 
									 Timing::Time duration, 
									 Timing::Time delay, 									 
									 GraphicsFeature::AttachmentManager::AttachmentRotation rotationMode )
{
	Ptr<AttachmentEffect> effect = AttachmentEffect::Create();
	effect->SetJoint(joint);
	effect->SetBaseEntity(entity.cast<Graphics::ModelEntity>());
	effect->SetAttachmentResource(attachment);
	effect->SetStartDelay(delay);
	effect->SetDuration(duration);
	effect->SetRotation(rotationMode);

	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::SetupCameraShakeMixer( const Math::vector& maxDisplacement, const Math::vector& maxThumble )
{
	Ptr<CameraShakeMixer> mixer = CameraShakeMixer::Create();
	mixer->SetMaxDisplacement(maxDisplacement);
	mixer->SetMaxTumble(maxThumble);

	// add mixer
	this->activeEffectMixer.Append(mixer.upcast<EffectMixer>());
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectsFeatureUnit::FlushAll()
{
	// cleanup active effects
	int i;
	int num = this->activeEffects.Size();
	for (i = 0; i < num; i++)
	{
		if (!this->activeEffects[i]->IsFinished())
		{
			this->activeEffects[i]->OnDeactivate();
		}
		this->activeEffects[i] = 0;
	}
	this->activeEffects.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::AttachEffect(const Ptr<EffectsFeature::Effect> & effect)
{
	// add affect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect);
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::EmitPostFXEvent(const Util::String & preset, Timing::Time duration)
{
	Ptr<PostFXEffect> effect = PostFXEffect::Create();
	effect->SetPreset(preset);
	effect->SetDuration(duration);
	// add effect
	effect->OnActivate(this->curTime);
	this->activeEffects.Append(effect.upcast<Effect>());
}

//------------------------------------------------------------------------------
/**
*/
void
EffectsFeatureUnit::OnBeforeCleanup()
{
	this->FlushAll();
}

};