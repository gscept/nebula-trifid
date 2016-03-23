#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan texture.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/texturebase.h"
namespace Vulkan
{
class VkTexture : public Base::TextureBase
{
	__DeclareClass(VkTexture);
public:
	/// constructor
	VkTexture();
	/// destructor
	virtual ~VkTexture();

	/// unload the resource, or cancel the pending load
	virtual void Unload();
	/// map a texture mip level for CPU access
	bool Map(IndexT mipLevel, MapType mapType, MapInfo& outMapInfo);
	/// unmap texture after CPU access
	void Unmap(IndexT mipLevel);
	/// map a cube map face for CPU access
	bool MapCubeFace(CubeFace face, IndexT mipLevel, MapType mapType, MapInfo& outMapInfo);
	/// unmap cube map face after CPU access
	void UnmapCubeFace(CubeFace face, IndexT mipLevel);
	/// generates mipmaps
	void GenerateMipmaps();

	/// updates texture region
	void Update(void* data, SizeT dataSize, SizeT width, SizeT height, IndexT left, IndexT top, IndexT mip);
	/// updates entire texture
	void Update(void* data, SizeT dataSize, IndexT mip);
	/// updates texture cube face region
	void UpdateArray(void* data, SizeT dataSize, SizeT width, SizeT height, IndexT left, IndexT top, IndexT mip, IndexT layer);
	/// updates texture cube face region
	void UpdateArray(void* data, SizeT dataSize, IndexT mip, IndexT layer);

	/// setup from an opengl 2D texture
	void SetupFromVkTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips = 0, const bool setLoaded = true, const bool isAttachment = false);
	/// setup from an opengl 2d multisample texture
	void SetupFromVkMultisampleTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips = 0, const bool setLoaded = true, const bool isAttachment = false);
	/// setup from an opengl texture cube
	void SetupFromVkCubeTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips = 0, const bool setLoaded = true, const bool isAttachment = false);
	/// setup from an opengl volume texture
	void SetupFromVkVolumeTexture(VkImage img, VkDeviceMemory mem, CoreGraphics::PixelFormat::Code format, GLint numMips = 0, const bool setLoaded = true, const bool isAttachment = false);

	/// calculate the size of a texture given a certain mip, use face 0 when not accessing a cube or array texture
	void MipDimensions(IndexT mip, IndexT face, SizeT& width, SizeT& height, SizeT& depth);
	/// copy from one texture to another
	static void Copy(const Ptr<CoreGraphics::Texture>& from, uint32_t fromMip, uint32_t fromLayer, uint32_t fromXOffset, uint32_t fromYOffset, uint32_t fromZOffset,
					 const Ptr<CoreGraphics::Texture>& to, uint32_t toMip, uint32_t toLayer, uint32_t toXOffset, uint32_t toYOffset, uint32_t toZOffset);
private:
	void* mappedData;
	uint32_t mapCount;
	VkImage img;
	VkDeviceMemory mem;
};
} // namespace Vulkan