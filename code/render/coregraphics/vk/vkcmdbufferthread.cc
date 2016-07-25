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
			case BeginCommand:
				this->commandBuffer = cmd.bgCmd.buf;
				n_assert(vkBeginCommandBuffer(this->commandBuffer, &cmd.bgCmd.info) == VK_SUCCESS);
				break;
			case EndCommand:
				n_assert(vkEndCommandBuffer(this->commandBuffer) == VK_SUCCESS);
				this->commandBuffer = VK_NULL_HANDLE;
				break;
			case GraphicsPipeline:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cmd.pipeline);
				break;
			case ComputePipeline:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, cmd.pipeline);
				break;
			case InputAssemblyVertex:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdBindVertexBuffers(this->commandBuffer, cmd.vbo.index, 1, &cmd.vbo.buffer, &cmd.vbo.offset);
				break;
			case InputAssemblyIndex:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdBindIndexBuffer(this->commandBuffer, cmd.ibo.buffer, cmd.ibo.offset, cmd.ibo.indexType);
				break;
			case Draw:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				if (cmd.draw.numIndices > 0)	vkCmdDrawIndexed(this->commandBuffer, cmd.draw.numIndices, cmd.draw.numInstances, cmd.draw.baseIndex, cmd.draw.baseVertex, cmd.draw.baseInstance);
				else							vkCmdDraw(this->commandBuffer, cmd.draw.numVerts, cmd.draw.numInstances, cmd.draw.baseVertex, cmd.draw.baseInstance);
				break;
			case Dispatch:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdDispatch(this->commandBuffer, cmd.dispatch.numGroupsX, cmd.dispatch.numGroupsY, cmd.dispatch.numGroupsZ);
				break;
			case BindDescriptors:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdBindDescriptorSets(this->commandBuffer, cmd.descriptor.type, cmd.descriptor.layout, cmd.descriptor.baseSet, cmd.descriptor.numSets, cmd.descriptor.sets, cmd.descriptor.numOffsets, cmd.descriptor.offsets);
				break;
			case PushRange:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdPushConstants(this->commandBuffer, cmd.pushranges.layout, cmd.pushranges.stages, cmd.pushranges.offset, cmd.pushranges.size, cmd.pushranges.data);
				break;
			case Viewport:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdSetViewport(this->commandBuffer, cmd.viewport.index, 1, &cmd.viewport.vp);
				break;
			case ScissorRect:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdSetScissor(this->commandBuffer, cmd.scissorRect.index, 1, &cmd.scissorRect.sc);
				break;
			case ScissorRectArray:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdSetScissor(this->commandBuffer, cmd.scissorRectArray.first, cmd.scissorRectArray.num, cmd.scissorRectArray.scs);
				break;
			case UpdateBuffer:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				vkCmdUpdateBuffer(this->commandBuffer, cmd.updBuffer.buf, cmd.updBuffer.offset, cmd.updBuffer.size, cmd.updBuffer.data);
				break;
			case Sync:
				cmd.syncEvent->Signal();
				break;
			case LunarGCircumventValidation:
				n_assert(this->commandBuffer != VK_NULL_HANDLE);
				const float blend[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				vkCmdSetBlendConstants(this->commandBuffer, blend);
				vkCmdSetDepthBias(this->commandBuffer, 1.0f, 0.0f, 1.0f);
				vkCmdSetDepthBounds(this->commandBuffer, 0, 1);
				vkCmdSetLineWidth(this->commandBuffer, 1.0f);
				vkCmdSetStencilCompareMask(this->commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0xFFFFFFFF);
				vkCmdSetStencilReference(this->commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0xFFFFFFFF);
				vkCmdSetStencilWriteMask(this->commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0xFFFFFFFF);
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