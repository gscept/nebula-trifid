#pragma once
//------------------------------------------------------------------------------
/**
	Implements a texture streamwer which can setup a Vulkan texture from stream.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "resources/streamresourceloader.h"
#include "coregraphics/texture.h"

namespace Vulkan
{
class VkTextureStreamer : public Resources::StreamResourceLoader
{
	__DeclareClass(VkTextureStreamer);
public:
	/// constructor
	VkTextureStreamer();
	/// destructor
	virtual ~VkTextureStreamer();

	/// setup the texture from a Nebula3 stream
	virtual bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
	/// sets the texture to reuse on load
	void SetReuseTexture(const Ptr<CoreGraphics::Texture>& tex);
	/// get the texture to reuse on load (may return 0!)
	const Ptr<CoreGraphics::Texture>& GetReuseTexture() const;
	/// resets loader-stats e.g. state and reuseTexture (does not cut connection to Resource!)
	virtual void Reset();
	/// called by resource when a load is requested
	virtual bool OnLoadRequested();
protected:
	/// setup a 2D texture from a Nebula3 stream
	virtual bool SetupTexture2DFromStream(const Ptr<IO::Stream>& stream);
	/// setup a cube texture from a Nebula3 stream
	virtual bool SetupTextureCubeFromStream(const Ptr<IO::Stream>& stream);
	/// copies mips from reuseTexture to targeted texture
	virtual bool ReuseMips();

	/// calculates surface information depending on given width, height and pixel format
	void GetSurfaceInfo(uint width, uint height, CoreGraphics::PixelFormat::Code fmt, uint* pNumBytes, uint* pRowBytes, uint* pNumRows) const;

	Ptr<CoreGraphics::Texture> reuseTexture;

#define ENABLE_LOAD_TIMERS (0)
#if ENABLE_LOAD_TIMERS
	_declare_timer(LoadingTimerTotal);
	_declare_timer(LoadingTimer1);
	_declare_timer(LoadingTimer2);
	_declare_timer(LoadingTimer3);
	_declare_timer(LoadingTimerCopy);
#endif
};


//------------------------------------------------------------------------------
/**
*/
inline void
VkTextureStreamer::SetReuseTexture(const Ptr<CoreGraphics::Texture>& tex)
{
	n_assert(!this->reuseTexture.isvalid());
	this->reuseTexture = tex;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
VkTextureStreamer::GetReuseTexture() const
{
	return this->reuseTexture;
}
} // namespace Vulkan