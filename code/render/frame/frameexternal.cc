//------------------------------------------------------------------------------
//  frameexternal.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frameexternal.h"
#include "frameserver.h"

namespace Frame
{
__ImplementClass(Frame::FrameExternal, 'FREX', Frame::FramePassBase);

//------------------------------------------------------------------------------
/**
*/
FrameExternal::FrameExternal() : 
	frameShader(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameExternal::~FrameExternal()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameExternal::Discard()
{
	FramePassBase::Discard();

	// destroy frame shader
	if (this->frameShader.isvalid())
	{
		this->frameShader = 0;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameExternal::Render(IndexT frameIndex)
{
	n_assert(this->frameShaderName.IsValid());
	FrameServer* frameServer = FrameServer::Instance();

	// determine if frame shader has been loaded
	if (frameServer->HasFrameShader(this->frameShaderName))
	{
		// if loaded, and the pointer is valid, simply render
		if (this->frameShader.isvalid())
		{
			this->frameShader->Render(frameIndex);
		}
		else
		{
			// otherwise, lookup frame shader from server and render
			this->frameShader = frameServer->LookupFrameShader(this->frameShaderName);
			this->frameShader->Render(frameIndex);
		}
	}
	else if (this->frameShader.isvalid())
	{
		// if the frame shader has been unloaded, and our pointer is still intact, release reference
		this->frameShader = 0;
	}
}


} // namespace Frame