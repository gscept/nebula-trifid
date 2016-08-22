//------------------------------------------------------------------------------
// vkcpugpuinterlockthread.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkcpugpuinterlockthread.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkCpuGpuInterlockThread, 'VCPI', Threading::Thread);
//------------------------------------------------------------------------------
/**
*/
VkCpuGpuInterlockThread::VkCpuGpuInterlockThread() :
	event(VK_NULL_HANDLE)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkCpuGpuInterlockThread::~VkCpuGpuInterlockThread()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkCpuGpuInterlockThread::EmitWakeupSignal()
{
	this->commands.Signal();
}

//------------------------------------------------------------------------------
/**
*/
void
VkCpuGpuInterlockThread::DoWork()
{
	// create event
	VkEventCreateInfo info =
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		NULL,
		0
	};
	vkCreateEvent(this->dev, &info, NULL, &this->event);

	Util::Array<Command> curCommands;
	curCommands.Reserve(1000);
	while (true)
	{
		// dequeue all commands, this ensures we don't gain any new commands this thread loop
		this->commands.DequeueAll(curCommands);

		IndexT i;
		for (i = 0; i < curCommands.Size(); i++)
		{
			const Command& cmd = curCommands[i];

			// use the data in the command dependent on what type we have
			switch (cmd.type)
			{
			case Memcpy:
				n_assert(cmd.memCpy.mappedData != 0);
				n_assert(cmd.memCpy.data != 0);
				n_assert(cmd.memCpy.size > 0);
				memcpy(cmd.memCpy.mappedData, (uint8_t*)cmd.memCpy.data + cmd.memCpy.offset, cmd.memCpy.size);
				break;
			case WaitEvent:
			{
				VkResult res;
				do
				{
					// wait for event to be signaled
					res = vkGetEventStatus(cmd.dev, this->event);
					this->YieldThread();
				}
				while (res != VK_EVENT_SET);				
				break;
			}
			case ResetEvent:
				vkResetEvent(cmd.dev, this->event);
				break;
			case SignalEvent:
				vkSetEvent(cmd.dev, this->event);
				break;
			}
		}

		// reset (don't delete) consumed commands and wait for new ones
		curCommands.Reset();
		this->commands.Wait();
	}

	// clean up event
	vkDestroyEvent(this->dev, this->event, NULL);
}


} // namespace Vulkan