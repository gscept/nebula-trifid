#pragma once
//------------------------------------------------------------------------------
/**
	Implements a command buffer thread which incrementally writes to Vulkan command buffers.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "threading/thread.h"
#include "threading/safequeue.h"
#include <vulkan/vulkan.h>
#include "coregraphics/primitivegroup.h"
namespace Vulkan
{
class VkCmdBufferThread : public Threading::Thread
{
	__DeclareClass(VkCmdBufferThread);
	struct Command;
public:
	/// constructor
	VkCmdBufferThread();
	/// destructor
	virtual ~VkCmdBufferThread();

	/// called if thread needs a wakeup call before stopping
	void EmitWakeupSignal();
	/// this method runs in the thread context
	void DoWork();
	/// push command buffer work
	void PushCommand(const Command& command);
	/// set command buffer
	void SetCommandBuffer(const VkCommandBuffer& buffer);
private:
	friend class VkRenderDevice;

	enum CommandType
	{
		GraphicsPipeline,
		ComputePipeline,
		InputAssemblyVertex,
		InputAssemblyIndex,
		Draw,
		Dispatch,
		BindDescriptors,
		PushRange,
		UpdateBuffer,
		Sync
	};

	struct Command
	{
		CommandType type;

		union
		{
			VkPipeline pipeline;

			struct VBO
			{
				VkBuffer buffer;
				IndexT index;
				VkDeviceSize offset;
			} vbo;

			struct IBO
			{
				VkBuffer buffer;
				VkDeviceSize offset;
				VkIndexType indexType;
			} ibo;			
			
			struct Draw
			{
				uint32_t baseIndex;
				uint32_t baseVertex;
				uint32_t numIndices;
				uint32_t numVerts;
				uint32_t baseInstance;
				uint32_t numInstances;
			} draw;

			struct Dispatch
			{
				uint32_t numGroupsX;
				uint32_t numGroupsY;
				uint32_t numGroupsZ;
			} dispatch;

			struct Descriptors
			{
				VkPipelineBindPoint type;
				VkPipelineLayout layout;
				uint32_t baseSet;
				uint32_t numSets;
				VkDescriptorSet* sets;
				uint32_t numOffsets;
				uint32_t* offsets;
			} descriptor;

			struct UpdateBuffer
			{
				bool deleteWhenDone;
				VkBuffer buf;
				VkDeviceSize offset;
				VkDeviceSize size;
				uint32_t* data;
			} updBuffer;

			struct PushConstants
			{
				VkShaderStageFlags stages;
				VkPipelineLayout layout;
				uint32_t offset;
				uint32_t size;
				void* data;
			} pushranges;

			Threading::Event* syncEvent;
		};		
	};

	VkCommandBuffer commandBuffer;
	Threading::SafeQueue<Command> commands;
};


//------------------------------------------------------------------------------
/**
*/
inline void
VkCmdBufferThread::SetCommandBuffer(const VkCommandBuffer& buffer)
{
	this->commandBuffer = buffer;
}

} // namespace Vulkan