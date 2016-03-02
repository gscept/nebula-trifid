#pragma once
//------------------------------------------------------------------------------
/**
	Implements a uniform buffer used for shader uniforms in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/constantbufferbase.h"
namespace Vulkan
{
class VkUniformBuffer : public Base::ConstantBufferBase
{
	__DeclareClass(VkUniformBuffer);
public:
	/// constructor
	VkUniformBuffer();
	/// destructor
	virtual ~VkUniformBuffer();

	/// setup buffer
	void Setup(const SizeT numBackingBuffers = DefaultNumBackingBuffers);
	/// bind variables in a block with a name in a shader to this buffer (only do this on system managed blocks)
	void SetupFromBlockInShader(const Ptr<CoreGraphics::Shader>& shader, const Util::String& blockName, const SizeT numBackingBuffers = DefaultNumBackingBuffers);
	/// discard buffer
	void Discard();
private:

	VkDeviceMemory mem;
	VkBuffer buf;
};
} // namespace Vulkan