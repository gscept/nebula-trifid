#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan implementation of a render texture
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/base/rendertexturebase.h"
namespace Vulkan
{
class VkRenderTexture : public Base::RenderTextureBase
{
	__DeclareClass(VkRenderTexture);
public:
	/// constructor
	VkRenderTexture();
	/// destructor
	virtual ~VkRenderTexture();

	/// setup render texture
	void Setup();
	/// discard texture
	void Discard();
	/// resize render texture, retaining the same texture object
	void Resize();

	/// swap buffers, only valid if this is a window texture
	void SwapBuffers();

	/// get image
	const VkImage& GetVkImage() const;
	/// get image view
	const VkImageView& GetVkImageView() const;
	/// get memory
	const VkDeviceMemory& GetVkMemory() const;
private:
	VkImage img;
	VkImageView view;
	VkDeviceMemory mem;

	Util::FixedArray<VkImage> swapimages;
};

//------------------------------------------------------------------------------
/**
*/
inline const VkImage&
VkRenderTexture::GetVkImage() const
{
	return this->img;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkImageView&
VkRenderTexture::GetVkImageView() const
{
	return this->view;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkDeviceMemory&
VkRenderTexture::GetVkMemory() const
{
	return this->mem;
}

} // namespace Vulkan