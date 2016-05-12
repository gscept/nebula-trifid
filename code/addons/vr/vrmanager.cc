//------------------------------------------------------------------------------
//  vr/vrmanager.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "openvr.h"
#include "vrmanager.h"
#include "game/manager.h"

namespace VR
{
	//using namespace Graphics;
	using namespace Math;
	//using namespace Input;
	using namespace Game;

__ImplementClass(VR::VRManager, 'VRMA', Game::Manager);
__ImplementSingleton(VR::VRManager);



//------------------------------------------------------------------------------
/**
*/
VRManager::VRManager():
	HMD(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
VRManager::~VRManager()
{

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
VRManager::OnFrame()
{

}


}