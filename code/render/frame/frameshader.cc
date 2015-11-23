//------------------------------------------------------------------------------
//  frameshader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frameshader.h"
#include "frameserver.h"

namespace Frame
{
__ImplementClass(Frame::FrameShader, 'FSHD', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FrameShader::FrameShader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FrameShader::~FrameShader()
{
    // make sure Discard() has been called
    n_assert(this->renderTargets.IsEmpty());
    n_assert(this->shaderVariables.IsEmpty());
    n_assert(this->framePasses.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShader::Discard()
{
    IndexT i;
    for (i = 0; i < this->renderTargets.Size(); i++)
    {
        this->renderTargets.ValueAtIndex(i)->Discard();
    }
    this->renderTargets.Clear();
    this->shaderVariables.Clear();
    for (i = 0; i < this->framePasses.Size(); i++)
    {
        this->framePasses[i]->Discard();
    }
    this->framePasses.Clear();
	
	// remove from frame server
	FrameServer::Instance()->DiscardFrameShader(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShader::OnDisplayResized(SizeT width, SizeT height)
{
	IndexT i;
	for (i = 0; i < this->depthStencilTargets.Size(); i++)
	{
		this->depthStencilTargets.ValueAtIndex(i)->OnDisplayResized(width, height);
	}

	for (i = 0; i < this->renderTargets.Size(); i++)
	{
		this->renderTargets.ValueAtIndex(i)->OnDisplayResized(width, height);
	}

	for(i = 0; i < this->framePasses.Size(); i++)
	{
		this->framePasses[i]->OnDisplayResize(width, height);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShader::InsertBeforePass(const Ptr<FramePassBase>& framePass, const Resources::ResourceId& before)
{
	IndexT i;
	for (i = 0; i < this->framePasses.Size(); i++)
	{
		if (this->framePasses[i]->GetName() == before)
		{
			this->framePasses.Insert(i, framePass);
			return;
		}
	}
	n_error("No frame pass '%s' exists in frame shader '%s'", before.AsString().AsCharPtr(), this->name.AsString().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShader::InsertAfterPass(const Ptr<FramePassBase>& framePass, const Resources::ResourceId& after)
{
	IndexT i;
	for (i = 0; i < this->framePasses.Size(); i++)
	{
		if (this->framePasses[i]->GetName() == after)
		{
			this->framePasses.Insert(i + 1, framePass);
			return;
		}
	}
	n_error("No frame pass '%s' exists in frame shader '%s'", after.AsString().AsCharPtr(), this->name.AsString().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShader::Begin()
{
	// render passes
	FRAME_LOG("\n\nFrameShader::Begin()\n");
	IndexT i;
	for (i = 0; i < this->framePasses.Size(); i++)
	{ 
		FRAME_LOG("  FrameShader::Begin() pass: %d", i);
		this->framePasses[i]->Begin();
		FRAME_LOG(" ");
	}
}

//------------------------------------------------------------------------------
/**
*/
void
FrameShader::Render(IndexT frameIndex)
{
	// render passes
	FRAME_LOG("\n\nFrameShader::Render()\n");
	IndexT i;
	for (i = 0; i < this->framePasses.Size(); i++)
	{ 
		FRAME_LOG("  FrameShader::Render() pass: %d", i);
		this->framePasses[i]->Render(frameIndex);
		FRAME_LOG(" ");
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShader::End()
{
	// render passes
	FRAME_LOG("\n\nFrameShader::End()\n");
	IndexT i;
	for (i = 0; i < this->framePasses.Size(); i++)
	{ 
		FRAME_LOG("  FrameShader::End() pass: %d", i);
		this->framePasses[i]->End();
		FRAME_LOG(" ");
	}
}

} // namespace Frame