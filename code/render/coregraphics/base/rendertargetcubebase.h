#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::RenderTargetBase

    Base class for render targets. A render targets wraps up to 4 color buffers
    and an optional depth/stencil buffer into a C++ object. The special
    default render target represents the backbuffer and default depth/stencil
    surface.

	If created layered, it means this render target will be bound to the shader context as layered.
	If not layered, it means this render target will be treated as 6 individual render targets.
    
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
class RenderTargetCube;
}

//------------------------------------------------------------------------------
namespace Base
{
class RenderTargetCubeBase : public Core::RefCounted
{
    __DeclareClass(RenderTargetCubeBase);
public:

    /// clear flags
    enum ClearFlag
    {
        ClearColor = (1<<0)
    };

    /// constructor
    RenderTargetCubeBase();
    /// destructor
    virtual ~RenderTargetCubeBase();

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
    /// add a color buffer
    void SetColorBufferFormat(CoreGraphics::PixelFormat::Code colorFormat);
    /// get color buffer format at index
    CoreGraphics::PixelFormat::Code GetColorBufferFormat() const;
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
	/// set the face to which we are currently drawing
	void SetDrawFace(CoreGraphics::Texture::CubeFace face);
	/// get the face to which we are currently drawing
	CoreGraphics::Texture::CubeFace GetDrawFace() const;
	/// set if render target cube should be layered
	void SetLayered(bool layered);
	/// get if render target cube is created as layered
	const bool GetLayered() const;

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

    /// sets if we should create a depth-stencil cube
    void SetDepthStencilCube(bool b);
    /// get if we should use a depth-stencil cube
    bool HasDepthStencilCube() const;

    /// get byte size in memory, implemented in platform specific classes
    SizeT GetMemorySize() const;
    /// set optional memory offset, not used by all platforms
    void SetMemoryOffset(SizeT size);

	/// force-clears render target, useful if clearing is required outside a frame shader
	virtual void Clear(uint flags);

protected:
    friend class RenderDeviceBase;

    Resources::ResourceId resolveTextureResId;
    CoreGraphics::FrameBatchType::Code batchType;
    SizeT width;
    SizeT height;

	bool resolveTextureDimensionsValid;
	bool resolveRectValid;
	bool resolveRectArrayValid;
	Math::rectangle<int> resolveRect;
	Util::Array<Math::rectangle<int> > resolveRectArray;

    uint clearFlags;
    Math::float4 clearColor;
	float clearDepth;
	int clearStencil;
	CoreGraphics::Texture::CubeFace currentDrawFace;
	bool layered;
    CoreGraphics::AntiAliasQuality::Code antiAliasQuality;
    CoreGraphics::PixelFormat::Code colorBufferFormat;
    Ptr<CoreGraphics::Texture> resolveTexture;
    Ptr<CoreGraphics::Texture> resolveCPUTexture;
	SizeT mipLevels;
    bool mipMapsEnabled;
    bool isValid;
    bool inBeginPass;
    bool inBeginBatch;
    bool resolveCpuAccess;
    bool useDepthStencilCube;
	
    SizeT memoryOffset;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetCubeBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetWidth(SizeT w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetCubeBase::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetHeight(SizeT h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
RenderTargetCubeBase::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetColorBufferFormat(CoreGraphics::PixelFormat::Code colorFormat)
{
    this->colorBufferFormat = colorFormat;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::PixelFormat::Code
RenderTargetCubeBase::GetColorBufferFormat() const
{
    return this->colorBufferFormat;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetCubeBase::SetMipMaps( SizeT mips )
{
	this->mipLevels = mips;
	this->mipMapsEnabled = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT 
RenderTargetCubeBase::GetMipMaps() const
{
	return this->mipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetClearColor(const Math::float4& c)
{
    this->clearColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
RenderTargetCubeBase::GetClearColor() const
{
    return this->clearColor;
}
//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetCubeBase::SetClearDepth( float f )
{
	this->clearDepth = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
RenderTargetCubeBase::GetClearDepth() const
{
	return this->clearDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetCubeBase::SetClearStencil( int i )
{
	this->clearStencil = i;
}

//------------------------------------------------------------------------------
/**
*/
inline int 
RenderTargetCubeBase::GetClearStencil() const
{
	return this->clearStencil;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetDrawFace(CoreGraphics::Texture::CubeFace i)
{
	this->currentDrawFace = i;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::Texture::CubeFace
RenderTargetCubeBase::GetDrawFace() const
{
	return this->currentDrawFace;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetLayered(bool layered)
{
	this->layered = layered;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
RenderTargetCubeBase::GetLayered() const
{
	return this->layered;
}
//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetResolveRect(const Math::rectangle<int>& r)
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
RenderTargetCubeBase::GetResolveRect() const
{
	return this->resolveRect;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
RenderTargetCubeBase::IsResolveRectValid() const
{
	return this->resolveRectValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetResolveRectArray(const Util::Array<Math::rectangle<int> >& rects)
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
RenderTargetCubeBase::GetResolveRectArray() const
{
	return this->resolveRectArray;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
RenderTargetCubeBase::IsResolveRectArrayValid() const
{
	return this->resolveRectArrayValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetResolveTextureResourceId(const Resources::ResourceId& resId)
{
    this->resolveTextureResId = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
RenderTargetCubeBase::GetResolveTextureResourceId() const
{
    return this->resolveTextureResId;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetCubeBase::HasResolveTexture() const
{
    return this->resolveTexture.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
RenderTargetCubeBase::GetResolveTexture() const
{
    n_assert(this->resolveTexture.isvalid());
    return this->resolveTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetCubeBase::HasCPUResolveTexture() const
{
    return this->resolveCPUTexture.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
RenderTargetCubeBase::GetCPUResolveTexture() const
{
    n_assert(this->resolveCPUTexture.isvalid());
    return this->resolveCPUTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetCubeBase::AreMipMapsEnabled() const
{
    return this->mipMapsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetResolveTargetCpuAccess(bool b)
{
    this->resolveCpuAccess = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderTargetCubeBase::GetResolveTargetCpuAccess() const
{
    return this->resolveCpuAccess;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderTargetCubeBase::SetClearFlags(uint f)
{
    this->clearFlags = f;
}


//------------------------------------------------------------------------------
/**
*/
inline void 
RenderTargetCubeBase::SetDepthStencilCube( bool b )
{
    this->useDepthStencilCube = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderTargetCubeBase::HasDepthStencilCube() const
{
    return this->useDepthStencilCube;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
RenderTargetCubeBase::GetClearFlags() const
{
    return this->clearFlags;
}

} // namespace Base    
//------------------------------------------------------------------------------


