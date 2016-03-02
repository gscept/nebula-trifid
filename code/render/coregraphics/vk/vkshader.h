#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader effect (using AnyFX) in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderbase.h"
#include "lowlevel/afxapi.h"

namespace CoreGraphics
{
	class ConstantBuffer;
}

namespace Vulkan
{
class VkShader : public Base::ShaderBase
{
	__DeclareClass(VkShader);
public:
	/// constructor
	VkShader();
	/// destructor
	virtual ~VkShader();

	/// unload the resource, or cancel the pending load
	virtual void Unload();
	/// returns effect
	AnyFX::ShaderEffect* GetVkEffect() const;

	/// begin updating shader state
	void BeginUpdate();
	/// end updating shader state
	void EndUpdate();

	/// reloads a shader from file
	void Reload();

private:
	friend class VkStreamShaderLoader;

	/// cleans up the shader
	void Cleanup();

	/// called by ogl4 shader server when ogl4 device is lost
	void OnLostDevice();
	/// called by ogl4 shader server when ogl4 device is reset
	void OnResetDevice();

	/// setup descriptors
	void SetupDescriptorSets();

	AnyFX::ShaderEffect* vkEffect;
	VkPipelineLayout pipelineLayout;

	Util::Array<VkSampler> immutableSamplers;
	VkPushConstantRange constantRange;
	Util::FixedArray<VkDescriptorSetLayout> layouts;
	Util::FixedArray<VkDescriptorSet> descriptorSets;

	Ptr<CoreGraphics::ConstantBuffer> globalBlockBuffer;
	Ptr<CoreGraphics::ShaderVariable> globalBlockBufferVar;
};


//------------------------------------------------------------------------------
/**
*/
inline AnyFX::ShaderEffect*
VkShader::GetVkEffect() const
{
	return this->vkEffect;
}

} // namespace Vulkan