//------------------------------------------------------------------------------
// vktexture.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vktexture.h"
#include "vkrenderdevice.h"
#include "../pixelformat.h"
#include "vktypes.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkTexture, 'VKTX', Base::TextureBase);
//------------------------------------------------------------------------------
/**
*/
VkTexture::VkTexture()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkTexture::~VkTexture()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Unload()
{
	vkDestroyImage(VkRenderDevice::dev, this->img, NULL);
}

//------------------------------------------------------------------------------
/**
*/
bool
VkTexture::Map(IndexT mipLevel, MapType mapType, MapInfo& outMapInfo)
{
	bool retval = false;
	if (Texture2D == this->type)
	{		
		VkFormat format = VkTypes::AsVkFormat(this->pixelFormat);
		uint32_t size = CoreGraphics::PixelFormat::ToSize(this->pixelFormat);
		
		int32_t mipWidth;
		int32_t mipHeight;
		VkImageSubresource subres;
		subres.arrayLayer = 0;
		subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_METADATA_BIT;
		subres.mipLevel = mipLevel;
		VkSubresourceLayout layout;
		vkGetImageSubresourceLayout(VkRenderDevice::dev, this->img, &subres, &layout);
		mipWidth = (int32_t)(layout.rowPitch / size);
		mipHeight = (int32_t)(layout.depthPitch / mipWidth);

		// the row pitch must be the size of one pixel times the number of pixels in width
		outMapInfo.mipWidth = mipWidth;
		outMapInfo.mipHeight = mipHeight;
		outMapInfo.rowPitch = (int32_t)layout.rowPitch;
		outMapInfo.depthPitch = (int32_t)layout.depthPitch;

		this->mappedData = Memory::Alloc(Memory::ObjectArrayHeap, (int32_t)layout.size);
		VkResult res = vkMapMemory(VkRenderDevice::dev, this->mem, layout.offset, (int32_t)layout.size, 0, &this->mappedData);

		outMapInfo.data = this->mappedData;
		retval = res == VK_SUCCESS;
	}
	else if (Texture3D == this->type)
	{
		VkFormat format = VkTypes::AsVkFormat(this->pixelFormat);
		uint32_t size = CoreGraphics::PixelFormat::ToSize(this->pixelFormat);

		int32_t mipWidth;
		int32_t mipHeight;
		int32_t mipDepth;
		VkImageSubresource subres;
		subres.arrayLayer = 0;
		subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subres.mipLevel = mipLevel;
		VkSubresourceLayout layout;
		vkGetImageSubresourceLayout(VkRenderDevice::dev, this->img, &subres, &layout);
		mipWidth = (int32_t)(layout.rowPitch / size);
		mipHeight = (int32_t)(layout.depthPitch / layout.rowPitch);
		mipDepth = (int32_t)(layout.arrayPitch / layout.depthPitch);

		// the row pitch must be the size of one pixel times the number of pixels in width
		outMapInfo.mipWidth = mipWidth;
		outMapInfo.mipHeight = mipHeight;
		outMapInfo.rowPitch = (int32_t)layout.rowPitch;
		outMapInfo.depthPitch = (int32_t)layout.depthPitch;

		this->mappedData = Memory::Alloc(Memory::ObjectArrayHeap, (int32_t)layout.size);
		VkResult res = vkMapMemory(VkRenderDevice::dev, this->mem, layout.offset, (int32_t)layout.size, 0, &this->mappedData);

		outMapInfo.data = this->mappedData;
		retval = res == VK_SUCCESS;
	}
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Unmap(IndexT mipLevel)
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
}

//------------------------------------------------------------------------------
/**
*/
bool
VkTexture::MapCubeFace(CubeFace face, IndexT mipLevel, MapType mapType, MapInfo& outMapInfo)
{
	bool retval = false;
	
	VkFormat format = VkTypes::AsVkFormat(this->pixelFormat);
	uint32_t size = CoreGraphics::PixelFormat::ToSize(this->pixelFormat);

	int32_t mipWidth;
	int32_t mipHeight;
	int32_t mipDepth;
	VkImageSubresource subres;
	subres.arrayLayer = (int32_t)face;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.mipLevel = mipLevel;
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(VkRenderDevice::dev, this->img, &subres, &layout);
	mipWidth = (int32_t)(layout.rowPitch / size);
	mipHeight = (int32_t)(layout.depthPitch / layout.rowPitch);
	mipDepth = (int32_t)(layout.size / layout.depthPitch);

	// the row pitch must be the size of one pixel times the number of pixels in width
	outMapInfo.mipWidth = mipWidth;
	outMapInfo.mipHeight = mipHeight;
	outMapInfo.rowPitch = (int32_t)layout.rowPitch;
	outMapInfo.depthPitch = (int32_t)layout.depthPitch;

	this->mappedData = Memory::Alloc(Memory::ObjectArrayHeap, (int32_t)layout.size);
	VkResult res = vkMapMemory(VkRenderDevice::dev, this->mem, layout.offset, (int32_t)layout.size, 0, &this->mappedData);

	outMapInfo.data = this->mappedData;
	retval = res == VK_SUCCESS;

	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::UnmapCubeFace(CubeFace face, IndexT mipLevel)
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::GenerateMipmaps()
{
	// implement some way to generate mipmaps here, perhaps a compute shader is the easiest...
	// hmm, perhaps we can use a sequence of subpasses here to create our mip chain
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Update(void* data, SizeT size, SizeT width, SizeT height, IndexT left, IndexT top, IndexT mip)
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture2D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkMultisampleTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture2D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkCubeTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = TextureCube;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkVolumeTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture3D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::MipDimensions(IndexT mip, IndexT face, SizeT& width, SizeT& height, SizeT& depth)
{
	n_assert(mip >= 0);
	n_assert(face >= 0);
	uint32_t size = CoreGraphics::PixelFormat::ToSize(this->pixelFormat);
	VkImageSubresource subres;
	subres.arrayLayer = (int32_t)face;
	subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subres.mipLevel = mip;
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(VkRenderDevice::dev, this->img, &subres, &layout);
	width = (int32_t)(layout.rowPitch / size);
	height = (int32_t)(layout.depthPitch / layout.rowPitch);
	depth = (int32_t)(layout.arrayPitch / layout.depthPitch);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Copy(const Ptr<CoreGraphics::Texture>& from, uint32_t fromMip, uint32_t fromLayer, uint32_t fromXOffset, uint32_t fromYOffset, uint32_t fromZOffset, 
				const Ptr<CoreGraphics::Texture>& to, uint32_t toMip, uint32_t toLayer, uint32_t toXOffset, uint32_t toYOffset, uint32_t toZOffset)
{
	// TODO: implement me
}

} // namespace Vulkan