#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::OGL4TextureStreamer
  
    Resource loader for loading texture data from a Nebula3 stream. Supports
    synchronous and asynchronous loading. Specialized for DirectX 9.
    
    (C) 2010 Radon Labs GmbH
*/    
#include "resources/streamresourceloader.h"
#include "debug/debugtimer.h"
#include "coregraphics/texture.h"

namespace Resources
{
#define MAX_DEFAULT_MIPS 36

class OGL4TextureStreamer : public Resources::StreamResourceLoader
{
    __DeclareClass(OGL4TextureStreamer);

public:
	/// constructor
    OGL4TextureStreamer();
	//destructor
    ~OGL4TextureStreamer();

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
    void GetSurfaceInfo(uint width, uint height, GLenum fmt, uint* pNumBytes, uint* pRowBytes, uint* pNumRows) const;
    /// returns appropriate bit-per-pixel-count of given pixel format by looking it up in a table
    uint BitsPerPixel(GLenum fmt) const;

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
OGL4TextureStreamer::SetReuseTexture(const Ptr<CoreGraphics::Texture>& tex)
{
    n_assert(!this->reuseTexture.isvalid());
    this->reuseTexture = tex;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
OGL4TextureStreamer::GetReuseTexture() const
{
    return this->reuseTexture;
}
} // namespace Resources
//------------------------------------------------------------------------------


