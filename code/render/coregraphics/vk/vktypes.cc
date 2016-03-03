//------------------------------------------------------------------------------
// vktypes.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vktypes.h"

namespace Vulkan
{

//------------------------------------------------------------------------------
/**
*/
VkFormat
VkTypes::AsVkFormat(CoreGraphics::PixelFormat::Code p)
{
	return VK_FORMAT_UNDEFINED;
}

//------------------------------------------------------------------------------
/**
*/
VkFormat
VkTypes::AsVkFormat(ILenum p)
{
	return VK_FORMAT_UNDEFINED;
}

//------------------------------------------------------------------------------
/**
*/
CoreGraphics::PixelFormat::Code
VkTypes::AsNebulaPixelFormat(VkFormat f)
{
	return CoreGraphics::PixelFormat::InvalidPixelFormat;
}

//------------------------------------------------------------------------------
/**
*/
VkComponentMapping
VkTypes::AsVkMapping(CoreGraphics::PixelFormat::Code p)
{
	VkComponentMapping mapping;
	mapping.r = VK_COMPONENT_SWIZZLE_R;
	mapping.g = VK_COMPONENT_SWIZZLE_G;
	mapping.b = VK_COMPONENT_SWIZZLE_B;
	mapping.a = VK_COMPONENT_SWIZZLE_A;

	switch (p)
	{
	case CoreGraphics::PixelFormat::A8B8G8R8:
		mapping.r = VK_COMPONENT_SWIZZLE_B;
		mapping.b = VK_COMPONENT_SWIZZLE_R;
		break;
	}

	return mapping;
}

//------------------------------------------------------------------------------
/**
*/
VkPrimitiveTopology
VkTypes::AsVkPrimitiveType(CoreGraphics::PrimitiveTopology::Code t)
{
	return VK_PRIMITIVE_TOPOLOGY_BEGIN_RANGE;
}

//------------------------------------------------------------------------------
/**
*/
uint32_t
VkTypes::AsByteSize(uint32_t semantic)
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
uint32_t
VkTypes::AsVkSize(CoreGraphics::VertexComponent::Format f)
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
uint32_t
VkTypes::AsVkNumComponents(CoreGraphics::VertexComponent::Format f)
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
VkFormat
VkTypes::AsVkVertexType(CoreGraphics::VertexComponent::Format f)
{
	return VK_FORMAT_R32_SINT;
}

} // namespace Vulkan