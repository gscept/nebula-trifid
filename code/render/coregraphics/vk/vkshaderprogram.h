#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader variation (shader program within effect) in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shadervariationbase.h"
#include "lowlevel/vk/vkprogram.h"
#include "lowlevel/vk/vkrenderstate.h"
#include "lowlevel/afxapi.h"

namespace Vulkan
{
class VkShaderProgram : public Base::ShaderVariationBase
{
	__DeclareClass(VkShaderProgram);
public:
	/// constructor
	VkShaderProgram();
	/// destructor
	virtual ~VkShaderProgram();

	/// applies program
	void Apply();
	/// performs a variable commit to the current program
	void Commit();
private:

	friend class VkShader;
	friend class VkStreamShaderLoader;

	enum PipelineType
	{
		InvalidType,
		Compute,
		Graphics
	};

	/// setup from AnyFX program
	void Setup(AnyFX::VkProgram* program, AnyFX::ShaderEffect* effect);

	/// create shader object
	void CreateShader(VkShaderModule* shader, unsigned binarySize, char* binary);
	/// create this program as a graphics program
	void SetupAsGraphics();
	/// create this program as a compute program (can be done immediately)
	void SetupAsCompute();

	/// setup descriptor pipeline layout
	void SetupDescriptorLayout(AnyFX::ShaderEffect* effect);

	AnyFX::VkProgram* program;

	Util::Array<VkSampler> immutableSamplers;
	VkPushConstantRange constantRange;
	Util::FixedArray<VkDescriptorSetLayout> layouts;
	Util::FixedArray<VkDescriptorSet> descriptorSets;

	VkShaderModule vs, hs, ds, gs, ps, cs;

	VkPipeline computePipeline;
	VkPipelineLayout pipelineLayout;

	VkPipelineRasterizationStateCreateInfo rasterizerInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDynamicStateCreateInfo dynamicInfo;
	VkPipelineShaderStageCreateInfo shaderInfos[5];
	VkGraphicsPipelineCreateInfo shaderPipelineInfo;
	PipelineType pipelineType;
};


} // namespace Vulkan