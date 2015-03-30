//------------------------------------------------------------------------------
//  effect.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effect.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::Effect, 'CFXE',Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Effect::Effect() :
    activationTime(0.0),
    startDelay(0.0),    
    startTime(0.0),
    duration(0.0),
    isWaiting(false),
    isPlaying(false),
    isFinished(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Effect::~Effect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Activate the effect. If no start delay is defined (the default) it 
    immediately starts playing, otherwise it starts waiting.
*/
void
Effect::OnActivate(Timing::Time time)
{
    n_assert((!this->IsWaiting() && !this->IsPlaying() && !this->IsFinished()));
    this->activationTime = time;
    if (this->startDelay > 0.0)
    {
        this->isWaiting = true;        
    }
    else
    {
        this->OnStart(time);
    }
}

//------------------------------------------------------------------------------
/**
    Starts the effect. This is either called directly by OnActivate() if
    no start delay is defined, or after the start delay is over from
    OnFrame().
*/
void
Effect::OnStart(Timing::Time time)
{
    n_assert(!this->IsPlaying() && !this->IsFinished());
    this->isWaiting = false;
    this->isPlaying = true;
    this->startTime = time;
}

//------------------------------------------------------------------------------
/**
    Deactivates the effect and sets its state to finished. This is usually
    called from OnFrame() when the effect has expired.
*/
void
Effect::OnDeactivate()
{
    this->isWaiting = false;
    this->isPlaying = false;
    this->isFinished = true;
}

//------------------------------------------------------------------------------
/**
    Called once per frame by the FxServer.
*/
void
Effect::OnFrame(Timing::Time time)
{
    n_assert(!this->IsFinished());

    // if waiting, check if start delay is over...
    if (this->IsWaiting())
    {
        Timing::Time timeDiff = time - this->activationTime;
        if ((timeDiff > this->startDelay)/* || (timeDiff < 0.0)<<<== why this [relict from N2]?!?!?*/)
        {
            this->OnStart(time);
        }
    }

    // if we're playing, check if we're expired
    if (this->IsPlaying())
    {
        Timing::Time timeDiff = time - this->startTime;
        if (timeDiff > this->duration && this->duration != 0.0f)
        {
            this->OnDeactivate();
        }
    }
}

} // namespace VfxFeature