//------------------------------------------------------------------------------
//  fxfeature/fxmanager.cc
//  (C) 2009 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/fxfeature/fxmanager.h"
#include "fxprotocol.h"
#include "graphics/graphicsinterface.h"

namespace FxFeature
{
__ImplementClass(FxFeature::FxManager, 'VFVS', Core::RefCounted);
__ImplementSingleton(FxFeature::FxManager);

using namespace FX;

//------------------------------------------------------------------------------
/**
*/
FxManager::FxManager() :
    isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FxManager::~FxManager()
{
    if (this->isOpen)
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
FxManager::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FxManager::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
FxManager::SetupCameraShakeMixer(const Math::vector& maxDisplacement, const Math::vector& maxThumble)
{
    // create mixer and send
	Ptr<CreateCameraEffectMixer> mixer = CreateCameraEffectMixer::Create();
	mixer->SetDisplacement(maxDisplacement);
	mixer->SetTumble(maxThumble);
    Graphics::GraphicsInterface::Instance()->SendBatched(mixer.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
FxManager::EmitCameraShakeEffect(const Math::vector& intensity,
                                 const Math::vector& rotation,
                                 Timing::Time duration,
                                 float range,
                                 const Math::matrix44& transform)
{
    // create effect and send
    Ptr<CreateCameraShakeEffect> camShake = CreateCameraShakeEffect::Create();
    camShake->SetIntensity(intensity);
    camShake->SetRotation(rotation);
    camShake->SetDuration(duration);
    camShake->SetRange(range);
    camShake->SetTransform(transform);
    Graphics::GraphicsInterface::Instance()->SendBatched(camShake.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
FxManager::EmitVibrationEffect( float highFreqIntensity,
                                Timing::Time highFreqDuration,
                                float lowFreqIntensity,
                                Timing::Time lowFreqDuration,
                                IndexT playerIndex)
{
    // create fx
    Ptr<CreateVibrationEffect> vibFx = CreateVibrationEffect::Create();
    vibFx->SetHighFrequencyDuration(highFreqDuration);
    vibFx->SetHighFrequencyIntensity(highFreqIntensity);
    vibFx->SetLowFrequencyDuration(lowFreqDuration);
    vibFx->SetLowFrequencyIntensity(lowFreqIntensity);
    vibFx->SetPlayerIndex(playerIndex);
    Graphics::GraphicsInterface::Instance()->SendBatched(vibFx.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
FxManager::EmitAudioEffect( const Math::point& position, 
						    int amplitude, 
							const Resources::ResourceId& sound, 
							Timing::Time duration)
{
	// create fx
	Ptr<CreateAudioEffect> audFx = CreateAudioEffect::Create();
	audFx->SetPosition(position);
	audFx->SetAmplitude(amplitude);
	audFx->SetSound(sound);
	audFx->SetDuration(duration);
	Graphics::GraphicsInterface::Instance()->SendBatched(audFx.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
FxManager::EmitGraphicsEffect( const Math::matrix44& transform, 
							    const Resources::ResourceId& model, 
								Timing::Time duration)
{
	// create fx
	Ptr<CreateGraphicsEffect> graFx = CreateGraphicsEffect::Create();
	graFx->SetTransform(transform);
	graFx->SetModel(model);
	graFx->SetDuration(duration);
	Graphics::GraphicsInterface::Instance()->SendBatched(graFx.upcast<Messaging::Message>());
}
} // namespace FxFeature