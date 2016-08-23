//------------------------------------------------------------------------------
// vkuniformbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkuniformbuffer.h"
#include "vkrenderdevice.h"
#include "coregraphics/constantbuffer.h"
#include "coregraphics/config.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkUniformBuffer, 'VKUB', Base::ConstantBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkUniformBuffer::VkUniformBuffer() :
	binding(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkUniformBuffer::~VkUniformBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::Setup(const SizeT numBackingBuffers)
{
	ConstantBufferBase::Setup(numBackingBuffers);
	uint32_t queues[] = { VkRenderDevice::Instance()->drawQueueFamily, VkRenderDevice::Instance()->computeQueueFamily, VkRenderDevice::Instance()->transferQueueFamily };
	this->createInfo =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		0,
		this->size * numBackingBuffers,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_SHARING_MODE_CONCURRENT,
		3,
		queues
	};
	VkResult res = vkCreateBuffer(VkRenderDevice::dev, &this->createInfo, NULL, &this->buf);
	n_assert(res == VK_SUCCESS);

	uint32_t alignedSize;
	VkRenderDevice::Instance()->AllocateBufferMemory(this->buf, this->mem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);
	
	// bind to buffer
	res = vkBindBufferMemory(VkRenderDevice::dev, this->buf, this->mem, 0);
	n_assert(res == VK_SUCCESS);

	// size and stride for a single buffer are equal
	this->size = alignedSize;
	this->stride = alignedSize / numBackingBuffers;

	// map memory so we can use it later
	res = vkMapMemory(VkRenderDevice::dev, this->mem, 0, this->size, 0, &this->buffer);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::SetupFromBlockInShader(const Ptr<CoreGraphics::ShaderState>& shaderState, const Util::String& blockName, const SizeT numBackingBuffers)
{
	ConstantBufferBase::SetupFromBlockInShader(shaderState, blockName, numBackingBuffers);
	const Ptr<CoreGraphics::Shader>& shader = shaderState->GetShader();
	AnyFX::VkVarblock* varblock = static_cast<AnyFX::VkVarblock*>(shader->GetVkEffect()->GetVarblock(blockName.AsCharPtr()));

	// setup buffer from other buffer
	this->binding = varblock->binding;
	this->size = varblock->byteSize;
	this->Setup(numBackingBuffers);

	// move free indices to used indices
	IndexT i;
	for (i = 0; i < this->freeIndices.Size(); i++)
	{
		this->usedIndices.Append(i);
	}
	this->freeIndices.Clear();

	// begin synced update, this will cause the uniform buffer to be setup straight from the start
	this->BeginUpdateSync();
	for (uint i = 0; i < varblock->variables.size(); i++)
	{
		AnyFX::VkVariable* var = static_cast<AnyFX::VkVariable*>(varblock->variables[i]);
		uint32_t offset = varblock->offsetsByName[var->name];

		Ptr<CoreGraphics::ShaderVariable> svar = CoreGraphics::ShaderVariable::Create();
		Ptr<VkUniformBuffer> thisPtr(this);
		svar->Setup(var, shaderState.downcast<VkShaderState>(), VK_NULL_HANDLE);
		svar->BindToUniformBuffer(thisPtr.downcast<CoreGraphics::ConstantBuffer>(), offset, var->byteSize, (int8_t*)var->currentValue);
		this->variables.Append(svar);
		this->variablesByName.Add(var->name.c_str(), svar);
	}
	this->EndUpdateSync();
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::Discard()
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
	vkDestroyBuffer(VkRenderDevice::dev, this->buf, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::UpdateAsync(void* data, uint offset, uint size)
{
	n_assert(size + offset <= this->size);
	byte* buf = (byte*)this->buffer + offset + this->baseOffset;
	memcpy(buf, data, size);
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::UpdateArrayAsync(void* data, uint offset, uint size, uint count)
{
	n_assert(size + offset <= this->size);
	byte* buf = (byte*)this->buffer + offset + this->baseOffset;
	memcpy(buf, data, size * count);
}

#define ROUND_TO_POW(n, p) ((n + p - 1) & ~(p - 1))
//------------------------------------------------------------------------------
/**
	Performs an expansion of the uniform buffer, based on how many instances we want to allocate.
	If the buffer is full it is expanded to a new size.
	If the buffer has free instances we reuse them by returning the offset into that buffer.
	If a continuous range of numInstances fits in the free list, the offset is the offset to the first element.
	If there is no continuous range fitting numInstances, alloc and return offset without disturbing other free instances.
*/
SizeT
VkUniformBuffer::AllocateInstance(SizeT numInstances)
{
	ConstantBufferBase::AllocateInstance(numInstances);
	uint32_t alignment = VK_DEVICE_SIZE_CONV(VkRenderDevice::Instance()->deviceProps.limits.minUniformBufferOffsetAlignment);
	SizeT offset;

	// pool is exhausted, allocate new elements
	if (this->freeIndices.IsEmpty())
	{
		// calculate new capacity by number of instances, or grow number if bigger
		SizeT capacity = this->usedIndices.Size();

		// calculate current offset
		offset = ROUND_TO_POW(capacity * this->stride, alignment);
		uint32_t alignedSize = this->Grow(capacity, numInstances);		

		// add instance to used indices
		IndexT i;
		for (i = 0; i < numInstances; i++)
		{
			this->usedIndices.Append(capacity + i);
		}
	}
	else
	{
		// if pool is not exhausted, return index
		if (numInstances == 1)
		{
			// grab index from free indices list
			IndexT index = this->freeIndices.Front();
			this->freeIndices.EraseIndex(0);

			// return offset
			offset = ROUND_TO_POW(index * this->stride, alignment);
			this->usedIndices.Append(index);
		}
		else if (this->freeIndices.Size() > numInstances) // pool could possibly fit range, find range
		{
			// try to find range of instances
			SizeT validRangeCount = 1;
			IndexT freeIndex = this->freeIndices.Front();
			IndexT i;
			for (i = 1; i < this->freeIndices.Size(); i++)
			{
				IndexT idx = this->freeIndices[i];
				if (idx-1 == freeIndex) validRangeCount++;
				else
				{
					validRangeCount = 1;
				}
				

				// if a valid range is found, find the offset and return
				if (validRangeCount == numInstances)
				{
					// find offset to first index
					offset = ROUND_TO_POW((idx - numInstances) * this->stride, alignment);
					break;
				}
				freeIndex = idx;
			}
			
			// if no range was found, alloc
			if (validRangeCount != numInstances)
			{
				SizeT capacity = this->usedIndices.Size() + this->freeIndices.Size();

				// calculate offset
				offset = ROUND_TO_POW(capacity * this->stride, alignment);
				uint32_t alignedSize = this->Grow(capacity, numInstances);				

				// add instance to used indices
				IndexT i;
				for (i = 0; i < numInstances; i++)
				{
					this->usedIndices.Append(capacity + i);
				}
			}
		}
		else // fitting is impossible, but pool is not exhausted, so grow!
		{
			SizeT capacity = this->usedIndices.Size() + this->freeIndices.Size();

			// calculate offset
			offset = ROUND_TO_POW(capacity * this->stride, alignment);
			uint32_t alignedSize = this->Grow(capacity, numInstances);

			// add instance to used indices
			IndexT i;
			for (i = 0; i < numInstances; i++)
			{
				this->usedIndices.Append(capacity + i);
			}
		}
	}

	return offset;
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::FreeInstance(SizeT offset)
{
	ConstantBufferBase::FreeInstance(offset);
	IndexT instance = offset / this->stride;
	IndexT idx = this->usedIndices.FindIndex(instance);
	n_assert(idx != InvalidIndex);
	this->usedIndices.EraseIndex(idx);
	this->freeIndices.Append(instance);
}

//------------------------------------------------------------------------------
/**
*/
uint32_t
VkUniformBuffer::Grow(SizeT oldCapacity, SizeT growBy)
{
	// new capacity is the old one, plus the number of elements we wish to allocate, although never allocate fewer than grow
	//SizeT newCapacity = ROUND_TO_POW(oldCapacity + growBy, this->grow);
	SizeT increment = oldCapacity >> 1;
	increment = Math::n_iclamp(increment, this->grow, 65535);
	n_assert(increment >= growBy);
	SizeT newCapacity = oldCapacity + increment;	

	// create new buffer
	uint32_t queues[] = { VkRenderDevice::Instance()->drawQueueFamily, VkRenderDevice::Instance()->computeQueueFamily, VkRenderDevice::Instance()->transferQueueFamily };
	this->createInfo.pQueueFamilyIndices = queues;
	this->createInfo.size = newCapacity * this->stride;

	VkBuffer newBuf;
	VkResult res = vkCreateBuffer(VkRenderDevice::dev, &this->createInfo, NULL, &newBuf);
	n_assert(res == VK_SUCCESS);

	// allocate new instance memory, alignedSize is the aligned size of a single buffer
	VkDeviceMemory newMem;
	uint32_t alignedSize;
	VkRenderDevice::Instance()->AllocateBufferMemory(newBuf, newMem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);

	// bind to buffer, this is the reason why we must destroy and create the buffer again
	res = vkBindBufferMemory(VkRenderDevice::dev, newBuf, newMem, 0);
	n_assert(res == VK_SUCCESS);

	// copy old to new, old memory is already mapped
	void* dstData;

	// map new memory with new capacity, avoids a second map
	res = vkMapMemory(VkRenderDevice::dev, newMem, 0, alignedSize, 0, &dstData);
	n_assert(res == VK_SUCCESS);
	memcpy(dstData, this->buffer, this->size);
	vkUnmapMemory(VkRenderDevice::dev, this->mem);

	// clean up old data	
	vkDestroyBuffer(VkRenderDevice::dev, this->buf, NULL);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);

	// add free indices
	IndexT i;
	for (i = 0; i < (newCapacity - oldCapacity - growBy); i++)
	{
		this->freeIndices.Append(oldCapacity + growBy + i);
	}

	// replace old device memory and size
	this->size = alignedSize;
	this->buf = newBuf;
	this->mem = newMem;
	this->buffer = dstData;
	return alignedSize;
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::Reset()
{
	IndexT i;
	for (i = 0; i < this->usedIndices.Size(); i++)
	{
		this->freeIndices.Append(this->usedIndices[i]);
	}
	this->usedIndices.Clear();
}

} // namespace Vulkan