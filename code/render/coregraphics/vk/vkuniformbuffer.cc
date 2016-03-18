//------------------------------------------------------------------------------
// vkuniformbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkuniformbuffer.h"
#include "vkrenderdevice.h"
#include "coregraphics/constantbuffer.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkUniformBuffer, 'VKUB', Base::ConstantBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkUniformBuffer::VkUniformBuffer()
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

	uint32_t queues[] = { VkRenderDevice::Instance()->renderQueueFamily, VkRenderDevice::Instance()->computeQueueFamily, VkRenderDevice::Instance()->transferQueueFamily };
	VkBufferCreateInfo info =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		0,
		this->size,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_SHARING_MODE_CONCURRENT,
		3,
		queues
	};
	VkResult res = vkCreateBuffer(VkRenderDevice::dev, &info, NULL, &this->buf);
	n_assert(res == VK_SUCCESS);

	uint32_t alignedSize;
	VkRenderDevice::Instance()->AllocateBufferMemory(this->buf, this->mem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);

	// bind to buffer
	vkBindBufferMemory(VkRenderDevice::dev, this->buf, this->mem, 0);

	// map memory so we can use it later
	res = vkMapMemory(VkRenderDevice::dev, this->mem, 0, alignedSize, 0, &this->buffer);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkUniformBuffer::SetupFromBlockInShader(const Ptr<CoreGraphics::Shader>& shader, const Util::String& blockName, const SizeT numBackingBuffers)
{
	n_assert(!this->isSetup);
	AnyFX::VarblockBase* varblock = shader->GetVkEffect()->GetVarblock(blockName.AsCharPtr());
	this->size = varblock->byteSize;

	// setup buffer
	this->Setup(numBackingBuffers);

	// begin synced update, this will cause the uniform buffer to be setup straight from the start
	this->BeginUpdateSync();

	// get a program, they should all have the same uniform offsets
	AnyFX::VkProgram* program = static_cast<AnyFX::VkProgram*>(shader->GetVkEffect()->GetPrograms().front());
	for (uint i = 0; i < varblock->variables.size(); i++)
	{
		AnyFX::VkVariable* var = static_cast<AnyFX::VkVariable*>(varblock->variables[i]);
		Ptr<CoreGraphics::ShaderVariable> svar = CoreGraphics::ShaderVariable::Create();
		Ptr<VkUniformBuffer> thisPtr(this);
		svar->BindToUniformBuffer(thisPtr.downcast<CoreGraphics::ConstantBuffer>(), program->variableBlockOffsets[var->name], var->byteSize, (int8_t*)var->currentValue);

		// add to variable and dictionary
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

} // namespace Vulkan