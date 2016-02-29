//------------------------------------------------------------------------------
// vkfence.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkcpusyncfence.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkCpuSyncFence, 'VKFE', Base::BufferLockBase);
//------------------------------------------------------------------------------
/**
*/
VkCpuSyncFence::VkCpuSyncFence()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkCpuSyncFence::~VkCpuSyncFence()
{
	// empty
}

} // namespace Vulkan