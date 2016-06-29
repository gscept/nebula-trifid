//------------------------------------------------------------------------------
//  framepassbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framepassbase.h"
#include "coregraphics/renderdevice.h"

namespace Frame
{
__ImplementClass(Frame::FramePassBase, 'FPSB', Core::RefCounted);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FramePassBase::FramePassBase() :
	renderTarget(0),
	multipleRenderTarget(0),
    renderTargetCube(0),
	shader(0),
	clearFlags(0),
    clearDepth(1.0f),
    clearStencil(0),
	useDefaultRendertarget(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FramePassBase::~FramePassBase()
{
    // make sure Discard() has been called
    n_assert(!this->renderTarget.isvalid());
    n_assert(!this->multipleRenderTarget.isvalid());
	n_assert(!this->shader.isvalid());
    n_assert(this->shaderVariables.IsEmpty());  
    n_assert(this->batches.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
FramePassBase::Discard()
{
	if (this->shader.isvalid())
	{
		this->shader = 0;
	}
    if (this->renderTarget.isvalid())
    {
        this->renderTarget = 0;
    }
    else if (this->multipleRenderTarget.isvalid())
    {
        this->multipleRenderTarget = 0;
    }
	else if (this->renderTargetCube.isvalid())
	{
		this->renderTargetCube = 0;
	}
    this->shaderVariables.Clear();
	
	IndexT i;
    for (i = 0; i < this->batches.Size(); i++)
    {
        this->batches[i]->Discard();
    }
    this->batches.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
FramePassBase::Begin()
{
	// implement in base class    
}

//------------------------------------------------------------------------------
/**
*/
void
FramePassBase::Render(IndexT frameIndex)
{
	// implement in base class
}

//------------------------------------------------------------------------------
/**
*/
void 
FramePassBase::End()
{
	// implement in base class
}

//------------------------------------------------------------------------------
/**
*/
void 
FramePassBase::OnWindowResize(SizeT width, SizeT height)
{
	// implement in subclass
}

} // namespace Frame
