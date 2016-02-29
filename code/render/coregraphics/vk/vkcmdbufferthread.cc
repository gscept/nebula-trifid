//------------------------------------------------------------------------------
// vkcmdbufferthread.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkcmdbufferthread.h"
#include "threading/event.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkCmdBufferThread, 'VCBT', Threading::Thread);
//------------------------------------------------------------------------------
/**
*/
VkCmdBufferThread::VkCmdBufferThread()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkCmdBufferThread::~VkCmdBufferThread()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdBufferThread::EmitWakeupSignal()
{
	this->commands.Signal();
}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdBufferThread::DoWork()
{
	Util::Array<Command> curCommands;
	while (!this->ThreadStopRequested())
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
			case GraphicsPipeline:
				vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cmd.pipeline);
				break;
			case ComputePipeline:
				vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmd.pipeline);
				break;
			case InputAssemblyVertex:
				vkCmdBindIndexBuffer(this->commandBuffer, cmd.ibo.buffer, cmd.ibo.offset, cmd.ibo.indexType);
				break;
			case InputAssemblyIndex:
				vkCmdBindVertexBuffers(this->commandBuffer, cmd.vbo.index, 1, &cmd.vbo.buffer, &cmd.vbo.offset);
				break;
			case Draw:
				if (cmd.draw.numIndices > 0) vkCmdDraw(this->commandBuffer, cmd.draw.numVerts, cmd.draw.numInstances, cmd.draw.baseVertex, cmd.draw.baseInstance);
				else						 vkCmdDrawIndexed(this->commandBuffer, cmd.draw.numIndices, cmd.draw.numInstances, cmd.draw.baseIndex, cmd.draw.baseVertex, cmd.draw.baseInstance);
				break;
			case Dispatch:
				vkCmdDispatch(this->commandBuffer, cmd.dispatch.numGroupsX, cmd.dispatch.numGroupsY, cmd.dispatch.numGroupsZ);
				break;
			case BindDescriptors:
				vkCmdBindDescriptorSets(this->commandBuffer, cmd.descriptor.type, cmd.descriptor.layout, cmd.descriptor.baseSet, cmd.descriptor.numSets, cmd.descriptor.sets, cmd.descriptor.numOffsets, cmd.descriptor.offsets);
				break;
			case PushRange:
				vkCmdPushConstants(this->commandBuffer, cmd.pushranges.layout, cmd.pushranges.stages, cmd.pushranges.offset, cmd.pushranges.size, cmd.pushranges.data);
				break;
			case Sync:
				cmd.syncEvent->Signal();
				break;
			}
		}

		this->commands.Wait();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdBufferThread::PushCommand(const Command& command)
{
	this->commands.Enqueue(command);
}

} // namespace Vulkan