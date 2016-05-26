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
	lastState(0)
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
#if _DEBUG
	if (this->buttonStates[btn].pressed)
	{
		n_printf("button pressed: %s\n", Base::GamePadBase::ButtonAsString(btn).AsCharPtr());
	}
#endif
}
} // namespace VR
