//------------------------------------------------------------------------------
//  animeventsoundhandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxanimeventhandler/animeventtiminghandler.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "debugrender/debugtextrenderer.h"
#include "threading/thread.h"

namespace FX
{
__ImplementClass(FX::AnimEventTimingHandler, 'AETH', Animation::AnimEventHandlerBase);

using namespace InternalGraphics;
using namespace Graphics;

//------------------------------------------------------------------------------ 
/**
*/
AnimEventTimingHandler::AnimEventTimingHandler() : curTimingActive(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEventTimingHandler::~AnimEventTimingHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventTimingHandler::HandleEvent(const Animation::AnimEventInfo& event)
{
    // first check if we got that attachment event    
    if (AnimEventRegistry::Instance()->HasTimingEvent(event.GetAnimEvent().GetName()))
    {
        this->curTiming = AnimEventRegistry::Instance()->GetTimingEvent(event.GetAnimEvent().GetName());
        this->curTiming.startTime = this->timer.GetTicks();
        this->curTimingActive = true;
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    ATTENTION DO NOT USE THE TIME GIVEN VIA PARAMETER
    THIS ABSOLUTE TIME SHOULD BE MANIPULATED AND NOT CORRECT FOR
    TIME CALCULATION IN THIS HANDLER
*/
void
AnimEventTimingHandler::OnFrame(Timing::Time time)
{
    // first set time    
    Timing::Tick ownTime = this->timer.GetTicks();

    // check if active
    if (this->curTimingActive)
    {
        Timing::Tick absoluteSustainTime = this->curTiming.startTime + this->curTiming.attack;
        Timing::Tick absoluteRelaseTime = absoluteSustainTime + this->curTiming.sustain;
        Timing::Tick absoluteEndTime = absoluteRelaseTime + this->curTiming.release;

        // in attack phase?
        if (ownTime < absoluteSustainTime)
        {
            float value = (float)(ownTime - this->curTiming.startTime) / (float)(this->curTiming.attack);
            this->curFactor = Math::n_lerp(1.0f, this->curTiming.factor, value);
        } 
        // in sustain phase?
        else if (ownTime < absoluteRelaseTime)
        {   
            this->curFactor = this->curTiming.factor;
        }
        // in release phase?
        else if (ownTime < absoluteEndTime)
        {
            float value = (float)(ownTime - absoluteRelaseTime) / (float)(this->curTiming.release);
            this->curFactor = Math::n_lerp(this->curTiming.factor, 1.0f, value);
        }
        // its over
        else
        {
            this->curFactor = 1.0f;
            this->curTimingActive = false;
        }

        Ptr<SetTimeFactor> setFactor = SetTimeFactor::Create();
        setFactor->SetFactor(this->curFactor);
        GraphicsInterface::Instance()->Send(setFactor.cast<Messaging::Message>());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimEventTimingHandler::Open()
{
    AnimEventHandlerBase::Open();
    this->timer.Start();
}

//------------------------------------------------------------------------------
/**
*/
void 
AnimEventTimingHandler::Close()
{
    this->timer.Stop();
    AnimEventHandlerBase::Close();
}
} // namespace FX
