//------------------------------------------------------------------------------
//  rendertargetbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/rendertarget.h"
#include "resources/resourcemanager.h"
#include "coregraphics/displaydevice.h"

namespace Base
{
__ImplementClass(Base::RenderTargetBase, 'RTGB', Core::RefCounted);

using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
RenderTargetBase::RenderTargetBase() :
    width(0),
    height(0),
    resolveTextureDimensionsValid(false),
    resolveRectValid(false),
	resolveRectArrayValid(false),
    resolveTextureWidth(0),
    resolveTextureHeight(0),
    resolveRect(0, 0, 0, 0),
    mrtIndex(0),
    clearFlags(0),
#if NEBULA3_DEBUG
    clearColor(1.0f, 0.0f, 1.0f, 0.0f),
#else // on release clear defaultrendertarget to black for nice loadingscreen fading
    clearColor(0.0f, 0.0f, 0.0f, 0.0f),
#endif
    antiAliasQuality(AntiAliasQuality::None),
    colorBufferFormat(PixelFormat::X8R8G8B8),
	depthStencilTarget(0),
    mipMapsEnabled(false),
    isValid(false),    
    inBeginPass(false),
    inBeginBatch(false),
    isDefaultRenderTarget(false),
    resolveCpuAccess(false),
	relativeSizeValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RenderTargetBase::~RenderTargetBase()
{
    n_assert(!this->isValid);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::Setup()
{
    n_assert(!this->isValid);
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::Discard()
{
    n_assert(this->isValid);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    this->depthStencilTarget = 0;
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
RenderTargetBase::BeginPass()
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
RenderTargetBase::BeginBatch(FrameBatchType::Code t)
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
RenderTargetBase::EndBatch()
{
    n_assert(this->inBeginBatch);
    this->inBeginBatch = false; 
    this->batchType = FrameBatchType::InvalidBatchType;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::EndPass()
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
RenderTargetBase::OnWindowResized(SizeT width, SizeT height)
{
	n_error("RenderTargetBase::OnDisplayResized() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::GenerateMipLevels()
{
    n_assert(this->mipMapsEnabled);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::ResolveDepthBuffer()
{
    n_error("RenderTargetBase::ResolveDepthBuffer() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderTargetBase::SetMemoryOffset(SizeT size)
{
    this->memoryOffset = size;            
}

//------------------------------------------------------------------------------
/**
*/
SizeT 
RenderTargetBase::GetMemorySize() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderTargetBase::Copy( const Ptr<CoreGraphics::RenderTarget>& tex )
{
    n_error("RenderTargetBase::Copy() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::Clear(uint flags)
{
	n_error("RenderTargetBase::Clear() not implemented!");
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTargetBase::ResetResolveRects()
{
	this->resolveRectValid = false;
	this->resolveRectArrayValid = false;
	this->resolveRectArray.Clear();

	// reset display to display region
	DisplayDevice* displayDevice = DisplayDevice::Instance();
	this->resolveRect.left = 0;
	this->resolveRect.top = 0;
	this->resolveRect.right = displayDevice->GetCurrentWindow()->GetDisplayMode().GetWidth();
	this->resolveRect.bottom = displayDevice->GetCurrentWindow()->GetDisplayMode().GetHeight();
}

} // namespace Base

