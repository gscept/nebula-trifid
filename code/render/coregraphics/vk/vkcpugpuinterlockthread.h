#pragma once
//------------------------------------------------------------------------------
/**
	VkCpuGpuInterlockThread is a single thread which runs in parallel to the rendering thread,
	it is used to wait for events on the GPU queue, using a provided VkEvent, and perform a job
	in lock step with the GPU.

	The idea is to do something like this:

	MAIN --- Draw SignalEvent Wait Draw Draw Draw Draw Draw Draw Draw SignalEvent Wait
	THREAD ------ Wait Work - SignalEvent --------------------------- Wait Work - SignalEvent

	In theory we can do this on any queue, the only thing that matters is where the VkEvent is inserted.

	Example:
		GPU signals event when work on dependent data is done, waits for the same event after.
		Thread waits for GPU to signal event, performs work, and unlocks the GPU by setting the event.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <vulkan/vulkan.h>
#include "threading/thread.h"
#include "threading/safequeue.h"
namespace Vulkan
{
class VkCpuGpuInterlockThread : public Threading::Thread
{
	__DeclareClass(VkCpuGpuInterlockThread);
	struct Command;
public:
	/// constructor
	VkCpuGpuInterlockThread();
	/// destructor
	virtual ~VkCpuGpuInterlockThread();

	/// called if thread needs a wakeup call before stopping
	void EmitWakeupSignal();
	/// this method runs in the thread context
	void DoWork();
	/// push command buffer work
	void PushCommand(const Command& command);
	/// push command buffer work
	void PushCommands(const Util::Array<Command>& commands);

private:
	friend class VkRenderDevice;
	enum CommandType
	{
		Memcpy,
		WaitEvent,
		ResetEvent,
		SignalEvent,
		SwitchEvent
	};
	struct Command
	{
		CommandType type;
		VkDevice dev;
		union
		{
			struct // Memcpy
			{
				void* mappedData;
				const void* data;
				uint32_t size;
				uint32_t offset;
			} memCpy;

			struct // SwtichEvent
			{
				VkEvent event;
			} switchEvent;
		};
	};

	VkDevice dev;
	VkEvent event;
	Threading::SafeQueue<Command> commands;
};

//------------------------------------------------------------------------------
/**
*/
inline void
VkCpuGpuInterlockThread::PushCommand(const Command& command)
{
	this->commands.Enqueue(command);
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkCpuGpuInterlockThread::PushCommands(const Util::Array<Command>& commands)
{
	this->commands.EnqueueArray(commands);
}

} // namespace Vulkan
