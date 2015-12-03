#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::RenderTargetBase

    Base class for render targets. A render targets wraps up to 4 color buffers
    and an optional depth/stencil buffer into a C++ object. The special
    default render target represents the backbuffer and default depth/stencil
    surface.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "coregraphics/antialiasquality.h"
#include "coregraphics/texture.h"
#include "coregraphics/framebatchtype.h"
#include "coregraphics/depthstenciltarget.h"
#include "math/rectangle.h"

namespace CoreGraphics
{
class RenderTarget;
}

//------------------------------------------------------------------------------
namespace Base
{
class RenderTargetBase : public Core::RefCounted
{
    __DeclareClass(RenderTargetBase);
public:

    /// clear flags
    enum ClearFlag
    {
        ClearColor = (1<<0)
    };

    /// constructor
    RenderTargetBase();
    /// destructor
    virtual ~RenderTargetBase();
    
    /// return true if valid (has been setup)
    bool IsValid() const;

    /// set to true if default render target (only called by RenderDevice)
    void SetDefaultRenderTarget(bool b);
    /// get default render target flag
    bool IsDefaultRenderTarget() const;
    /// set render target width
    void SetWidth(SizeT w);
    /// get width of render target in pixels
    SizeT GetWidth() const;
    /// set render target height
    void SetHeight(SizeT h);
    /// get height of render target in pixels
    SizeT GetHeight() const;
	/// set render target relative width
	void SetRelativeWidth(float w);
	/// get render target relative width
	float GetRelativeWidth() const;
	/// set render target relative height
	void SetRelativeHeight(float h);
	/// get render target relative height
	float GetRelativeHeight() const;
    /// set antialias quality
    void SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::Code c);
    /// get anti-alias-quality
    CoreGraphics::AntiAliasQuality::Code GetAntiAliasQuality() const;
    /// add a color buffer
    void SetColorBufferFormat(CoreGraphics::PixelFormat::Code colorFormat);
    /// get color buffer format at index
    CoreGraphics::PixelFormat::Code GetColorBufferFormat() const;
    /// use external depth-stencil buffer
	void SetDepthStencilTarget(const Ptr<CoreGraphics::DepthStencilTarget>& dt);
	/// get external depth-stencil buffe
	const Ptr<CoreGraphics::DepthStencilTarget>& GetDepthStencilTarget() const;
    /// return true if the render target has a depth/stencil buffer
    bool HasDepthStencilBuffer() const;
	/// set number of mipmaps
	void SetMipMaps(SizeT mips);
	/// get number of mipmaps
	const SizeT GetMipMaps() const;
    /// get mipmap generation flag
    bool AreMipMapsEnabled() const;
    /// set resolve texture resource id
    void SetResolveTextureResourceId(const Resources::ResourceId& resId);
    /// get resolve texture resource id
    const Resources::ResourceId& GetResolveTextureResourceId() const;
    /// set resolve texture width
    void SetResolveTextureWidth(SizeT w);
    /// get resolve texture width
    SizeT GetResolveTextureWidth() const;
    /// set resolve texture height
    void SetResolveTextureHeight(SizeT h);
    /// get resolve texture height
    SizeT GetResolveTextureHeight() const;    
    /// set cpu access flag
    void SetResolveTargetCpuAccess(bool b);
    /// get cpu access flag
    bool GetResolveTargetCpuAccess() const;
    /// set optional MRT (Multiple Render Target) index, default is 0
    void SetMRTIndex(IndexT i);
    /// get multiple-render-target index
    IndexT GetMRTIndex() const;

    /// set clear flags
    void SetClearFlags(uint clearFlags);
    /// get clear flags
    uint GetClearFlags() const;
    /// set clear color 
    void SetClearColor(const Math::float4& c);
    /// get clear color
    const Math::float4& GetClearColor() const;
	/// set clear depth
	void SetClearDepth(float f);
	/// get clear depth
	float GetClearDepth() const;
	/// set clear stencil
	void SetClearStencil(int i);
	/// get clear stencil
	int GetClearStencil() const;

    /// set the current resolve rectangle (in pixels)
    void SetResolveRect(const Math::rectangle<int>& r);
    /// get resolve rectangle
    const Math::rectangle<int>& GetResolveRect() const;
	/// returns true if resolve rect is valid
	const bool IsResolveRectValid() const;
	/// set array of resolve rects
	void SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects);
	/// get array of resolve rects
	const Util::Array<Math::rectangle<int> >& GetResolveRectArray() const;
	/// returns true if resolve rect array is valid
	const bool IsResolveRectArrayValid() const;
	/// remove resolve rects from render target
	void ResetResolveRects();

	/// called after we change the display size
	void OnDisplayResized(SizeT width, SizeT height);

    /// setup the render target object
    void Setup();
    /// discard the render target object
    void Discard();
    /// begin rendering to the render target
    void BeginPass();
    /// begin a batch
    void BeginBatch(CoreGraphics::FrameBatchType::Code batchType);
    /// end current batch
    void EndBatch();
    /// end current render pass
    void EndPass();
    /// generate mipmap levels
    void GenerateMipLevels();
    /// return true if resolve texture is valid
    bool HasResolveTexture() const;
    /// get the resolve texture as Nebula texture object
    const Ptr<CoreGraphics::Texture>& GetResolveTexture() const;  
    /// return true if cpu access resolve texture is valid
    bool HasCPUResolveTexture() const;
    /// get the resolve texture as Nebula texture object
    const Ptr<CoreGraphics::Texture>& GetCPUResolveTexture() const;  
    /// resolve depth buffer
    virtual void ResolveDepthBuffer();

    /// get byte size in memory, implemented in platform specific classes
    SizeT GetMemorySize() const;
    /// set optional memory offset, not used by all platforms
    void SetMemoryOffset(SizeT size);

    /// copy from this render target to the target texture
    void Copy(const Ptr<CoreGraphics::RenderTarget>& tex);

	/// force-clears render target, useful if clearing is required outside a frame shader
	virtual void Clear(uint flags);

protected:
    friend class RenderDeviceBase;

    Ptr<CoreGraphics::DepthStencilTarget> depthStencilTarget;   // target which provides shared depth-stencil buffer
    Resources::ResourceId resolveTextureResId;
    CoreGraphics::FrameBatchType::Code batchType;
	bool relativeSizeValid;
    SizeT width;
    SizeT height;
	float relWidth;
	float relHeight;
    bool resolveTextureDimensionsValid;
    bool resolveRectValid;
	bool resolveRectArrayValid;
    SizeT resolveTextureWidth;
    SizeT resolveTextureHeight;
    Math::rectangle<int> resolveRect;
	Util::Array<Math::rectangle<int> > resolveRectArray;
    IndexT mrtIndex;
    uint clearFlags;
    Math::float4 clearColor;
	float clearDepth;
	int clearStencil;
    CoreGraphics::AntiAliasQuality::Code antiAliasQuality;
    CoreGraphics::PixelFormat::Code colorBufferFormat;
    Ptr<CoreGraphics::Texture> resolveTexture;
    Ptr<CoreGraphics::Texture> resolveCPUTexture;
    Ptr<CoreGraphics::Texture> resolveDepthTexture;
	SizeT mipLevels;
    bool mipMapsEnabled;
    bool isValid;
    bool inBeginPass;
    bool inBeginBatch;
    bool isDefaultRenderTarget;
    bool resolveCpuAccess;
	
    SizeT memoryOffset;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetDefaultRenderTarget(bool b)
{
    this->isDefaultRenderTarget = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::IsDefaultRenderTarget() const
{
    return this->isDefaultRenderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetWidth(SizeT w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetBase::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetHeight(SizeT h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetBase::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetRelativeWidth( float w )
{
	this->relWidth = w;
	this->relativeSizeValid = true;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
RenderTargetBase::GetRelativeWidth() const
{
	return this->relWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetRelativeHeight( float h )
{
	this->relHeight = h;
	this->relativeSizeValid = true;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
RenderTargetBase::GetRelativeHeight() const
{
	return this->relHeight;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::Code aaq)
{
    this->antiAliasQuality = aaq;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::AntiAliasQuality::Code
RenderTargetBase::GetAntiAliasQuality() const
{
    return this->antiAliasQuality;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetColorBufferFormat(CoreGraphics::PixelFormat::Code colorFormat)
{
    this->colorBufferFormat = colorFormat;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::PixelFormat::Code
RenderTargetBase::GetColorBufferFormat() const
{
    return this->colorBufferFormat;
}


//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetDepthStencilTarget( const Ptr<CoreGraphics::DepthStencilTarget>& dt )
{
	n_assert(dt.isvalid());
	this->depthStencilTarget = dt;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::DepthStencilTarget>& 
RenderTargetBase::GetDepthStencilTarget() const
{
	return this->depthStencilTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderTargetBase::HasDepthStencilBuffer() const
{
	return this->depthStencilTarget.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetMipMaps( SizeT mips )
{
	this->mipLevels = mips;
	this->mipMapsEnabled = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
RenderTargetBase::GetMipMaps() const
{
	return this->mipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetClearColor(const Math::float4& c)
{
    this->clearColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
RenderTargetBase::GetClearColor() const
{
    return this->clearColor;
}
//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetClearDepth( float f )
{
	this->clearDepth = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
RenderTargetBase::GetClearDepth() const
{
	return this->clearDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetClearStencil( int i )
{
	this->clearStencil = i;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
RenderTargetBase::GetClearStencil() const
{
	return this->clearStencil;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetResolveTextureResourceId(const Resources::ResourceId& resId)
{
    this->resolveTextureResId = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
RenderTargetBase::GetResolveTextureResourceId() const
{
    return this->resolveTextureResId;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::HasResolveTexture() const
{
    return this->resolveTexture.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
RenderTargetBase::GetResolveTexture() const
{
    n_assert(this->resolveTexture.isvalid());
    return this->resolveTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::HasCPUResolveTexture() const
{
    return this->resolveCPUTexture.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
RenderTargetBase::GetCPUResolveTexture() const
{
    n_assert(this->resolveCPUTexture.isvalid());
    return this->resolveCPUTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::AreMipMapsEnabled() const
{
    return this->mipMapsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetResolveTextureWidth(SizeT w)
{
    this->resolveTextureWidth = w;
    this->resolveTextureDimensionsValid = true;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetBase::GetResolveTextureWidth() const
{
    return this->resolveTextureWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetResolveTextureHeight(SizeT h)
{
    this->resolveTextureHeight = h;
    this->resolveTextureDimensionsValid = true;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetBase::GetResolveTextureHeight() const
{
    return this->resolveTextureHeight;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetResolveRect(const Math::rectangle<int>& r)
{
    this->resolveRectValid = true;
	this->resolveRectArrayValid = false;
	this->resolveRectArray.Clear();
    this->resolveRect = r;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::rectangle<int>&
RenderTargetBase::GetResolveRect() const
{
    return this->resolveRect;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
RenderTargetBase::IsResolveRectValid() const
{
	return this->resolveRectValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetBase::SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects)
{
	this->resolveRectArray = rects;
	this->resolveRectArrayValid = true;
	this->resolveRectValid = false;
	this->resolveRect = Math::rectangle<int>();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Math::rectangle<int> >& 
RenderTargetBase::GetResolveRectArray() const
{
	return this->resolveRectArray;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
RenderTargetBase::IsResolveRectArrayValid() const
{
	return this->resolveRectArrayValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetResolveTargetCpuAccess(bool b)
{
    this->resolveCpuAccess = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetBase::GetResolveTargetCpuAccess() const
{
    return this->resolveCpuAccess;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetMRTIndex(IndexT i)
{
    this->mrtIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
RenderTargetBase::GetMRTIndex() const
{
    return this->mrtIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetBase::SetClearFlags(uint f)
{
    this->clearFlags = f;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
RenderTargetBase::GetClearFlags() const
{
    return this->clearFlags;
}

} // namespace Base    
//------------------------------------------------------------------------------


