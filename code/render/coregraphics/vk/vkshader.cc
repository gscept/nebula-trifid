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
static int debug2 = 0;
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
VkShader::CreateDescriptorSetLayout(AnyFX::ShaderEffect* effect)
{
	const eastl::vector<AnyFX::VarblockBase*>& varblocks = effect->GetVarblocks();
	const eastl::vector<AnyFX::VarbufferBase*>& varbuffers = effect->GetVarbuffers();
	const eastl::vector<AnyFX::VariableBase*>& variables = effect->GetVariables();
	const eastl::vector<AnyFX::SamplerBase*>& samplers = effect->GetSamplers();

	this->constantRange.size = 0;
	this->constantRange.offset = 0;
	this->constantRange.stageFlags = VK_SHADER_STAGE_ALL;
	uint32_t numsets = 0;

#define AMD_DESC_SETS 1

#define uint_max(a, b) (a > b ? a : b)
	uint i;
	for (i = 0; i < varblocks.size(); i++)
	{
		AnyFX::VkVarblock* block = static_cast<AnyFX::VkVarblock*>(varblocks[i]);
		//if (this->program->activeVarblockNames.find(block->name) == this->program->activeVarblockNames.end()) continue;
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
				this->sets.Add(block->set, arr);
#ifdef AMD_DESC_SETS
				numsets = uint_max(numsets, block->set + 1);
#else
				numsets++;
#endif

			}
			else
			{
				this->sets.ValueAtIndex(index).Append(block->bindingLayout);
			}
		}
	}

	for (i = 0; i < varbuffers.size(); i++)
	{
		AnyFX::VkVarbuffer* buffer = static_cast<AnyFX::VkVarbuffer*>(varbuffers[i]);

		IndexT index = this->sets.FindIndex(buffer->set);
		if (index == InvalidIndex)
		{
			Util::Array<VkDescriptorSetLayoutBinding> arr;
			arr.Append(buffer->bindingLayout);
			this->sets.Add(buffer->set, arr);
		}
		else
		{
			this->sets.ValueAtIndex(index).Append(buffer->bindingLayout);
		}
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
		//if (this->program->activeVariableNames.find(variable->name) == this->program->activeVariableNames.end()) continue;
		if (variable->type >= AnyFX::Sampler1D && variable->type <= AnyFX::TextureCubeArray)
		{
			IndexT index = this->sets.FindIndex(variable->set);
			if (index == InvalidIndex)
			{
				Util::Array<VkDescriptorSetLayoutBinding> arr;
				arr.Append(variable->bindingLayout);
				this->sets.Add(variable->set, arr);

#ifdef AMD_DESC_SETS
				numsets = uint_max(numsets, variable->set + 1);
#else
				numsets++;
#endif
			}
			else
			{
				this->sets.ValueAtIndex(index).Append(variable->bindingLayout);
			}
		}
	}

	// skip the rest if we don't have any descriptor sets
	if (!sets.IsEmpty())
	{
		this->layouts.Resize(numsets);
		for (IndexT i = 0; i < this->layouts.Size(); i++)
		{
			VkDescriptorSetLayoutCreateInfo info;
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.pNext = NULL;
			info.flags = 0;

#ifdef AMD_DESC_SETS
			info.bindingCount = 0;
			info.pBindings = VK_NULL_HANDLE;
			if (this->sets.Contains(i))
			{
				const Util::Array<VkDescriptorSetLayoutBinding>& binds = this->sets[i];
				info.bindingCount = binds.Size();
				info.pBindings = binds.Size() > 0 ? &binds[0] : VK_NULL_HANDLE;
			}
#else
			const Util::Array<VkDescriptorSetLayoutBinding>& binds = sets.ValueAtIndex(i);
			info.bindingCount = binds.Size();
			info.pBindings = binds.Size() > 0 ? &binds[0] : VK_NULL_HANDLE;
#endif

			// create layout
			VkResult res = vkCreateDescriptorSetLayout(VkRenderDevice::dev, &info, NULL, &this->layouts[i]);
			assert(res == VK_SUCCESS);
		}
	}

	VkPipelineLayoutCreateInfo layoutInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		NULL,
		0,
		this->layouts.Size(),
		this->layouts.Size() > 0 ? &this->layouts[0] : NULL,
		0,
		&this->constantRange
	};

	// create pipeline layout, every program should inherit this one
	VkResult res = vkCreatePipelineLayout(VkRenderDevice::dev, &layoutInfo, NULL, &this->pipelineLayout);
	assert(res == VK_SUCCESS);
}

} // namespace Vulkan