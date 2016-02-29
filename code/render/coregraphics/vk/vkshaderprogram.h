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
	void Setup(AnyFX::VkProgram* program);

	/// create shader object
	void CreateShader(VkShaderModule* shader, unsigned binarySize, char* binary);
	/// create this program as a graphics program
	void SetupAsGraphics();
	/// create this program as a compute program (can be done immediately)
	void SetupAsCompute();

	AnyFX::VkProgram* program;
	AnyFX::VkRenderState* renderState;

	VkPushConstantRange constantRange;
	Util::Array<VkDescriptorSetLayout> descriptorLayouts;
	Util::Array<VkDescriptorSet> descriptorSets;

	VkShaderModule vs, hs, ds, gs, ps, cs;

	VkPipeline computePipeline;
	VkPipelineLayout pipelineLayout;
	VkGraphicsPipelineCreateInfo shaderPipelineInfo;
	PipelineType pipelineType;
};
} // namespace Vulkan