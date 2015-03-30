//------------------------------------------------------------------------------
//  multiplerendertargetbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/multiplerendertargetbase.h"

namespace Base
{
__ImplementClass(Base::MultipleRenderTargetBase, 'MRTB', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
MultipleRenderTargetBase::MultipleRenderTargetBase() :
    clearDepthStencil(false),
	depthStencilTarget(0),
    numRenderTargets(0)
{
    IndexT i;
    for (i = 0; i < MaxNumRenderTargets; i++)
    {
        this->clearColor[i].set(0.0f, 0.0f, 0.0f, 0.0f);
		this->clearDepth = 1.0f;
		this->clearStencil = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
MultipleRenderTargetBase::~MultipleRenderTargetBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
MultipleRenderTargetBase::AddRenderTarget(const Ptr<RenderTarget>& rt)
{
    n_assert(rt.isvalid());
    n_assert(this->numRenderTargets < MaxNumRenderTargets);
    this->renderTarget[this->numRenderTargets] = rt;
    this->renderTarget[this->numRenderTargets]->SetMRTIndex(this->numRenderTargets);
    this->numRenderTargets++;
}


//------------------------------------------------------------------------------
/**
*/
void 
MultipleRenderTargetBase::BeginPass()
{    
    IndexT i;
    for (i = 0; i < this->numRenderTargets; i++)
    {
        uint clearFlags = this->renderTarget[i]->GetClearFlags();
		this->renderTarget[i]->SetClearFlags(this->clearFlags[i]);
        this->renderTarget[i]->SetClearColor(this->clearColor[i]);
        this->renderTarget[i]->BeginPass();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultipleRenderTargetBase::BeginBatch(CoreGraphics::BatchType::Code batchType)
{
    IndexT i;
    for (i = 0; i < this->numRenderTargets; i++)
    {
        this->renderTarget[i]->BeginBatch(batchType);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultipleRenderTargetBase::EndBatch()
{
    IndexT i;
    for (i = 0; i < this->numRenderTargets; i++)
    {
        this->renderTarget[i]->EndBatch();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultipleRenderTargetBase::EndPass()
{
    IndexT i;
    for (i = 0; i < this->numRenderTargets; i++)
    {
        this->renderTarget[i]->EndPass();
    }
}



} // namespace Base