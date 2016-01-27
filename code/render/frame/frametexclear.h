#pragma once
//------------------------------------------------------------------------------
/**
	Clears an texture within a frame shader.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frame/framepassbase.h"
namespace CoreGraphics
{
class ShaderReadWriteTexture;
}

namespace Frame
{
class FrameTexClear : public FramePassBase
{
	__DeclareClass(FrameTexClear);
public:
	/// constructor
	FrameTexClear();
	/// destructor
	virtual ~FrameTexClear();

	/// setup texture clear pass
	void Setup(const Ptr<CoreGraphics::ShaderReadWriteTexture>& target, const Math::float4& clearColor);
	/// render the pass
	void Render(IndexT frameIndex);
private:
	Ptr<CoreGraphics::ShaderReadWriteTexture> target;
	Math::float4 clearColor;
};
} // namespace Frame