#pragma once
//------------------------------------------------------------------------------
/**
    @class VR::ViveMote

    An input handler which represents htc vive controllers (aka donut sticks)
    
    (C) 2016 Individual contributors, see AUTHORS file
*/

#include "vrmanager.h"
#include "input/gamepad.h"

//------------------------------------------------------------------------------
namespace VR
{
class ViveMote : public Input::GamePad
{
__DeclareClass(ViveMote);
public:
    /// constructor
	ViveMote();
    /// destructor
	virtual ~ViveMote();

	/// called on InputServer::BeginFrame()
	virtual void OnBeginFrame();
	///
        
protected:   	
	friend class VR::VRManager;
    /// called when the handler is attached to the input server
    virtual void OnAttach();
    /// reset the input handler
    virtual void OnReset();

	/// set the tracked object identity
	void SetTracker(VR::TrackerType tr);
	///
	void UpdateButtonState(vr::EVRButtonId xiBtn, GamePadBase::Button btn);
	VR::TrackerType trackedObject;
	vr::VRControllerState_t * state;
	uint32_t lastState;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
ViveMote::SetTracker(VR::TrackerType tr)
{
	this->trackedObject = tr;
}

} // namespace VR
//------------------------------------------------------------------------------

