//------------------------------------------------------------------------------
// vktexture.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vktexture.h"
#include "vkrenderdevice.h"
#include "coregraphics/pixelformat.h"
#include "vktypes.h"
#include "coregraphics/renderdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkTexture, 'VKTX', Base::TextureBase);
//------------------------------------------------------------------------------
/**
*/
VkTexture::VkTexture() :
	mappedBuf(VK_NULL_HANDLE),
	mappedMem(VK_NULL_HANDLE)
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
	n_assert(this->mapCount == 0);
	vkDestroyImage(VkRenderDevice::dev, this->img, NULL);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
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

		this->mappedBufferLayout.bufferOffset = 0;
		this->mappedBufferLayout.bufferImageHeight = this->GetHeight();
		this->mappedBufferLayout.bufferRowLength = size * this->GetWidth();
		this->mappedBufferLayout.imageExtent = { this->width, this->height, 1 };
		this->mappedBufferLayout.imageOffset = { 0, 0, 0 };
		this->mappedBufferLayout.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, mipLevel };
		uint32_t memSize;
		CoreGraphics::RenderDevice::Instance()->ReadImage(this->img, this->mappedBufferLayout, memSize, this->mappedMem, this->mappedBuf);

		int32_t mipWidth = (int32_t)Math::n_max(1.0f, Math::n_floor(this->width / Math::n_pow(2, (float)mipLevel)));
		int32_t mipHeight = (int32_t)Math::n_max(1.0f, Math::n_floor(this->height / Math::n_pow(2, (float)mipLevel)));

		// the row pitch must be the size of one pixel times the number of pixels in width
		outMapInfo.mipWidth = mipWidth;
		outMapInfo.mipHeight = mipHeight;
		outMapInfo.rowPitch = (int32_t)memSize / mipWidth;
		outMapInfo.depthPitch = (int32_t)memSize / (mipWidth * mipHeight);
		VkResult res = vkMapMemory(VkRenderDevice::dev, this->mappedMem, 0, (int32_t)memSize, 0, &this->mappedData);
		n_assert(res == VK_SUCCESS);

		outMapInfo.data = this->mappedData;
		retval = res == VK_SUCCESS;
		this->mapCount++;
	}
	else if (Texture3D == this->type)
	{
		VkFormat format = VkTypes::AsVkFormat(this->pixelFormat);
		uint32_t size = CoreGraphics::PixelFormat::ToSize(this->pixelFormat);

		this->mappedBufferLayout.bufferOffset = 0;
		this->mappedBufferLayout.bufferImageHeight = this->GetHeight();
		this->mappedBufferLayout.bufferRowLength = size * this->GetWidth();
		this->mappedBufferLayout.imageExtent = { this->width, this->height, this->depth };
		this->mappedBufferLayout.imageOffset = { 0, 0, 0 };
		this->mappedBufferLayout.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, mipLevel };
		uint32_t memSize;
		CoreGraphics::RenderDevice::Instance()->ReadImage(this->img, this->mappedBufferLayout, memSize, this->mappedMem, this->mappedBuf);

		int32_t mipWidth = (int32_t)Math::n_max(1.0f, Math::n_floor(this->width / Math::n_pow(2, (float)mipLevel)));
		int32_t mipHeight = (int32_t)Math::n_max(1.0f, Math::n_floor(this->height / Math::n_pow(2, (float)mipLevel)));
		int32_t mipDepth = (int32_t)Math::n_max(1.0f, Math::n_floor(this->depth / Math::n_pow(2, (float)mipLevel)));

		// the row pitch must be the size of one pixel times the number of pixels in width
		outMapInfo.mipWidth = mipWidth;
		outMapInfo.mipHeight = mipHeight;
		outMapInfo.rowPitch = (int32_t)memSize / mipWidth;
		outMapInfo.depthPitch = (int32_t)memSize / (mipWidth * mipHeight);
		VkResult res = vkMapMemory(VkRenderDevice::dev, this->mappedMem, 0, (int32_t)memSize, 0, &this->mappedData);
		n_assert(res == VK_SUCCESS);

		outMapInfo.data = this->mappedData;
		retval = res == VK_SUCCESS;
		this->mapCount++;
	}
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Unmap(IndexT mipLevel)
{
	n_assert(this->mapCount > 0);

	// unmap and dealloc
	vkUnmapMemory(VkRenderDevice::dev, this->mappedMem);
	VkRenderDevice::Instance()->WriteImage(this->mappedBuf, this->img, this->mappedBufferLayout);
	this->mapCount--;

	if (this->mapCount == 0)
	{
		vkFreeMemory(VkRenderDevice::dev, this->mappedMem, NULL);
		vkDestroyBuffer(VkRenderDevice::dev, this->mappedBuf, NULL);

		this->mappedData = 0;
		this->mappedBuf = VK_NULL_HANDLE;
		this->mappedMem = VK_NULL_HANDLE;
		this->mappedBufferLayout = VkBufferImageCopy();
	}
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

	this->mappedBufferLayout.bufferOffset = 0;
	this->mappedBufferLayout.bufferImageHeight = this->GetHeight();
	this->mappedBufferLayout.bufferRowLength = size * this->GetWidth();
	this->mappedBufferLayout.imageExtent = { this->width, this->height, 1 };
	this->mappedBufferLayout.imageOffset = { 0, 0, 0 };
	this->mappedBufferLayout.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, (uint32_t)face, 1, mipLevel };
	uint32_t memSize;
	CoreGraphics::RenderDevice::Instance()->ReadImage(this->img, this->mappedBufferLayout, memSize, this->mappedMem, this->mappedBuf);

	int32_t mipWidth = (int32_t)Math::n_max(1.0f, Math::n_floor(this->width / Math::n_pow(2, (float)mipLevel)));
	int32_t mipHeight = (int32_t)Math::n_max(1.0f, Math::n_floor(this->height / Math::n_pow(2, (float)mipLevel)));
	int32_t mipDepth = (int32_t)Math::n_max(1.0f, Math::n_floor(this->depth / Math::n_pow(2, (float)mipLevel)));

	// the row pitch must be the size of one pixel times the number of pixels in width
	outMapInfo.mipWidth = mipWidth;
	outMapInfo.mipHeight = mipHeight;
	outMapInfo.rowPitch = (int32_t)memSize / mipWidth;
	outMapInfo.depthPitch = (int32_t)memSize / (mipWidth * mipHeight);
	VkResult res = vkMapMemory(VkRenderDevice::dev, this->mappedMem, 0, (int32_t)memSize, 0, &this->mappedData);
	n_assert(res == VK_SUCCESS);

	outMapInfo.data = this->mappedData;
	retval = res == VK_SUCCESS;
	this->mapCount++;

	return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::UnmapCubeFace(CubeFace face, IndexT mipLevel)
{
	n_assert(this->mapCount > 0);
	// unmap and dealloc
	vkUnmapMemory(VkRenderDevice::dev, this->mappedMem);
	VkRenderDevice::Instance()->WriteImage(this->mappedBuf, this->img, this->mappedBufferLayout);
	this->mapCount--;

	if (this->mapCount == 0)
	{
		vkFreeMemory(VkRenderDevice::dev, this->mappedMem, NULL);
		vkDestroyBuffer(VkRenderDevice::dev, this->mappedBuf, NULL);

		this->mappedData = 0;
		this->mappedBuf = VK_NULL_HANDLE;
		this->mappedMem = VK_NULL_HANDLE;
		this->mappedBufferLayout = VkBufferImageCopy();
	}	
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
VkTexture::Update(void* data, SizeT dataSize, SizeT width, SizeT height, IndexT left, IndexT top, IndexT mip)
{
	n_assert(this->img != VK_NULL_HANDLE);
	n_assert(this->mem != VK_NULL_HANDLE);

	VkBufferImageCopy copy;
	copy.imageExtent.width = width;
	copy.imageExtent.height = height;
	copy.imageExtent.depth = 1;			// hmm, might want this for cube maps and volume textures too
	copy.imageOffset.x = left;
	copy.imageOffset.y = top;
	copy.imageOffset.z = 0;
	copy.imageSubresource.mipLevel = mip;
	copy.imageSubresource.layerCount = 1;
	copy.imageSubresource.baseArrayLayer = 0;
	copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.bufferOffset = 0;
	copy.bufferRowLength = dataSize / width;
	copy.bufferImageHeight = height;

	// push update
	VkRenderDevice::Instance()->PushImageUpdate(this->img, copy, dataSize, (uint32_t*)data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Update(void* data, SizeT dataSize, IndexT mip)
{
	n_assert(this->img != VK_NULL_HANDLE);
	n_assert(this->mem != VK_NULL_HANDLE);

	VkBufferImageCopy copy;
	copy.imageExtent.width = this->width;
	copy.imageExtent.height = this->height;
	copy.imageExtent.depth = 1;			// hmm, might want this for cube maps and volume textures too
	copy.imageOffset.x = 0;
	copy.imageOffset.y = 0;
	copy.imageOffset.z = 0;
	copy.imageSubresource.mipLevel = mip;
	copy.imageSubresource.layerCount = 1;
	copy.imageSubresource.baseArrayLayer = 0;
	copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.bufferOffset = 0;
	copy.bufferRowLength = dataSize / this->width;
	copy.bufferImageHeight = this->height;

	// push update
	VkRenderDevice::Instance()->PushImageUpdate(this->img, copy, dataSize, (uint32_t*)data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::UpdateArray(void* data, SizeT dataSize, SizeT width, SizeT height, IndexT left, IndexT top, IndexT mip, IndexT layer)
{
	n_assert(this->img != VK_NULL_HANDLE);
	n_assert(this->mem != VK_NULL_HANDLE);

	VkBufferImageCopy copy;
	copy.imageExtent.width = width;
	copy.imageExtent.height = height;
	copy.imageExtent.depth = 1;			// hmm, might want this for cube maps and volume textures too
	copy.imageOffset.x = left;
	copy.imageOffset.y = top;
	copy.imageOffset.z = 0;
	copy.imageSubresource.mipLevel = mip;
	copy.imageSubresource.layerCount = 1;
	copy.imageSubresource.baseArrayLayer = layer;
	copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.bufferOffset = 0;
	copy.bufferRowLength = dataSize / width;
	copy.bufferImageHeight = height;

	// push update
	VkRenderDevice::Instance()->PushImageUpdate(this->img, copy, dataSize, (uint32_t*)data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::UpdateArray(void* data, SizeT dataSize, IndexT mip, IndexT layer)
{
	n_assert(this->img != VK_NULL_HANDLE);
	n_assert(this->mem != VK_NULL_HANDLE);

	VkBufferImageCopy copy;
	copy.imageExtent.width = this->width;
	copy.imageExtent.height = this->height;
	copy.imageExtent.depth = 1;			// hmm, might want this for cube maps and volume textures too
	copy.imageOffset.x = 0;
	copy.imageOffset.y = 0;
	copy.imageOffset.z = 0;
	copy.imageSubresource.mipLevel = mip;
	copy.imageSubresource.layerCount = 1;
	copy.imageSubresource.baseArrayLayer = layer;
	copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.bufferOffset = 0;
	copy.bufferRowLength = dataSize / this->width;
	copy.bufferImageHeight = this->height;

	// push update
	VkRenderDevice::Instance()->PushImageUpdate(this->img, copy, dataSize, (uint32_t*)data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkTexture(VkImage img, VkDeviceMemory mem, VkImageView imgView, uint32_t width, uint32_t height, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture2D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
	this->imgView = imgView;

	this->SetType(VkTexture::Texture2D);
	this->SetWidth(width);
	this->SetHeight(height);
	this->SetDepth(1);
	this->SetNumMipLevels(Math::n_max(1, numMips));
	this->SetPixelFormat(format);
	this->access = ResourceBase::AccessRead;
	this->isRenderTargetAttachment = isAttachment;
	if (setLoaded)
	{
		this->SetState(Resource::Loaded);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkMultisampleTexture(VkImage img, VkDeviceMemory mem, VkImageView imgView, uint32_t width, uint32_t height, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture2D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
	this->imgView = imgView;

	this->SetType(VkTexture::Texture2D);
	this->SetWidth(width);
	this->SetHeight(height);
	this->SetDepth(1);
	this->SetNumMipLevels(Math::n_max(1, numMips));
	this->SetPixelFormat(format);
	this->access = ResourceBase::AccessRead;
	this->isRenderTargetAttachment = isAttachment;
	if (setLoaded)
	{
		this->SetState(Resource::Loaded);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkCubeTexture(VkImage img, VkDeviceMemory mem, VkImageView imgView, uint32_t width, uint32_t height, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = TextureCube;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
	this->imgView = imgView;

	this->SetType(VkTexture::TextureCube);
	this->SetWidth(width);
	this->SetHeight(height);
	this->SetDepth(1);
	this->SetNumMipLevels(Math::n_max(1, numMips));
	this->SetPixelFormat(format);
	this->access = ResourceBase::AccessRead;
	this->isRenderTargetAttachment = isAttachment;
	if (setLoaded)
	{
		this->SetState(Resource::Loaded);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::SetupFromVkVolumeTexture(VkImage img, VkDeviceMemory mem, VkImageView imgView, uint32_t width, uint32_t height, uint32_t depth, CoreGraphics::PixelFormat::Code format, GLint numMips /*= 0*/, const bool setLoaded /*= true*/, const bool isAttachment /*= false*/)
{
	this->type = Texture3D;
	this->pixelFormat = format;
	this->img = img;
	this->numMipLevels = numMips;
	this->mem = mem;
	this->imgView = imgView;

	this->SetType(VkTexture::Texture3D);
	this->SetWidth(width);
	this->SetHeight(height);
	this->SetDepth(depth);
	this->SetNumMipLevels(Math::n_max(1, numMips));
	this->SetPixelFormat(format);
	this->access = ResourceBase::AccessRead;
	this->isRenderTargetAttachment = isAttachment;
	if (setLoaded)
	{
		this->SetState(Resource::Loaded);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkTexture::Copy(const Ptr<CoreGraphics::Texture>& from, const Ptr<CoreGraphics::Texture>& to, uint32_t width, uint32_t height, uint32_t depth,
	uint32_t srcMip, uint32_t srcLayer, uint32_t srcXOffset, uint32_t srcYOffset, uint32_t srcZOffset,
	uint32_t dstMip, uint32_t dstLayer, uint32_t dstXOffset, uint32_t dstYOffset, uint32_t dstZOffset)
{
	VkImageCopy copy;
	copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.srcSubresource.baseArrayLayer = srcLayer;
	copy.srcSubresource.layerCount = 1;
	copy.srcSubresource.mipLevel = srcMip;
	copy.srcOffset = { srcXOffset, srcYOffset, srcZOffset };

	copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.dstSubresource.baseArrayLayer = dstLayer;
	copy.dstSubresource.layerCount = 1;
	copy.dstSubresource.mipLevel = dstMip;
	copy.dstOffset = { dstXOffset, dstYOffset, dstZOffset };
	
	copy.extent = { width, height, depth };

	// begin immediate action, this might actually be delayed but we can't really know from here
	VkCommandBuffer cmdBuf = VkRenderDevice::Instance()->BeginImmediateTransfer();
	vkCmdCopyImage(cmdBuf, from->GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, to->GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, 1, &copy);
	VkRenderDevice::Instance()->EndImmediateTransfer(cmdBuf);
}

} // namespace Vulkan