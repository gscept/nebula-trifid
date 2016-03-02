//------------------------------------------------------------------------------
// vkshaderprogram.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderprogram.h"
#include "vkrenderdevice.h"
#include "coregraphics/shaderserver.h"
#include "lowlevel/vk/vkrenderstate.h"

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
	cs(VK_NULL_HANDLE)
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
VkShaderProgram::Setup(AnyFX::VkProgram* program, VkPipelineLayout pipeline)
{
	this->program = program;
	this->renderState = renderState;
	String mask = program->GetAnnotationString("Mask").c_str();
	String name = program->name.c_str();
	this->pipelineLayout = pipeline;

	this->CreateShader(&this->vs, program->shaderBlock.vsBinarySize, program->shaderBlock.vsBinary);
	this->CreateShader(&this->hs, program->shaderBlock.hsBinarySize, program->shaderBlock.hsBinary);
	this->CreateShader(&this->ds, program->shaderBlock.dsBinarySize, program->shaderBlock.dsBinary);
	this->CreateShader(&this->gs, program->shaderBlock.gsBinarySize, program->shaderBlock.gsBinary);
	this->CreateShader(&this->ps, program->shaderBlock.psBinarySize, program->shaderBlock.psBinary);
	this->CreateShader(&this->cs, program->shaderBlock.csBinarySize, program->shaderBlock.csBinary);

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
			binarySize / sizeof(unsigned),	// Vulkan expects the binary to be uint32, so we must assume size is in units of 4 bytes
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
	// create 6 shader info stages for each shader type
	VkPipelineShaderStageCreateInfo shaders[6];

	// we have to keep track of how MANY shaders we are using too
	unsigned shaderIdx = 0;

	// attach vertex shader
	if (0 != this->vs)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaders[shaderIdx].module = this->vs;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->hs)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		shaders[shaderIdx].module = this->hs;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->ds)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		shaders[shaderIdx].module = this->ds;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->gs)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		shaders[shaderIdx].module = this->gs;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->ps)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaders[shaderIdx].module = this->ps;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	if (0 != this->cs)
	{
		shaders[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaders[shaderIdx].pNext = NULL;
		shaders[shaderIdx].flags = 0;
		shaders[shaderIdx].stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaders[shaderIdx].module = this->cs;
		shaders[shaderIdx].pName = "main";
		shaders[shaderIdx].pSpecializationInfo = NULL;
		shaderIdx++;
	}

	// retrieve implementation specific state
	AnyFX::VkRenderState* vkRenderState = static_cast<AnyFX::VkRenderState*>(this->program->renderState);

	VkPipelineRasterizationStateCreateInfo rastInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		NULL,
	};
	vkRenderState->SetupRasterization(&rastInfo);

	VkPipelineMultisampleStateCreateInfo msInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		NULL,
	};
	vkRenderState->SetupMultisample(&msInfo);

	VkPipelineDepthStencilStateCreateInfo dsInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		NULL,
	};
	vkRenderState->SetupDepthStencil(&dsInfo);

	VkPipelineColorBlendStateCreateInfo blendInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		NULL,
	};
	vkRenderState->SetupBlend(&blendInfo);

	// setup dynamic state, we only support dynamic viewports and scissor rects
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicInfo = 
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
		shaders,
		NULL, NULL, NULL, NULL,			// these are our primitive and viewport infos, keep them null to create derivatives later
		&rastInfo,
		&msInfo,
		&dsInfo,
		&blendInfo,
		&dynamicInfo,					
		this->pipelineLayout,
		NULL,							// pass specific stuff, keep as NULL
		0,
		0, -1							// base pipeline is kept as NULL too, because this is the base for all derivatives
	};

	// be sure to flag compute shader as null
	this->computePipeline = VK_NULL_HANDLE;
	this->pipelineType = Graphics;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderProgram::SetupAsCompute()
{
	// create 6 shader info stages for each shader type
	VkPipelineShaderStageCreateInfo shader;

	if (0 != this->cs)
	{
		shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader.pNext = NULL;
		shader.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shader.module = this->cs;
	}

	VkComputePipelineCreateInfo info =
	{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		NULL,
		VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,
		shader,
		NULL,
		NULL, -1							// base pipeline is kept as 'NULL' too, because this is the base for all derivatives
	};

	// create pipeline
	vkCreateComputePipelines(VkRenderDevice::dev, VkRenderDevice::cache, 1, &info, NULL, &this->computePipeline);
	this->pipelineType = Compute;
}

} // namespace Vulkan