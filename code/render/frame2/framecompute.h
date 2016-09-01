#pragma once
//------------------------------------------------------------------------------
/**
	Executes compute shader.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
#include "coregraphics/shaderstate.h"
namespace Frame2
{
class FrameCompute : public FrameOp
{
	__DeclareClass(FrameCompute);
public:
	/// constructor
	FrameCompute();
	/// destructor
	virtual ~FrameCompute();

	/// set shader
	void SetShaderState(const Ptr<CoreGraphics::ShaderState>& state);
	/// set computation invocations
	void SetInvocations(const SizeT x, const SizeT y, const SizeT z);

	/// run operation
	void Run(const IndexT frameIndex);
private:
	Ptr<CoreGraphics::ShaderState> state;
	SizeT x, y, z;
};


//------------------------------------------------------------------------------
/**
*/
inline void
FrameCompute::SetShaderState(const Ptr<CoreGraphics::ShaderState>& state)
{
	this->state = state;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameCompute::SetInvocations(const SizeT x, const SizeT y, const SizeT z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

} // namespace Frame2