//------------------------------------------------------------------------------
//  vivemote.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "openvr.h"
#include "vivemote.h"


namespace VR
{
__ImplementClass(VR::ViveMote, 'VRVM', Input::GamePad);

using namespace Input;
using namespace Util;
using namespace vr;

//------------------------------------------------------------------------------
/**
*/
ViveMote::ViveMote() :
	trackedObject(InvalidTracker),
	lastState(0),
    enabled(true)
{
	this->state = n_new(vr::VRControllerState_t);
}

//------------------------------------------------------------------------------
/**
*/
ViveMote::~ViveMote()
{
	n_delete(this->state);
	this->state = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
ViveMote::OnAttach()
{
    InputHandler::OnAttach();	
   
}

//------------------------------------------------------------------------------
/**
*/
void
ViveMote::OnReset()
{
    InputHandler::OnReset();

    // reset button states (should behave as if the player
    // suddenly released all buttons and triggers)
    IndexT i;
    for (i = 0; i < this->buttonStates.Size(); i++)
    {
        ButtonState& btnState = this->buttonStates[i];
        if (btnState.pressed)
        {
            btnState.up = true;
        }
        else
        {
            btnState.up = false;
        }
        btnState.down = false;
        btnState.pressed = false;
    }
    this->axisValues.Fill(0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void ViveMote::OnBeginFrame()
{
    if (!this->enabled)
    {
        return;
    }
	if (this->trackedObject == InvalidTracker && !VRManager::Instance()->HasHMD())
	{
		return;
	}
	int32_t idx = VRManager::Instance()->GetTrackerId(this->trackedObject);
	
	if (idx != vr::k_unTrackedDeviceIndexInvalid)
	{
		this->transform = VRManager::Instance()->GetTrackedObject(this->trackedObject);

		vr::IVRSystem* hmd = VRManager::Instance()->GetHMD();
		if (hmd->GetControllerState(idx, this->state))
		{
			if (this->lastState != this->state->unPacketNum)
			{
				this->lastState = this->state->unPacketNum;				
				this->UpdateButtonState(vr::k_EButton_ApplicationMenu, StartButton);
				this->UpdateButtonState(vr::k_EButton_Grip, BackButton);
				this->UpdateButtonState(vr::k_EButton_SteamVR_Trigger, RightShoulderButton);
				this->UpdateButtonState(vr::k_EButton_SteamVR_Touchpad, RightThumbButton);	
				//FIXME hardcoded
				this->axisValues[RightTriggerAxis] = this->state->rAxis[1].x;
				this->axisValues[RightThumbXAxis] = this->state->rAxis[0].x;
				this->axisValues[RightThumbYAxis] = this->state->rAxis[0].y;                
			}
            else
            {
                // reset button up state                
                this->buttonStates[StartButton].up = false;
                this->buttonStates[StartButton].down = false;
                this->buttonStates[BackButton].up = false;
                this->buttonStates[BackButton].down = false;
                this->buttonStates[RightShoulderButton].up = false;
                this->buttonStates[RightShoulderButton].down = false;
                this->buttonStates[RightThumbButton].up = false;
                this->buttonStates[RightThumbButton].down = false;
            }
		}
	}
}

//------------------------------------------------------------------------------
/**
Compares the previous and current state of a button
and updates the parent class' state accordingly.
*/
void
ViveMote::UpdateButtonState(vr::EVRButtonId xiBtn, GamePadBase::Button btn)
{
	if (0 != ((this->state->ulButtonPressed) & ButtonMaskFromId( xiBtn)))
	{
		// has button been down-pressed in this frame?
		if (!this->buttonStates[btn].pressed)
		{
			this->buttonStates[btn].down = true;
		}
		else
		{
			this->buttonStates[btn].down = false;
		}
		this->buttonStates[btn].pressed = true;
		this->buttonStates[btn].up = false;
	}
	else
	{
		// has button been released in this frame?
		if (this->buttonStates[btn].pressed)
		{
			this->buttonStates[btn].up = true;
		}
		else
		{
			this->buttonStates[btn].up = false;
		}
		this->buttonStates[btn].pressed = false;
		this->buttonStates[btn].down = false;
	}
#if 0
    if (this->buttonStates[btn].down)
	{
		n_printf("button down: %s\n", Base::GamePadBase::ButtonAsString(btn).AsCharPtr());
	}
    if (this->buttonStates[btn].up)
    {
        n_printf("button up: %s\n", Base::GamePadBase::ButtonAsString(btn).AsCharPtr());
    }
#endif
}

Util::String ViveMote::GetStateString()
{
    Util::String stateString;
    stateString.Format("%f:%f:%f:%d:%d:%d:%d", this->GetAxisValue(Base::GamePadBase::RightThumbXAxis),
        this->GetAxisValue(Base::GamePadBase::RightThumbYAxis),
        this->GetAxisValue(Base::GamePadBase::RightTriggerAxis),
        (this->ButtonPressed(Base::GamePadBase::StartButton) ? 0x4 : 0) + (this->ButtonDown(Base::GamePadBase::StartButton) ? 0x1 : 0) + (this->ButtonUp(Base::GamePadBase::StartButton) ? 0x2 : 0),
        (this->ButtonPressed(Base::GamePadBase::BackButton) ? 0x4 : 0) + (this->ButtonDown(Base::GamePadBase::BackButton) ? 0x1 : 0) + (this->ButtonUp(Base::GamePadBase::BackButton) ? 0x2 : 0),
        (this->ButtonPressed(Base::GamePadBase::RightShoulderButton) ? 0x4 : 0) + (this->ButtonDown(Base::GamePadBase::RightShoulderButton) ? 0x1 : 0) + (this->ButtonUp(Base::GamePadBase::RightShoulderButton) ? 0x2 : 0),
        (this->ButtonPressed(Base::GamePadBase::RightThumbButton) ? 0x4 : 0) + (this->ButtonDown(Base::GamePadBase::RightThumbButton) ? 0x1 : 0) + (this->ButtonUp(Base::GamePadBase::RightThumbButton) ? 0x2 : 0)
        );
    return stateString;
}

void ViveMote::SetStateFromString(const Util::String & state)
{    
    Array<String> toks = state.Tokenize(":");
    if (toks.Size() != 7)
    {
        return;
    }
    this->axisValues[RightThumbXAxis] = toks[0].AsFloat();
    this->axisValues[RightThumbYAxis] = toks[1].AsFloat();
    this->axisValues[RightTriggerAxis] = toks[2].AsFloat();
    int button;
    button = toks[3].AsInt();
    this->buttonStates[StartButton].down = (button & 0x1)>0;
    this->buttonStates[StartButton].pressed = (button & 0x4) >0;
    this->buttonStates[StartButton].up = (button & 0x2) >0;

    button = toks[4].AsInt();
    this->buttonStates[BackButton].down = (button & 0x1) > 0;
    this->buttonStates[BackButton].pressed = (button & 0x4) > 0;
    this->buttonStates[BackButton].up = (button & 0x2) > 0;

    button = toks[5].AsInt();
    this->buttonStates[RightShoulderButton].down = (button & 0x1) > 0;
    this->buttonStates[RightShoulderButton].pressed = (button & 0x4) > 0;
    this->buttonStates[RightShoulderButton].up = (button & 0x2) > 0;

    button = toks[6].AsInt();
    this->buttonStates[RightThumbButton].down = (button & 0x1) > 0;
    this->buttonStates[RightThumbButton].pressed = (button & 0x4) > 0;
    this->buttonStates[RightThumbButton].up = (button & 0x2) > 0;
}

} // namespace VR
