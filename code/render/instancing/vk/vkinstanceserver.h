#pragma once
//------------------------------------------------------------------------------
/**
	Implements the entry point to the instance rendering subsystem, for Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "instancing/base/instanceserverbase.h"
namespace Vulkan
{
class VkInstanceServer : public Base::InstanceServerBase
{
	__DeclareClass(VkInstanceServer);
public:
	/// constructor
	VkInstanceServer();
	/// destructor
	virtual ~VkInstanceServer();
private:
};
} // namespace Vulkan