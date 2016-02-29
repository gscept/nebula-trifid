#pragma once
//------------------------------------------------------------------------------
/**
	Implements a memory fence in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/bufferlockbase.h"
namespace Vulkan
{
class VkCpuSyncFence : public Base::BufferLockBase
{
	__DeclareClass(VkCpuSyncFence);
public:
	/// constructor
	VkCpuSyncFence();
	/// destructor
	virtual ~VkCpuSyncFence();
private:
};
} // namespace Vulkan