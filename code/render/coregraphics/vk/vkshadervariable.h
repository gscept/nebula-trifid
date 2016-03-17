#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader variable in Vulkan (push constant?!?!?!?!)
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shadervariablebase.h"
#include "lowlevel/vk/vkvarblock.h"
#include "lowlevel/vk/vkvarbuffer.h"
#include "lowlevel/vk/vkvariable.h"

namespace CoreGraphics
{
class ConstantBuffer;
class Texture;
}

namespace Vulkan
{
class VkStreamShaderLoader;
class VkShaderInstance;
class VkShaderVariable : public Base::ShaderVariableBase
{
	__DeclareClass(VkShaderVariable);
public:
	/// constructor
	VkShaderVariable();
	/// destructor
	virtual ~VkShaderVariable();

	/// bind variable to uniform buffer
	void BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size, int8_t* defaultValue);

private:

	friend class Vulkan::VkStreamShaderLoader;
	friend class Vulkan::VkShaderInstance;

	/// setup from AnyFX variable
	void Setup(AnyFX::VkVariable* var);
	/// setup from AnyFX varbuffer
	void Setup(AnyFX::VkVarbuffer* var);
	/// setup from AnyFX varblock
	void Setup(AnyFX::VkVarblock* var);

	struct BufferBinding
	{
		Ptr<CoreGraphics::ConstantBuffer> uniformBuffer;
		uint32_t offset;
		uint32_t size;
		int8_t* defaultValue;
	} *bufferBinding;

	union
	{
		AnyFX::VkVariable* var;
		AnyFX::VkVarbuffer* buffer;
		AnyFX::VkVarblock* block;
	};
};
} // namespace Vulkan