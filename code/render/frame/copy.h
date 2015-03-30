#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::Copy
    
    Copies from one rendertarget to another inside a frame shader.
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "framepassbase.h"
#include "coregraphics/rendertarget.h"
namespace Frame
{
class Copy : public Frame::FramePassBase
{
	__DeclareClass(Copy);
public:
	/// constructor
	Copy();
	/// destructor
	virtual ~Copy();

    /// perform copy
    void Render();

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
Copy::SetFrom( const Ptr<CoreGraphics::RenderTarget>& rt )
{
    n_assert(rt.isvalid());
    this->from = rt;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Copy::SetTo( const Ptr<CoreGraphics::RenderTarget>& rt )
{
    n_assert(rt.isvalid());
    this->to = rt;
}

} // namespace Frame
//------------------------------------------------------------------------------