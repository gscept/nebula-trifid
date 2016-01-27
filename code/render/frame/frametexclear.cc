//------------------------------------------------------------------------------
// frametexclear.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frametexclear.h"
#include "coregraphics/shaderreadwritetexture.h"

namespace Frame
{

__ImplementClass(Frame::FrameTexClear, 'FTXC', Frame::FramePassBase);
//------------------------------------------------------------------------------
/**
*/
FrameTexClear::FrameTexClear()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameTexClear::~FrameTexClear()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameTexClear::Setup(const Ptr<CoreGraphics::ShaderReadWriteTexture>& target, const Math::float4& clearColor)
{
	n_assert(target.isvalid());
	this->target = target;
	this->clearColor = clearColor;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameTexClear::Render(IndexT frameIndex)
{
	// simply perform clear
	this->target->Clear(this->clearColor);
}

} // namespace Frame