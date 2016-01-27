#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameRenderTargetBlit
    
    Copies from one render target to another inside a frame shader.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "framepassbase.h"
#include "coregraphics/rendertarget.h"
namespace Frame
{
class FrameRenderTargetBlit : public Frame::FramePassBase
{
	__DeclareClass(FrameRenderTargetBlit);
public:
	/// constructor
	FrameRenderTargetBlit();
	/// destructor
	virtual ~FrameRenderTargetBlit();

    /// perform copy
	void Render(IndexT frameIndex);

    /// set the render target from whom we want to copy
    void SetFrom(const Ptr<CoreGraphics::RenderTarget>& rt);
    /// set the render target to which we want to copy
    void SetTo(const Ptr<CoreGraphics::RenderTarget>& rt);

private:
    Ptr<CoreGraphics::RenderTarget> from;
    Ptr<CoreGraphics::RenderTarget> to;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameRenderTargetBlit::SetFrom( const Ptr<CoreGraphics::RenderTarget>& rt )
{
    n_assert(rt.isvalid());
    this->from = rt;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameRenderTargetBlit::SetTo( const Ptr<CoreGraphics::RenderTarget>& rt )
{
    n_assert(rt.isvalid());
    this->to = rt;
}

} // namespace Frame
//------------------------------------------------------------------------------