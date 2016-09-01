#pragma once
//------------------------------------------------------------------------------
/**
	A frame pass prepares a rendering sequence, draws and subpasses must reside
	within one of these objects.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
#include "framesubpass.h"
#include "coregraphics/pass.h"
namespace Frame2
{
class FramePass : public FrameOp
{
	__DeclareClass(FramePass);
public:
	/// constructor
	FramePass();
	/// destructor
	virtual ~FramePass();

	/// set pass
	void SetPass(const Ptr<CoreGraphics::Pass>& pass);
	/// add subpass
	void AddSubpass(const Ptr<FrameSubpass>& subpass);

	/// run operation
	void Run(const IndexT frameIndex);

private:
	Ptr<CoreGraphics::Pass> pass;
	Util::Array<Ptr<FrameSubpass>> subpasses;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FramePass::SetPass(const Ptr<CoreGraphics::Pass>& pass)
{
	this->pass = pass;
}

} // namespace Frame2