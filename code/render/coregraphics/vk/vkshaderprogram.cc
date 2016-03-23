//------------------------------------------------------------------------------
// vkshaderprogram.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderprogram.h"
#include "vkrenderdevice.h"
#include "coregraphics/shaderserver.h"
#include "lowlevel/vk/vkrenderstate.h"
#include "lowlevel/vk/vksampler.h"

using namespace Util;
namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderProgram, 'VKSP', Base::ShaderVariationBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderProgram::VkShaderProgram() :
	vs(VK_NULL_HANDLE),
	hs(VK_NULL_HANDLE),
	ds(VK_NULL_HANDLE),
	gs(VK_NULL_HANDLE),
	ps(VK_NULL_HANDLE),
	cs(VK_NULL_HANDLE),
	pipelineType(InvalidType),
	computePipeline(VK_NULL_HANDLE)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderProgram::~VkShaderProgram()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::Apply()
{
	n_assert(this->program);

	// if we are compute, we can set the pipeline straight away, otherwise we have to accumulate the infos
	if (this->pipelineType == Compute)	vkCmdBindPipeline(VkRenderDevice::mainCmdCmpBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipeline);
	else								VkRenderDevice::Instance()->SetShaderPipelineInfo(this->shaderPipelineInfo);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::Commit()
{
	n_assert(this->program);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::Setup(AnyFX::VkProgram* program, AnyFX::ShaderEffect* effect)
{
	this->program = program;
	String mask = program->GetAnnotationString("Mask").c_str();
	String name = program->name.c_str();

	this->CreateShader(&this->vs, program->shaderBlock.vsBinarySize, program->shaderBlock.vsBinary);
	this->CreateShader(&this->hs, program->shaderBlock.hsBinarySize, program->shaderBlock.hsBinary);
	this->CreateShader(&this->ds, program->shaderBlock.dsBinarySize, program->shaderBlock.dsBinary);
	this->CreateShader(&this->gs, program->shaderBlock.gsBinarySize, program->shaderBlock.gsBinary);
	this->CreateShader(&this->ps, program->shaderBlock.psBinarySize, program->shaderBlock.psBinary);
	this->CreateShader(&this->cs, program->shaderBlock.csBinarySize, program->shaderBlock.csBinary);

	// setup pipeline layout
	this->SetupDescriptorLayout(effect);

	// if we have a compute shader, it will be the one we use, otherwise use the graphics one
	if (this->cs) this->SetupAsCompute();
	else		  this->SetupAsGraphics();

	// setup feature mask and name
	this->SetFeatureMask(CoreGraphics::ShaderServer::Instance()->FeatureStringToMask(mask));
	this->SetName(name);
	this->SetNumPasses(1);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::CreateShader(VkShaderModule* shader, unsigned binarySize, char* binary)
{
	if (binarySize > 0)
	{
		VkShaderModuleCreateInfo info =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			NULL,
			0,										// flags
			binarySize,	// Vulkan expects the binary to be uint32, so we must assume size is in units of 4 bytes
			(unsigned*)binary
		};

		// create shader
		VkResult res = vkCreateShaderModule(VkRenderDevice::dev, &info, NULL, shader);
		assert(res == VK_SUCCESS);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::SetupAsGraphics()
{
	// we have to keep track of how MANY shaders we are using too
	unsigned shaderIdx = 0;

	// attach vertex shader
	if (0 != this->vs)
	{
		this->shaderInfos[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->shaderInfos[shaderIdx].pNext = NULL;
		this->shaderInfos[shaderIdx].flags = 0;
		this->shaderInfos[shaderIdx].stage = VK_SHADER_STAGE_VERTEX_BIT;
		this->shaderInfos[shaderIdx].module = this->vs;
		this->shaderInfos[shaderIdx].pName = "main";
		this->shaderInfos[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->hs)
	{
		this->shaderInfos[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->shaderInfos[shaderIdx].pNext = NULL;
		this->shaderInfos[shaderIdx].flags = 0;
		this->shaderInfos[shaderIdx].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		this->shaderInfos[shaderIdx].module = this->hs;
		this->shaderInfos[shaderIdx].pName = "main";
		this->shaderInfos[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->ds)
	{
		this->shaderInfos[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->shaderInfos[shaderIdx].pNext = NULL;
		this->shaderInfos[shaderIdx].flags = 0;
		this->shaderInfos[shaderIdx].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		this->shaderInfos[shaderIdx].module = this->ds;
		this->shaderInfos[shaderIdx].pName = "main";
		this->shaderInfos[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->gs)
	{
		this->shaderInfos[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->shaderInfos[shaderIdx].pNext = NULL;
		this->shaderInfos[shaderIdx].flags = 0;
		this->shaderInfos[shaderIdx].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		this->shaderInfos[shaderIdx].module = this->gs;
		this->shaderInfos[shaderIdx].pName = "main";
		this->shaderInfos[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->ps)
	{
		this->shaderInfos[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->shaderInfos[shaderIdx].pNext = NULL;
		this->shaderInfos[shaderIdx].flags = 0;
		this->shaderInfos[shaderIdx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		this->shaderInfos[shaderIdx].module = this->ps;
		this->shaderInfos[shaderIdx].pName = "main";
		this->shaderInfos[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	// retrieve implementation specific state
	AnyFX::VkRenderState* vkRenderState = static_cast<AnyFX::VkRenderState*>(this->program->renderState);

	this->rasterizerInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		NULL,
		0
	};
	vkRenderState->SetupRasterization(&this->rasterizerInfo);

	this->multisampleInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		NULL,
		0
	};
	vkRenderState->SetupMultisample(&this->multisampleInfo);

	this->depthStencilInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		NULL,
		0
	};
	vkRenderState->SetupDepthStencil(&this->depthStencilInfo);

	this->colorBlendInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		NULL,
		0
	};
	vkRenderState->SetupBlend(&this->colorBlendInfo);

	this->tessInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		NULL,
		0,
		this->program->patchSize
	};

	// setup dynamic state, we only support dynamic viewports and scissor rects
	static const VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_SCISSOR };
	this->dynamicInfo = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		NULL,
		0,
		sizeof(dynamicStates) / sizeof(VkDynamicState),
		dynamicStates
	};

	// setup pipeline information regarding the shader state
	this->shaderPipelineInfo =
	{
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		NULL,
		VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,
		shaderIdx,
		this->shaderInfos,
		NULL, NULL,					// these are vertex related and will depend upon the vertex layout we use
		program->supportsTessellation ? &this->tessInfo : VK_NULL_HANDLE,
		NULL,						// this is our viewport and is setup by the framebuffer
		&this->rasterizerInfo,
		&this->multisampleInfo,
		&this->depthStencilInfo,
		&this->colorBlendInfo,
		&this->dynamicInfo,					
		this->pipelineLayout,
		NULL,							// pass specific stuff, keep as NULL
		0,
		VK_NULL_HANDLE, 0				// base pipeline is kept as NULL too, because this is the base for all derivatives
	};

	// be sure to flag compute shader as null
	this->pipelineType = Graphics;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::SetupAsCompute()
{
	// create 6 shader info stages for each shader type
	n_assert(0 != this->cs);

	VkPipelineShaderStageCreateInfo shader =
	{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		NULL,
		0,
		VK_SHADER_STAGE_COMPUTE_BIT,
		this->cs,
		"main",
		VK_NULL_HANDLE,
	};

	VkComputePipelineCreateInfo info =
	{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		NULL,
		0,
		shader,
		this->pipelineLayout,
		VK_NULL_HANDLE, -1
	};

	// create pipeline
	VkResult res = vkCreateComputePipelines(VkRenderDevice::dev, VkRenderDevice::cache, 1, &info, NULL, &this->computePipeline);
	n_assert(res == VK_SUCCESS);
	this->pipelineType = Compute;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::SetupDescriptorLayout(AnyFX::ShaderEffect* effect)
{
	Util::Dictionary<IndexT, Util::Array<VkDescriptorSetLayoutBinding>> sets;

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
				sets.Add(block->set, arr);
#ifdef AMD_DESC_SETS
				numsets = uint_max(numsets, block->set + 1);
#else
				numsets++;
#endif
				
			}
			else
			{
				sets.ValueAtIndex(index).Append(block->bindingLayout);
			}
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
			IndexT index = sets.FindIndex(variable->set);
			if (index == InvalidIndex)
			{
				Util::Array<VkDescriptorSetLayoutBinding> arr;
				arr.Append(variable->bindingLayout);
				sets.Add(variable->set, arr);
				
#ifdef AMD_DESC_SETS
				numsets = uint_max(numsets, variable->set + 1);
#else
				numsets++;
#endif
			}
			else
			{
				sets.ValueAtIndex(index).Append(variable->bindingLayout);
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
			info.bindingCount = 0;
			info.pBindings = VK_NULL_HANDLE;

#if AMD_DESC_SETS
			if (sets.Contains(i))
			{
				const Util::Array<VkDescriptorSetLayoutBinding>& binds = sets[i];
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