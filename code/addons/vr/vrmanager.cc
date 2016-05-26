//------------------------------------------------------------------------------
//  vr/vrmanager.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "openvr.h"
#include "vrmanager.h"
#include "game/manager.h"
#include "core/debug.h"
#include "vrcameraproperty.h"
#include "vivemote.h"
#include "input/inputserver.h"

namespace VR
{
	//using namespace Graphics;
	using namespace Math;
	//using namespace Input;
	using namespace Game;
	using namespace vr;

__ImplementClass(VR::VRManager, 'VRMA', Game::Manager);
__ImplementSingleton(VR::VRManager);



//------------------------------------------------------------------------------
/**
*/
VRManager::VRManager():
	HMD(NULL)
{
	__ConstructSingleton
}

//------------------------------------------------------------------------------
/**
*/
VRManager::~VRManager()
{
	__DestructSingleton
}

//------------------------------------------------------------------------------
/**
*/
void
VRManager::OnActivate()
{
	n_assert(HMD == NULL);
	Manager::OnActivate();

	// Loading the SteamVR Runtime
	vr::EVRInitError vrerror = vr::VRInitError_None;
	this->HMD = vr::VR_Init(&vrerror, vr::VRApplication_Scene);
	if (vrerror != vr::VRInitError_None)
	{
		n_error("Failed to initialize OpenVR: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(vrerror));
	}
	this->trackedObjects.SetSize(vr::k_unMaxTrackedDeviceCount);

	this->RetrieveTrackers();
	this->mites[0] = ViveMote::Create();
	this->mites[0]->SetTracker(LeftControl);
	Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::Game, this->mites[0].cast<Input::InputHandler>());
	this->mites[1] = ViveMote::Create();
	this->mites[1]->SetTracker(RightControl);
	Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::Game, this->mites[1].cast<Input::InputHandler>());	
}

//------------------------------------------------------------------------------
/**
*/
void
VRManager::OnDeactivate()
{
	Manager::OnDeactivate();
	vr::VR_Shutdown();
	this->HMD = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
VRManager::OnBeginFrame()
{
	this->RetrieveTrackers();
}

void VRManager::RetrieveTrackers()
{
	if (this->HMD)
	{
		static vr::TrackedDevicePose_t tracked[vr::k_unMaxTrackedDeviceCount];
		static uint32_t trackerIds[vr::k_unMaxTrackedDeviceCount];
		vr::EVRCompositorError ret = vr::VRCompositor()->WaitGetPoses(NULL, 0, tracked, vr::k_unMaxTrackedDeviceCount);
		n_assert(ret == EVRCompositorError::VRCompositorError_None);

		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
		{
			if (tracked[i].bPoseIsValid)
			{
				for (int j = 0; j < 3; j++)
				{
					this->trackedObjects[i].row(j).loadu(tracked[i].mDeviceToAbsoluteTracking.m[j]);
				}
			}
		}
		// FIXME these should only be updated if a connect/disconnect event occured
		static uint32_t ids[3];
		this->HMD->GetSortedTrackedDeviceIndicesOfClass(TrackedDeviceClass_HMD, ids, 3, -1);
		this->trackerIds[HMDisplay] = ids[0];
		this->trackerIds[LeftControl] = this->HMD->GetTrackedDeviceIndexForControllerRole(TrackedControllerRole_LeftHand);
		this->trackerIds[RightControl] = this->HMD->GetTrackedDeviceIndexForControllerRole(TrackedControllerRole_RightHand);
	}

}

}