#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::MultipleRenderTargetBase

    Base class for render targets. A render targets wraps up to 4 color buffers
    and an optional depth/stencil buffer into a C++ object. The special
    default render target represents the backbuffer and default depth/stencil
    surface.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "coregraphics/rendertarget.h"

//------------------------------------------------------------------------------
namespace Base
{
class MultipleRenderTargetBase : public Core::RefCounted
{
    __DeclareClass(MultipleRenderTargetBase);
public:
#if (__OGL4__ || __VULKAN__ || __DX11__)
	static const IndexT MaxNumRenderTargets = 8;
#else
	static const IndexT MaxNumRenderTargets = 4;
#endif

    /// constructor
    MultipleRenderTargetBase();
    /// destructor
    virtual ~MultipleRenderTargetBase();

    /// set rendertarget
    void AddRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt);
    /// get rendertarget at index
    const Ptr<CoreGraphics::RenderTarget>& GetRenderTarget(IndexT i) const;
    /// getnumber of rendertargets used
    SizeT GetNumRendertargets() const;

	/// setup render target
	void Setup();
	/// discard render target
	void Discard();

	/// sets depth-stencil target
	void SetDepthStencilTarget(const Ptr<CoreGraphics::DepthStencilTarget>& dt);
	/// get depth-stencil target
	const Ptr<CoreGraphics::DepthStencilTarget>& GetDepthStencilTarget() const;

    /// begin rendering to the render target
    void BeginPass();
    /// begin a batch
    void BeginBatch(CoreGraphics::FrameBatchType::Code batchType);
    /// end current batch
    void EndBatch();
    /// end current render pass
    void EndPass(); 
    
	/// set clear flags for render target at index
	void SetClearFlags(IndexT i, uint clearFlags);
	/// get clear flags for render target at index i
	uint GetClearFlags(IndexT i);
	/// set clear flags for depth-stencil target
	void SetDepthStencilClearFlags(uint clearFlags);
	/// get depth-stencil target clear flags
	uint GetDepthStencilClearFlags() const;
    /// set clear color of rendertarget at index
    void SetClearColor(IndexT i, const Math::float4& color);
	/// returns clear color of rendertarget at index
	const Math::float4& GetClearColor(IndexT i);
    /// set clear depth
    void SetClearDepth(float d);
    /// get clear depth
    float GetClearDepth() const;
    /// set clear stencil value
    void SetClearStencil(int s);
    /// get clear stencil value
    int GetClearStencil() const;

	/// called after we change the display size
	void OnDisplayResized(SizeT width, SizeT height);

protected:
    friend class RenderDeviceBase;

    Ptr<CoreGraphics::RenderTarget> renderTarget[MaxNumRenderTargets];
	Ptr<CoreGraphics::DepthStencilTarget> depthStencilTarget;
    Math::float4 clearColor[MaxNumRenderTargets];
	float clearDepth;
	int clearStencil;
	uint clearFlags[MaxNumRenderTargets];
	uint depthStencilClearFlags;
    bool clearDepthStencil;
    SizeT numRenderTargets;

	bool isSetup;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>& 
MultipleRenderTargetBase::GetRenderTarget(IndexT i) const
{
    n_assert(i < this->numRenderTargets);
    return this->renderTarget[i];
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT 
MultipleRenderTargetBase::GetNumRendertargets() const
{
    return this->numRenderTargets;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
MultipleRenderTargetBase::SetClearColor(IndexT i, const Math::float4& color)
{
	n_assert(i < this->numRenderTargets);
	this->clearColor[i] = color;
}


//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
MultipleRenderTargetBase::GetClearColor(IndexT i)
{
	n_assert(i < this->numRenderTargets);
	return this->clearColor[i];
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultipleRenderTargetBase::SetClearDepth(float d)
{
    this->clearDepth = d;
    this->clearDepthStencil = true;
}

//------------------------------------------------------------------------------
/**
*/
inline float
MultipleRenderTargetBase::GetClearDepth() const
{
    return this->clearDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultipleRenderTargetBase::SetClearStencil(int s)
{
    this->clearStencil = s;
    this->clearDepthStencil = true;
}

//------------------------------------------------------------------------------
/**
*/
inline int
MultipleRenderTargetBase::GetClearStencil() const
{
    return this->clearStencil;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultipleRenderTargetBase::SetClearFlags(IndexT i, uint clearFlags)
{
	this->clearFlags[i] = clearFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
MultipleRenderTargetBase::GetClearFlags(IndexT i)
{
	return this->clearFlags[i];
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultipleRenderTargetBase::SetDepthStencilClearFlags(uint clearFlags)
{
	this->depthStencilClearFlags = clearFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline uint 
MultipleRenderTargetBase::GetDepthStencilClearFlags() const
{
	return this->depthStencilClearFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultipleRenderTargetBase::SetDepthStencilTarget(const Ptr<CoreGraphics::DepthStencilTarget>& dt)
{
	n_assert(dt.isvalid());
	this->depthStencilTarget = dt;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::DepthStencilTarget>& 
MultipleRenderTargetBase::GetDepthStencilTarget() const
{
	return this->depthStencilTarget;
}

} // namespace Base    
//------------------------------------------------------------------------------

