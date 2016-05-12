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
	virtual void OnFrame();
	///
	const vr::IVRSystem* GetHMD() const;

	///
	const bool HasHMD() const;
private:
	vr::IVRSystem * HMD;	
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
const vr::IVRSystem*
VRManager::GetHMD() const
{
	return this->HMD;
}
} // namespace VR
