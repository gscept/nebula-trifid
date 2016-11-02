#pragma once
//------------------------------------------------------------------------------
/**
	Fetch texture information using the Vulkan implementation.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "resources/streaming/resourceinfo.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/texture.h"
#include "coregraphics/vk/vktypes.h"
#include <IL/il.h>
namespace Vulkan
{
class VkTextureInfo : public Resources::ResourceInfo
{
public:
	/// constructor
	VkTextureInfo();
	/// destructor
	virtual ~VkTextureInfo();

	/// returns true if elements are equal
	virtual bool IsEqual(const ResourceInfo* info) const;
	/// returns true if elements are equal
	virtual bool IsEqual(const VkTextureInfo* info) const;

	/// get texture type
	CoreGraphics::Texture::Type GetType() const;
	/// get width of texture 
	uint GetWidth() const;
	/// get height of texture (if 2d or 3d texture)
	uint GetHeight() const;
	/// get depth of texture (if 3d texture)
	uint GetDepth() const;
	/// get number of mip levels
	uint GetMipLevels() const;
	/// get pixel format of the texture
	CoreGraphics::PixelFormat::Code GetPixelFormat() const;

	/// set info from IL image
	void SetInfo(ILint image);

	/// set texture type
	void SetType(CoreGraphics::Texture::Type t);
	/// set texture width
	void SetWidth(uint w);
	/// set texture height
	void SetHeight(uint h);
	/// set texture depth
	void SetDepth(uint d);
	/// set number of mip levels
	void SetMipLevels(uint n);
	/// set pixel format
	void SetPixelFormat(CoreGraphics::PixelFormat::Code f);

protected:
	uint width;
	uint height;
	uint depth;
	uint numMipLevels;
	CoreGraphics::PixelFormat::Code pixelFormat;
	CoreGraphics::Texture::Type textureType;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
VkTextureInfo::IsEqual(const VkTextureInfo* info) const
{
	return this->width == info->width &&
		this->height == info->height &&
		this->depth == info->depth &&
		this->numMipLevels == info->numMipLevels &&
		this->textureType == info->textureType &&
		this->pixelFormat == info->pixelFormat;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
VkTextureInfo::IsEqual(const ResourceInfo* info) const
{
	return this->IsEqual((const VkTextureInfo*)(info));
}


//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetType(CoreGraphics::Texture::Type t)
{
	this->textureType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::Texture::Type
VkTextureInfo::GetType() const
{
	return this->textureType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetWidth(uint w)
{
	this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
VkTextureInfo::GetWidth() const
{
	return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetHeight(uint h)
{
	this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
VkTextureInfo::GetHeight() const
{
	return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetDepth(uint d)
{
	this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
VkTextureInfo::GetDepth() const
{
	return this->depth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetMipLevels(uint n)
{
	this->numMipLevels = n;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
VkTextureInfo::GetMipLevels() const
{
	return this->numMipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureInfo::SetPixelFormat(CoreGraphics::PixelFormat::Code f)
{
	this->pixelFormat = f;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::PixelFormat::Code
VkTextureInfo::GetPixelFormat() const
{
	return this->pixelFormat;
}

} // namespace Vulkan