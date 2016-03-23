#pragma once
//------------------------------------------------------------------------------
/**
	Implements a deferred delegate, which is used to perform an action whenever a fence object is done.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/fixedarray.h"
#include <vulkan/vulkan.h>
namespace Vulkan
{
struct VkDeferredDelegate
{
	/// constructor
	VkDeferredDelegate();
	/// destructor
	~VkDeferredDelegate();

	/// assignment operator from other delegate
	void operator=(const VkDeferredDelegate& rhs);

	enum DelegateType
	{
		FreeCmdBuffers,
		FreeMemory
	};

	struct Delegate
	{
		DelegateType type;
		union
		{
			struct CmdBufferFree
			{
				VkCommandBuffer buffers[64];
				VkCommandPool pool;
				uint32_t numBuffers;
			} cmdbufferfree;

			struct MemoryFree
			{
				void* data;
			} memory;
		};
	} del;

	VkFence fence;
	VkDevice dev;
	
	/// check delegate condition and execute if the fence is done, returns true if the delegate runs
	bool CheckAsync();
	/// check delegate condition by locking, will stop the CPU thread
	void CheckSync();
	/// reset the fence object, keeps the rest of the state intact
	void Reset();
	/// get status of delegate
	const VkResult GetStatus() const;

private:
	/// run delegate action
	void RunDelegate();
	VkResult currentStatus;
	
};

//------------------------------------------------------------------------------
/**
*/
inline const VkResult
VkDeferredDelegate::GetStatus() const
{
	return this->currentStatus;
}

} // namespace Vulkan