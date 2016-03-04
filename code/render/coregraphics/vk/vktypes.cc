//------------------------------------------------------------------------------
// vktypes.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vktypes.h"
#include <vulkan/vulkan.h>
#include "coregraphics/pixelformat.h"
#include "include/il_dds.h"

namespace Vulkan
{

using namespace CoreGraphics;
//------------------------------------------------------------------------------
/**
*/
VkFormat
VkTypes::AsVkFormat(CoreGraphics::PixelFormat::Code p)
{
	switch (p)
	{
	case PixelFormat::X8R8G8B8:         return VK_FORMAT_R8G8B8A8_UINT;
	case PixelFormat::A8R8G8B8:         return VK_FORMAT_R8G8B8A8_UINT;
	case PixelFormat::R8G8B8:           return VK_FORMAT_R8G8B8_UINT;
	case PixelFormat::R5G6B5:           return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case PixelFormat::SRGBA8:			return VK_FORMAT_R8G8B8A8_SRGB;
	case PixelFormat::A1R5G5B5:         return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case PixelFormat::A4R4G4B4:         return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case PixelFormat::DXT1:             return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case PixelFormat::DXT1A:            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case PixelFormat::DXT3:             return VK_FORMAT_BC2_UNORM_BLOCK;
	case PixelFormat::DXT5:             return VK_FORMAT_BC3_UNORM_BLOCK;
	case PixelFormat::DXT1sRGB:         return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case PixelFormat::DXT1AsRGB:        return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case PixelFormat::DXT3sRGB:         return VK_FORMAT_BC2_SRGB_BLOCK;
	case PixelFormat::DXT5sRGB:         return VK_FORMAT_BC3_SRGB_BLOCK;
	case PixelFormat::BC7:				return VK_FORMAT_BC7_UNORM_BLOCK;
	case PixelFormat::BC7sRGB:			return VK_FORMAT_BC7_SRGB_BLOCK;
	case PixelFormat::R16F:             return VK_FORMAT_R16_SFLOAT;
	case PixelFormat::G16R16F:          return VK_FORMAT_R16G16_SFLOAT;
	case PixelFormat::A16B16G16R16F:    return VK_FORMAT_R16G16B16A16_SFLOAT;
	case PixelFormat::A16B16G16R16:		return VK_FORMAT_R16G16B16A16_UINT;
	case PixelFormat::R11G11B10F:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case PixelFormat::R32F:             return VK_FORMAT_R32_SFLOAT;
	case PixelFormat::G32R32F:          return VK_FORMAT_R32G32_SFLOAT;
	case PixelFormat::A32B32G32R32F:    return VK_FORMAT_R32G32B32A32_SFLOAT;
	case PixelFormat::R32G32B32F:		return VK_FORMAT_R32G32B32_SFLOAT;
	case PixelFormat::A8:               return VK_FORMAT_R8_SINT;
	case PixelFormat::R8:               return VK_FORMAT_R8_SINT;
	case PixelFormat::G8:               return VK_FORMAT_R8_SINT;
	case PixelFormat::B8:               return VK_FORMAT_R8_SINT;
	case PixelFormat::A2R10G10B10:      return VK_FORMAT_A2B10G10R10_UINT_PACK32;
	case PixelFormat::G16R16:           return VK_FORMAT_R16G16_UINT;
	case PixelFormat::D24X8:			return VK_FORMAT_X8_D24_UNORM_PACK32;
	case PixelFormat::D24S8:            return VK_FORMAT_D24_UNORM_S8_UINT;
	default:
		{
			n_error("VkTypes::AsVkFormat(): invalid pixel format '%d'", p);
			return VK_FORMAT_R8G8B8A8_UINT;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
VkFormat
VkTypes::AsVkFormat(ILenum p)
{
	switch (p)
	{
	case PF_DXT1:				return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case PF_DXT3:				return VK_FORMAT_BC2_UNORM_BLOCK;
	case PF_DXT5:				return VK_FORMAT_BC3_UNORM_BLOCK;
	case PF_BC7:				return VK_FORMAT_BC7_UNORM_BLOCK;
	case PF_DXT1_sRGB:			return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case PF_DXT3_sRGB:			return VK_FORMAT_BC2_SRGB_BLOCK;
	case PF_DXT5_sRGB:			return VK_FORMAT_BC3_SRGB_BLOCK;
	case PF_BC7_sRGB:			return VK_FORMAT_BC7_SRGB_BLOCK;
	case PF_A16R16G16B16:		return VK_FORMAT_R16G16B16A16_UINT;
	case PF_A16B16G16R16:		return VK_FORMAT_R16G16B16A16_UINT;
	case PF_A16R16B16G16F:		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case PF_A16B16G16R16F:		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case PF_A32B32G32R32F:		return VK_FORMAT_R16G16B16A16_UINT;
	case PF_A32R32G32B32F:		return VK_FORMAT_R16G16B16A16_UINT;
	case PF_R16F:				return VK_FORMAT_R16_SFLOAT;
	case PF_R32F:				return VK_FORMAT_R32_SFLOAT;
	case PF_G16R16F:			return VK_FORMAT_R16G16_SFLOAT;
	case PF_G32R32F:			return VK_FORMAT_R32G32_SFLOAT;
	default:
		{
			n_error("VkTypes::AsVkFormat(): invalid compression '%d'", p);
			return VK_FORMAT_A8B8G8R8_UINT_PACK32;
		}		
	}
}

//------------------------------------------------------------------------------
/**
*/
CoreGraphics::PixelFormat::Code
VkTypes::AsNebulaPixelFormat(VkFormat f)
{
	
	switch (f)
	{
	case	VK_FORMAT_R8G8B8A8_UINT:					return	PixelFormat::A8R8G8B8;
	case	VK_FORMAT_R8G8B8_UINT:						return	PixelFormat::R8G8B8;
	case	VK_FORMAT_R5G6B5_UNORM_PACK16:				return	PixelFormat::R5G6B5;
	case	VK_FORMAT_R8G8B8A8_SRGB:					return	PixelFormat::SRGBA8;
	case	VK_FORMAT_R5G5B5A1_UNORM_PACK16:			return	PixelFormat::A1R5G5B5;
	case	VK_FORMAT_R4G4B4A4_UNORM_PACK16:			return	PixelFormat::A4R4G4B4;
	case	VK_FORMAT_BC1_RGB_UNORM_BLOCK:				return	PixelFormat::DXT1;
	case	VK_FORMAT_BC1_RGBA_UNORM_BLOCK:				return	PixelFormat::DXT1A;
	case	VK_FORMAT_BC2_UNORM_BLOCK:					return	PixelFormat::DXT3;
	case	VK_FORMAT_BC3_UNORM_BLOCK:					return	PixelFormat::DXT5;
	case	VK_FORMAT_BC1_RGB_SRGB_BLOCK:				return	PixelFormat::DXT1sRGB;
	case	VK_FORMAT_BC1_RGBA_SRGB_BLOCK:				return	PixelFormat::DXT1AsRGB;
	case	VK_FORMAT_BC2_SRGB_BLOCK:					return	PixelFormat::DXT3sRGB;
	case	VK_FORMAT_BC3_SRGB_BLOCK:					return	PixelFormat::DXT5sRGB;
	case	VK_FORMAT_BC7_UNORM_BLOCK:					return	PixelFormat::BC7;
	case	VK_FORMAT_BC7_SRGB_BLOCK:					return	PixelFormat::BC7sRGB;
	case	VK_FORMAT_R16_SFLOAT:						return	PixelFormat::R16F;
	case	VK_FORMAT_R16G16_SFLOAT:					return	PixelFormat::G16R16F;
	case	VK_FORMAT_R16G16B16A16_SFLOAT:				return	PixelFormat::A16B16G16R16F;
	case	VK_FORMAT_R16G16B16A16_UINT:				return	PixelFormat::A16B16G16R16;
	case	VK_FORMAT_B10G11R11_UFLOAT_PACK32:			return	PixelFormat::R11G11B10F;
	case	VK_FORMAT_R32_SFLOAT:						return	PixelFormat::R32F;
	case	VK_FORMAT_R32G32_SFLOAT:					return	PixelFormat::G32R32F;
	case	VK_FORMAT_R32G32B32A32_SFLOAT:				return	PixelFormat::A32B32G32R32F;
	case	VK_FORMAT_R32G32B32_SFLOAT:					return	PixelFormat::R32G32B32F;
	case	VK_FORMAT_R8_SINT:							return	PixelFormat::R8;
	case	VK_FORMAT_A2B10G10R10_UINT_PACK32:			return	PixelFormat::A2R10G10B10;
	case	VK_FORMAT_R16G16_UINT:						return	PixelFormat::G16R16;
	case	VK_FORMAT_X8_D24_UNORM_PACK32:				return	PixelFormat::D24X8;
	case	VK_FORMAT_D24_UNORM_S8_UINT:				return	PixelFormat::D24S8;
	default:
	{
		n_error("VkTypes::AsNebulaPixelFormat(): invalid pixel format '%d'", f);
		return PixelFormat::A8R8G8B8;
	}
	}
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
VkComponentMapping
VkTypes::AsVkMapping(ILenum p)
{
	VkComponentMapping mapping;
	mapping.r = VK_COMPONENT_SWIZZLE_R;
	mapping.g = VK_COMPONENT_SWIZZLE_G;
	mapping.b = VK_COMPONENT_SWIZZLE_B;
	mapping.a = VK_COMPONENT_SWIZZLE_A;

	switch (p)
	{
	case PF_DXT1:				
	case PF_DXT3:				
	case PF_DXT5:				
	case PF_BC7:				
	case PF_DXT1_sRGB:			
	case PF_DXT3_sRGB:			
	case PF_DXT5_sRGB:			
	case PF_BC7_sRGB:			
	case PF_A16R16G16B16:		
	case PF_A16R16B16G16F:		
	case PF_A32R32G32B32F:		
	case PF_R16F:				
	case PF_R32F:				
		break;
	case PF_A16B16G16R16:		
	case PF_A16B16G16R16F:		
	case PF_A32B32G32R32F:
		mapping.r = VK_COMPONENT_SWIZZLE_B;
		mapping.b = VK_COMPONENT_SWIZZLE_R;
		break;
	case PF_G16R16F:
	case PF_G32R32F:
		mapping.g = VK_COMPONENT_SWIZZLE_R;
		mapping.r = VK_COMPONENT_SWIZZLE_G;
	default:
		{
			n_error("VkTypes::AsVkFormat(): invalid compression '%d'", p);
		}
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