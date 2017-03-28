//------------------------------------------------------------------------------
//  vibrationhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vibration/vibrationhandler.h"
#include "core/config.h"
#include "input/inputserver.h"
#include "input/gamepad.h"

namespace Vibration
{
__ImplementClass(Vibration::VibrationHandler, 'VIBH', Interface::InterfaceHandlerBase);

using namespace Util;
using namespace Interface;
using namespace Timing;
using namespace Messaging;

const Time VibrationHandler::VibrationTriggerInterval = 0.1;

//------------------------------------------------------------------------------
/**
*/
VibrationHandler::VibrationHandler() : curHiFreqValues(NEBULA3_MAX_LOCAL_PLAYERS),
                                       curLowFreqValues(NEBULA3_MAX_LOCAL_PLAYERS), 
                                       targetVibratos(NEBULA3_MAX_LOCAL_PLAYERS),
                                       vibratosRunnung(NEBULA3_MAX_LOCAL_PLAYERS),
                                       lastTriggerTime(0),
                                       reseted(false)
{
    IndexT indeX;
    for (indeX = 0; indeX < NEBULA3_MAX_LOCAL_PLAYERS; indeX++)
    {
        this->curHiFreqValues[indeX] = 0;
        this->curLowFreqValues[indeX] = 0;
        this->vibratosRunnung[indeX] = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
VibrationHandler::~VibrationHandler()
{
    // empty
}            

//------------------------------------------------------------------------------
/**
*/
void
VibrationHandler::Open()
{
    n_assert(!this->IsOpen());

    // start the timer for measuring elapsed time
    this->timer.Start();

    InterfaceHandlerBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationHandler::Close()
{
    n_assert(this->IsOpen());

    // stop timer
    this->timer.Stop();

    InterfaceHandlerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationHandler::DoWork()
{
    // check if it's time to update 
    Time curTime = this->timer.GetTime();
    Time diffTime = curTime - this->lastTriggerTime;
    if (diffTime < VibrationTriggerInterval)
    {        
        return;
    }
    this->lastTriggerTime = curTime;

    // calc ...
    if (this->CalculateValues())
    {
        // ... and set
        this->ApplyValues();
        this->reseted = false;
    }
    else if (!this->reseted)
    {
        IndexT playerIdx;
        for (playerIdx = 0; playerIdx < NEBULA3_MAX_LOCAL_PLAYERS; playerIdx++)
        {
            this->curLowFreqValues[playerIdx] = 0;
            this->curHiFreqValues[playerIdx] = 0;
        }
        this->ApplyValues();
        this->reseted = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationHandler::ApplyValues()
{
    Input::InputServer* inputServer = Input::InputServer::Instance();    
    IndexT playerIdx;
    for (playerIdx = 0; playerIdx < NEBULA3_MAX_LOCAL_PLAYERS; playerIdx++)
    {
        const Ptr<Input::GamePad>& gamePad = inputServer->GetDefaultGamePad(playerIdx);
		if (gamePad.isvalid())
		{
			gamePad->SetLowFrequencyVibrator(this->curLowFreqValues[playerIdx]);
			gamePad->SetHighFrequencyVibrator(this->curHiFreqValues[playerIdx]);
		}
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
VibrationHandler::CalculateValues()
{
    bool dirty = false;

    // get time
    Timing::Time time = this->timer.GetTime();
    
    // do for each player
    IndexT playerIdx;
    for (playerIdx = 0; playerIdx < NEBULA3_MAX_LOCAL_PLAYERS; playerIdx++)
    {
        // do just if running
        if (this->vibratosRunnung[playerIdx])
        {
            // if at least one running, dirty
            dirty = true;

            const Vibrato& target = this->targetVibratos[playerIdx];
            float& curLow = this->curLowFreqValues[playerIdx];
            float& curHi = this->curHiFreqValues[playerIdx];
            bool hiRunning = true;
            bool lowRunning = true;

            // ===================================================================================== HI FREQ
            Timing::Time absoluteHiSustainTime = target.startTime + target.highFreqAttack;
            Timing::Time absoluteHiRelaseTime = absoluteHiSustainTime + target.highFreqSustain;
            Timing::Time absoluteHiEndTime = absoluteHiRelaseTime + target.highFreqRelease;

            // in attack phase?
            if (time < absoluteHiSustainTime)
            {
                float value = (float)(time - target.startTime) / (float)(target.highFreqAttack);
                curHi = Math::n_lerp(curHi, target.highFreqIntensity, value);
            } 
            // in sustain phase?
            else if (time < absoluteHiRelaseTime)
            {   
                curHi = target.highFreqIntensity;
            }
            // in release phase?
            else if (time < absoluteHiEndTime)
            {
                float value = (float)(time - absoluteHiRelaseTime) / (float)(target.highFreqRelease);
                curHi = Math::n_lerp(target.highFreqIntensity, 0, value);
            }
            // its over
            else
            {
                curHi = 0;
                hiRunning = false;
            }

            // ===================================================================================== LOW FREQ
            Timing::Time absoluteLowSustainTime = target.startTime + target.lowFreqAttack;
            Timing::Time absoluteLowRelaseTime = absoluteLowSustainTime + target.lowFreqSustain;
            Timing::Time absoluteLowEndTime = absoluteLowRelaseTime + target.lowFreqRelease;

            // in attack phase?
            if (time < absoluteLowSustainTime)
            {
                float value = (float)(time - target.startTime) / (float)(target.lowFreqAttack);
                curLow = Math::n_lerp(curLow, target.lowFreqIntensity, value);
            } 
            // in sustain phase?
            else if (time < absoluteLowRelaseTime)
            {   
                curLow = target.lowFreqIntensity;
            }
            // in release phase?
            else if (time < absoluteLowEndTime)
            {
                float value = (float)(time - absoluteLowRelaseTime) / (float)(target.lowFreqRelease);
                curLow = Math::n_lerp(target.lowFreqIntensity, 0, value);
            }
            // its over
            else
            {
                curLow = 0;
                lowRunning = false;
            }

            // stop whole anim?
            if (!lowRunning && !hiRunning)
            {
                this->vibratosRunnung[playerIdx] = false;
            }
        }
    }
    return dirty;
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationHandler::OnAddEnvelopeVibration(const Ptr<AddEnvelopeVibration>& msg)
{
    // check if index is -1, then apply to ALL players
    IndexT startIndex = msg->GetPlayerIndex();
    SizeT quantity = 1;
    if (msg->GetPlayerIndex() == InvalidIndex)
    {
        startIndex = 0;
        quantity = NEBULA3_MAX_LOCAL_PLAYERS;
    }

    IndexT index;
    for (index = startIndex; index < startIndex + quantity; index++)
    {    
        // set target values
        Vibrato& playerVibrato = this->targetVibratos[index];
        playerVibrato.highFreqAttack = msg->GetHighFreqAttack();
        playerVibrato.highFreqIntensity = msg->GetHighFreqAmount();
        playerVibrato.highFreqRelease = msg->GetHighFreqRelease();
        playerVibrato.highFreqSustain = msg->GetHighFreqSustain();
        playerVibrato.lowFreqAttack = msg->GetLowFreqAttack();
        playerVibrato.lowFreqIntensity = msg->GetLowFreqAmount();
        playerVibrato.lowFreqRelease = msg->GetLowFreqRelease();
        playerVibrato.lowFreqSustain = msg->GetLowFreqSustain();
        playerVibrato.startTime = this->timer.GetTime();

        // activate
        this->vibratosRunnung[index] = true;
    }
}

//------------------------------------------------------------------------------
/**
    Shutdown the audio runtime.
*/
void
VibrationHandler::OnClearVibration(const Ptr<ClearVibration>& msg)
{
    // check if index is -1, then apply to ALL players
    IndexT startIndex = msg->GetPlayerIndex();
    SizeT quantity = 1;
    if (msg->GetPlayerIndex() == InvalidIndex)
    {
        startIndex = 0;
        quantity = NEBULA3_MAX_LOCAL_PLAYERS;
    }

    IndexT index;
    for (index = startIndex; index < quantity; index++)
    {    
        // set target values
        Vibrato& playerVibrato = this->targetVibratos[index];
        playerVibrato.highFreqAttack = 0;
        playerVibrato.highFreqIntensity = 0;
        playerVibrato.highFreqRelease = msg->GetReleaseTime();
        playerVibrato.highFreqSustain = 0;
        playerVibrato.lowFreqAttack = 0;
        playerVibrato.lowFreqIntensity = 0;
        playerVibrato.lowFreqRelease = msg->GetReleaseTime();
        playerVibrato.lowFreqSustain = 0;
        playerVibrato.startTime = this->timer.GetTime();

        // check if stop immediate
        if (msg->GetReleaseTime() == 0)
        {
            this->curHiFreqValues[index] = 0;
            this->curLowFreqValues[index] = 0;
            this->vibratosRunnung[index] = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
VibrationHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(AddEnvelopeVibration::Id))
    {
        this->OnAddEnvelopeVibration(msg.cast<AddEnvelopeVibration>());
    }
    else if (msg->CheckId(ClearVibration::Id))
    {
        this->OnClearVibration(msg.cast<ClearVibration>());
    }
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}
} // namespace Vibration
