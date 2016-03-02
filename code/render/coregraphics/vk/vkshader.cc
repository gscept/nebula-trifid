//------------------------------------------------------------------------------
// vkshader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshader.h"
#include "coregraphics/constantbuffer.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "lowlevel/vk/vksampler.h"

namespace Vulkan
{


static int debug = 0;
__ImplementClass(Vulkan::VkShader, 'VKSH', Base::ShaderBase);
//------------------------------------------------------------------------------
/**
*/
VkShader::VkShader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShader::~VkShader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Unload()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::BeginUpdate()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::EndUpdate()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Reload()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::Cleanup()
{
	IndexT i;
	for (i = 0; i < this->immutableSamplers.Size(); i++)
	{
		vkDestroySampler(VkRenderDevice::dev, this->immutableSamplers[i], NULL);
	}
	this->immutableSamplers.Clear();

	for (i = 0; i < this->layouts.Size(); i++)
	{
		vkDestroyDescriptorSetLayout(VkRenderDevice::dev, this->layouts[i], NULL);
	}
	this->layouts.Clear();

	vkFreeDescriptorSets(VkRenderDevice::dev, VkRenderDevice::descPool, this->descriptorSets.Size(), &this->descriptorSets[0]);
	this->descriptorSets.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::OnLostDevice()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::OnResetDevice()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkShader::SetupDescriptorSets()
{
	Util::Dictionary<IndexT, Util::Array<VkDescriptorSetLayoutBinding>> sets;

	const eastl::vector<AnyFX::VarblockBase*>& varblocks = this->vkEffect->GetVarblocks();
	const eastl::vector<AnyFX::VarbufferBase*>& varbuffers = this->vkEffect->GetVarbuffers();
	const eastl::vector<AnyFX::VariableBase*>& variables = this->vkEffect->GetVariables();
	const eastl::vector<AnyFX::SamplerBase*>& samplers = this->vkEffect->GetSamplers();

	this->constantRange.size = 0;
	this->constantRange.offset = 0;
	this->constantRange.stageFlags = VK_SHADER_STAGE_ALL;

#define uint_max(a, b) (a > b ? a : b)
	uint numsets = 0;
	uint i;
	for (i = 0; i < varblocks.size(); i++)
	{
		AnyFX::VkVarblock* block = static_cast<AnyFX::VkVarblock*>(varblocks[i]);
		if (block->variables.empty()) continue;
		if (block->push)
		{
			this->constantRange.stageFlags = VK_SHADER_STAGE_ALL;
			this->constantRange.size = block->byteSize;
			this->constantRange.offset = 0;
		}
		else
		{
			IndexT index = sets.FindIndex(block->set);
			if (index == InvalidIndex)
			{
				Util::Array<VkDescriptorSetLayoutBinding> arr;
				arr.Append(block->bindingLayout);
				sets.Add(block->set, arr);
			}
			else
			{
				sets.ValueAtIndex(index).Append(block->bindingLayout);
			}
			numsets = uint_max(numsets, block->set);
		}
	}

	for (i = 0; i < varbuffers.size(); i++)
	{
		AnyFX::VkVarbuffer* buffer = static_cast<AnyFX::VkVarbuffer*>(varbuffers[i]);

		IndexT index = sets.FindIndex(buffer->set);
		if (index == InvalidIndex)
		{
			Util::Array<VkDescriptorSetLayoutBinding> arr;
			arr.Append(buffer->bindingLayout);
			sets.Add(buffer->set, arr);
		}
		else
		{
			sets.ValueAtIndex(index).Append(buffer->bindingLayout);
		}
		numsets = uint_max(numsets, buffer->set);
	}

	for (i = 0; i < samplers.size(); i++)
	{
		AnyFX::VkSampler* sampler = static_cast<AnyFX::VkSampler*>(samplers[i]);

		VkSampler vkSampler;
		VkResult res = vkCreateSampler(VkRenderDevice::dev, &sampler->samplerInfo, NULL, &vkSampler);
		n_assert(res == VK_SUCCESS);

		// add to list so we can remove it later
		this->immutableSamplers.Append(vkSampler);

		uint j;
		for (j = 0; j < sampler->textureVariables.size(); j++)
		{
			AnyFX::VkVariable* var = static_cast<AnyFX::VkVariable*>(sampler->textureVariables[j]);
			n_assert(var->type >= AnyFX::Sampler1D && var->type <= AnyFX::SamplerCubeArray);
			var->bindingLayout.pImmutableSamplers = &vkSampler;
		}
	}

	for (i = 0; i < variables.size(); i++)
	{
		AnyFX::VkVariable* variable = static_cast<AnyFX::VkVariable*>(variables[i]);

		if (variable->type >= AnyFX::Sampler1D && variable->type <= AnyFX::TextureCubeArray)
		{
			IndexT index = sets.FindIndex(variable->set);
			if (index == InvalidIndex)
			{
				Util::Array<VkDescriptorSetLayoutBinding> arr;
				arr.Append(variable->bindingLayout);
				sets.Add(variable->set, arr);
			}
			else
			{
				sets.ValueAtIndex(index).Append(variable->bindingLayout);
			}
			numsets = uint_max(numsets, variable->set);
		}
	}

	// skip the rest if we don't have any descriptor sets
	if (sets.IsEmpty()) return;
	this->layouts.Resize(sets.Size());
	for (IndexT i = 0; i < sets.Size(); i++)
	{
		VkDescriptorSetLayoutCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;
		

		const Util::Array<VkDescriptorSetLayoutBinding>& binds = sets.ValueAtIndex(i);
		info.bindingCount = binds.Size();
		info.pBindings = &binds[0];
		/*IndexT index = sets.FindIndex(i);
		if (index != InvalidIndex)
		{
			const Util::Array<VkDescriptorSetLayoutBinding>& binds = sets.ValueAtIndex(index);
			info.bindingCount = binds.Size();
			info.pBindings = &binds[0];
		}
		else
		{
			// no set found for this index, so we use a 'null' layout
			info.pBindings = VK_NULL_HANDLE;
			info.bindingCount = 0;
		}*/

		// create layout
		VkResult res = vkCreateDescriptorSetLayout(VkRenderDevice::dev, &info, NULL, &this->layouts[i]);
		assert(res == VK_SUCCESS);
	}

	VkPipelineLayoutCreateInfo layoutInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		NULL,
		0,
		this->layouts.Size(),
		layouts.Size() > 0 ? &this->layouts[0] : NULL,
		1,
		&this->constantRange
	};

	// create pipeline layout, every program should inherit this one
	VkResult res = vkCreatePipelineLayout(VkRenderDevice::dev, &layoutInfo, NULL, &this->pipelineLayout);
	assert(res == VK_SUCCESS);

	// allocate descriptor sets
	VkDescriptorSetAllocateInfo setInfo =
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		NULL,
		VkRenderDevice::descPool,
		layouts.Size(),
		layouts.Size() > 0 ? &this->layouts[0] : NULL
	};

	debug++;
	this->descriptorSets.Resize(layouts.Size());
	res = vkAllocateDescriptorSets(VkRenderDevice::dev, &setInfo, this->descriptorSets.Size() > 0 ? &this->descriptorSets[0] : NULL);
	assert(res == VK_SUCCESS);
}

} // namespace Vulkan