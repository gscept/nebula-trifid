#pragma once
//------------------------------------------------------------------------------
/**
	@class VR::VRManager

	(C) 2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"

namespace vr
{
	class IVRSystem;
}

//------------------------------------------------------------------------------
namespace VR
{
enum TrackerType
{
	InvalidTracker = -1,
	HMDisplay = 0,
	LeftControl = 1,
	RightControl = 2,

	TRACKER_COUNT
};

class ViveMote;

class VRManager : public Game::Manager
{
	__DeclareClass(VRManager);
	__DeclareSingleton(VRManager);
public:

	/// constructor
	VRManager();
	/// destructor
	virtual ~VRManager();

	/// initializes the hardware
	virtual void OnActivate();
	/// 
	virtual void OnDeactivate();
	/// 
	virtual void OnBeginFrame();
	///
	vr::IVRSystem* GetHMD() const;
	/// get internal tracked id
	const uint32_t GetTrackerId(VR::TrackerType tr) const;

	const Math::matrix44 & GetTrackedObject(VR::TrackerType t) const;
	///
	const bool HasHMD() const;
private:

	/// read tracking data
	void RetrieveTrackers();

	vr::IVRSystem * HMD;			
	Util::FixedArray<Math::matrix44> trackedObjects;
	uint32_t trackerIds[VR::TrackerType::TRACKER_COUNT];
	Ptr<VR::ViveMote> mites[2];
};

//------------------------------------------------------------------------------
/**
*/
inline 
const bool
VRManager::HasHMD() const
{
	return this->HMD != NULL;
}

//------------------------------------------------------------------------------
/**
*/
inline
const uint32_t
VRManager::GetTrackerId(VR::TrackerType tr) const
{
	return this->trackerIds[tr];
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44 &
VRManager::GetTrackedObject(VR::TrackerType t) const
{
	return this->trackedObjects[this->trackerIds[t]];
}
//------------------------------------------------------------------------------
/**
*/
inline
vr::IVRSystem*
VRManager::GetHMD() const
{
	return this->HMD;
}
} // namespace VR
