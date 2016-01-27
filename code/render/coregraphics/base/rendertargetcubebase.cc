//------------------------------------------------------------------------------
//  rendertargetbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/rendertargetcube.h"
#include "resources/resourcemanager.h"
#include "coregraphics/displaydevice.h"

namespace Base
{
__ImplementClass(Base::RenderTargetCubeBase, 'RTCB', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
RenderTargetCubeBase::RenderTargetCubeBase() :
    batchType(FrameBatchType::InvalidBatchType),
    width(0),
    height(0),
    clearFlags(0),
	layered(false),
#if NEBULA3_DEBUG
    clearColor(1.0f, 0.0f, 1.0f, 0.0f),
#else // on release clear defaultrendertarget to black for nice loadingscreen fading
    clearColor(0.0f, 0.0f, 0.0f, 0.0f),
#endif
    antiAliasQuality(AntiAliasQuality::None),
    colorBufferFormat(PixelFormat::X8R8G8B8),
	useDepthStencilCube(false),
    mipMapsEnabled(false),
    isValid(false),    
    inBeginPass(false),
    inBeginBatch(false),
    resolveCpuAccess(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RenderTargetCubeBase::~RenderTargetCubeBase()
{
    n_assert(!this->isValid);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::Setup()
{
    n_assert(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::Discard()
{
    n_assert(this->isValid);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    if (this->resolveTexture.isvalid())
    {
        const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
        if (resManager->HasResource(this->resolveTexture->GetResourceId()))
        {
            resManager->UnregisterUnmanagedResource(this->resolveTexture->GetResourceId());
        }
        this->resolveTexture = 0;
    }
	if (this->resolveCPUTexture.isvalid())
	{
		const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
		if (resManager->HasResource(this->resolveCPUTexture->GetResourceId()))
		{
			resManager->UnregisterUnmanagedResource(this->resolveCPUTexture->GetResourceId());
		}
		this->resolveCPUTexture = 0;
	}
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::BeginPass()
{
    n_assert(this->isValid);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    this->inBeginPass = true;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::BeginBatch(FrameBatchType::Code t)
{
    n_assert(this->inBeginPass);
    n_assert(!this->inBeginBatch);
    this->inBeginBatch = true;
    this->batchType = t;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::EndBatch()
{
    n_assert(this->inBeginBatch);
    this->inBeginBatch = false; 
    this->batchType = FrameBatchType::InvalidBatchType;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::EndPass()
{
    n_assert(this->isValid);
    n_assert(this->inBeginPass);
    n_assert(!this->inBeginBatch);
    this->inBeginPass = false;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::GenerateMipLevels()
{
    n_assert(this->mipMapsEnabled);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::ResolveDepthBuffer()
{
    n_error("RenderTargetBase::ResolveDepthBuffer() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderTargetCubeBase::SetMemoryOffset(SizeT size)
{
    this->memoryOffset = size;            
}

//------------------------------------------------------------------------------
/**
*/
SizeT 
RenderTargetCubeBase::GetMemorySize() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::Clear(uint flags)
{
	n_error("RenderTargetBase::Clear() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetCubeBase::OnDisplayResized(SizeT width, SizeT height)
{
	// override in subclass
}

} // namespace Base

