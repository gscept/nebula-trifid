//------------------------------------------------------------------------------
// vkstreamtextureloader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkstreamtextureloader.h"
#include "coregraphics/texture.h"
#include "coregraphics/renderdevice.h"
#include "io/ioserver.h"
#include "coregraphics/vk/vktypes.h"
#include "IL/il.h"

#include <vulkan/vulkan.h>
#include "vkrenderdevice.h"
namespace Vulkan
{

__ImplementClass(Vulkan::VkStreamTextureLoader, 'VKTL', Resources::StreamResourceLoader);

using namespace CoreGraphics;
using namespace Resources;
using namespace IO;
//------------------------------------------------------------------------------
/**
*/
VkStreamTextureLoader::VkStreamTextureLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkStreamTextureLoader::~VkStreamTextureLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
VkStreamTextureLoader::SetupResourceFromStream(const Ptr<IO::Stream>& stream)
{
	n_assert(stream.isvalid());
	n_assert(stream->CanBeMapped());
	n_assert(this->resource->IsA(Texture::RTTI));
	const Ptr<Texture>& res = this->resource.downcast<Texture>();
	n_assert(!res->IsLoaded());

	stream->SetAccessMode(Stream::ReadAccess);
	if (stream->Open())
	{
		void* srcData = stream->Map();
		uint srcDataSize = stream->GetSize();

		// load using IL
		ILuint image = ilGenImage();
		ilBindImage(image);
		ilSetInteger(IL_DXTC_NO_DECOMPRESS, IL_TRUE);
		ilLoadL(IL_DDS, srcData, srcDataSize);

		ILuint width = ilGetInteger(IL_IMAGE_WIDTH);
		ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);
		ILuint depth = ilGetInteger(IL_IMAGE_DEPTH);
		ILuint mips = ilGetInteger(IL_NUM_MIPMAPS);
		ILenum cube = ilGetInteger(IL_IMAGE_CUBEFLAGS);
		ILenum format = ilGetInteger(IL_PIXEL_FORMAT);	// only available when loading DDS, so this might need some work...

		VkFormat vkformat = VkTypes::AsVkFormat(format);
		VkExtent3D extents;
		extents.width = width;
		extents.height = height;
		extents.depth = 1;
		VkImageCreateInfo info =
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			NULL,
			0,
			height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D,
			vkformat,
			extents,
			mips,
			depth,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_SHARING_MODE_CONCURRENT,
			1,
			&VkRenderDevice::Instance()->renderQueueIdx,
			VK_IMAGE_LAYOUT_GENERAL
		};
		VkImage img;
		VkResult stat = vkCreateImage(VkRenderDevice::dev, &info, NULL, &img);
		n_assert(stat == VK_SUCCESS);

		// setup texture
		if (depth > 1)
		{
			if (cube)	res->SetupFromVkCubeTexture(img, VkTypes::AsNebulaPixelFormat(vkformat), mips);
			else		res->SetupFromVkVolumeTexture(img, VkTypes::AsNebulaPixelFormat(vkformat), mips);
		}
		else
		{
			res->SetupFromVkTexture(img, VkTypes::AsNebulaPixelFormat(vkformat), mips);
		}

		// now load texture by walking through all images and mips
		ILuint i;
		ILuint j;
		
		for (i = 0; i < depth; i++)
		{
			ilActiveImage(i);
			ilActiveMipmap(0);
			for (j = 0; j < mips; j++)
			{
				ILuint size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
				ILubyte* buf = ilGetData();

				VkDeviceMemory mem;
				uint32_t alignedSize;
				VkRenderDevice::Instance()->AllocateImageMemory(img, mem, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, alignedSize);
				vkBindImageMemory(VkRenderDevice::dev, img, mem, 0);
				VkTexture::MapInfo mapinfo;
				res->Map(j, VkTexture::MapWrite, mapinfo);
				memcpy(mapinfo.data, buf, size);
				res->Unmap(j);

				// move to next mip
				ilActiveMipmap(j);
			}
		}

		// create view
		VkImageViewType viewType;
		if (cube) viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		else
		{
			if (height > 1)
			{
				if (depth > 1) viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				else		   viewType = VK_IMAGE_VIEW_TYPE_2D;
			}
			else
			{
				if (depth > 1) viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				else		   viewType = VK_IMAGE_VIEW_TYPE_1D;
			}
		}

		VkImageSubresourceRange viewRange;
		viewRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewRange.baseMipLevel = 0;
		viewRange.levelCount = mips;
		viewRange.baseArrayLayer = 0;
		viewRange.layerCount = depth;
		VkImageViewCreateInfo viewCreate =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			NULL,
			0,
			img,
			viewType,
			vkformat,
			VkTypes::AsVkMapping(format),
			viewRange
		};
		VkImageView view;
		stat = vkCreateImageView(VkRenderDevice::dev, &viewCreate, NULL, &view);
		n_assert(stat == VK_SUCCESS);

		stream->Unmap();
		stream->Close();
		return true;

	}

	return false;
}

} // namespace Vulkan