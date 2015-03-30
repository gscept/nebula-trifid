//------------------------------------------------------------------------------
//  fx/rt/fxserver.cc
//  (C) 2009 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxserver.h"

namespace FX
{
__ImplementClass(FX::FxServer, 'CFXS', Core::RefCounted);
__ImplementSingleton(FX::FxServer);

//------------------------------------------------------------------------------
/**
*/
FxServer::FxServer() :
    isOpen(false),
    curTime(0.0),
    activeEffects(256, 256)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
FxServer::~FxServer()
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
FxServer::Open()
{
    n_assert(!this->isOpen);

    // create vibration player
    this->vibrationPlayer = Vibration::VibrationPlayer::Create();

	// setup anim event registry if not yet done
	this->animEventRegistry = FX::AnimEventRegistry::Create();
	//this->animEventRegistry->Setup();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
FxServer::Close()
{
    n_assert(this->isOpen);

    // cleanup active effects
    this->FlushAll();

    this->vibrationPlayer = 0;

	//this->animEventRegistry->Discard();
	this->animEventRegistry = 0;

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
FxServer::FlushAll()
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
    Attach a generic effect to the world.
*/
void
FxServer::AttachEffect(const Ptr<Effect>& effect)
{
    n_assert(effect.isvalid());
    effect->OnActivate(this->curTime);
    this->activeEffects.Append(effect);
}

//------------------------------------------------------------------------------
/**
    Remove an effect from the world.
*/
void
FxServer::RemoveEffect(const Ptr<Effect>& effect)
{
    n_assert(effect.isvalid());
    int index = this->activeEffects.FindIndex(effect);
    if (-1 != index)
    {
        if (!this->activeEffects[index]->IsFinished())
        {
            this->activeEffects[index]->OnDeactivate();
        }
        this->activeEffects.EraseIndex(index);
    }
}

//------------------------------------------------------------------------------
/**
	Attach a generic effect to the world.
*/
void
FxServer::AttachEffectMixer(const Ptr<EffectMixer>& mixer)
{
    n_assert(mixer.isvalid());
    this->activeEffectMixer.Append(mixer);
}

//------------------------------------------------------------------------------
/**
Remove an effect from the world.
*/
void
FxServer::RemoveEffectMixer(const Ptr<EffectMixer>& mixer)
{
    n_assert(mixer.isvalid());
    int index = this->activeEffectMixer.FindIndex(mixer);
    if (-1 != index)
    {
        this->activeEffectMixer.EraseIndex(index);
    }
}

//------------------------------------------------------------------------------
/**
    The EndScene() method must be called after rendering has happened.
    Expired effects will be removed here.
*/
void
FxServer::OnFrame(const Timing::Time& time)
{
    // set time
    this->curTime = time;

    // update active effects
    int i;
    int num = this->activeEffects.Size();
    for (i = 0; i < num; i++)
    {
        if (this->activeEffects[i]->IsPlaying())
        {
            this->activeEffects[i]->OnFrame(this->curTime);
        }
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
} // namespace VfxFeature