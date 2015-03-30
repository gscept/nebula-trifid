//------------------------------------------------------------------------------
//  d3d9rendertarget.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/config.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "coregraphics/win360/d3d9types.h"
#include "coregraphics/displaydevice.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/shaderserver.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9RenderTarget, 'D9RT', Base::RenderTargetBase);

using namespace Win360;
using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
D3D9RenderTarget::D3D9RenderTarget() :
    d3d9RenderTarget(0),
    d3d9DepthStencil(0),
    d3d9ResolveTexture(0),
    d3d9CPUResolveTexture(0),
    d3d9MultiSampleType(D3DMULTISAMPLE_NONE),
    d3d9MultiSampleQuality(0),
    d3d9ColorBufferFormat(D3DFMT_UNKNOWN),
    needsResolve(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9RenderTarget::~D3D9RenderTarget()
{
    n_assert(!this->isValid);
    n_assert(0 == this->d3d9RenderTarget);
    n_assert(0 == this->d3d9DepthStencil);
    n_assert(0 == this->d3d9ResolveTexture);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9RenderTarget::Setup()
{
    n_assert(0 == this->d3d9RenderTarget);
    n_assert(0 == this->d3d9DepthStencil);
    n_assert(0 == this->d3d9ResolveTexture);
    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = D3D9RenderDevice::Instance()->GetDirect3DDevice();
    
    // call parent class
    RenderTargetBase::Setup();

    // if we're the default render target, query display device
    // for setup parameters
    if (this->isDefaultRenderTarget)
    {
        // NOTE: the default render target will never be anti-aliased!
        // this assumes a render pipeline where the actual rendering goes
        // into an offscreen render target and is then resolved to the back buffer
        DisplayDevice* displayDevice = DisplayDevice::Instance();
        this->SetWidth(displayDevice->GetDisplayMode().GetWidth());
        this->SetHeight(displayDevice->GetDisplayMode().GetHeight());
        this->SetAntiAliasQuality(AntiAliasQuality::None);
        this->SetColorBufferFormat(displayDevice->GetDisplayMode().GetPixelFormat());
    }

    // setup our pixel format and multisample parameters (order important!)
    this->d3d9ColorBufferFormat = D3D9Types::AsD3D9PixelFormat(this->colorBufferFormat);
    this->SetupMultiSampleType();

    // check if a resolve texture must be allocated
    if (this->mipMapsEnabled ||
        (D3DMULTISAMPLE_NONE != this->d3d9MultiSampleType) ||
        (this->resolveTextureDimensionsValid &&
         ((this->resolveTextureWidth != this->width) ||
          (this->resolveTextureHeight != this->height))))
    {
        this->needsResolve = true;
    }
    else
    {
        this->needsResolve = false;
    }

    // create the render target either as a texture, or as
    // a surface, or don't create it if rendering goes
    // into backbuffer
    if (!this->needsResolve)
    {
        if (!this->isDefaultRenderTarget)
        {
            DWORD usage = D3DUSAGE_RENDERTARGET;
            D3DPOOL pool = D3DPOOL_DEFAULT;

            // inplace resolve texture can be used (render target == resolve texture)
            hr = d3d9Dev->CreateTexture(this->width,                    // Width
                                        this->height,                   // Height
                                        1,                              // Levels
                                        usage,                          // Usage
                                        this->d3d9ColorBufferFormat,    // Format
                                        pool,                           // Pool
                                        &(this->d3d9ResolveTexture),    // ppTexture
                                        NULL);                          // pSharedHandle
            n_assert(SUCCEEDED(hr));
            n_assert(0 != this->d3d9ResolveTexture);
            
            // get the actual render target surface of the texture
            hr = this->d3d9ResolveTexture->GetSurfaceLevel(0, &(this->d3d9RenderTarget));
            n_assert(SUCCEEDED(hr));

            // create extra cpu texture for cpu look up (read-only)
            if (this->resolveCpuAccess)
            {
                usage = D3DUSAGE_DYNAMIC;        
                pool = D3DPOOL_SYSTEMMEM;
                hr = d3d9Dev->CreateTexture(this->width,    // Width
                    this->height,                           // Height
                    1,                                      // Levels
                    usage,                                  // Usage
                    this->d3d9ColorBufferFormat,            // Format
                    pool,                                   // Pool
                    &(this->d3d9CPUResolveTexture),         // ppTexture
                    NULL);                                  // pSharedHandle

                n_assert(SUCCEEDED(hr));
                n_assert(0 != this->d3d9CPUResolveTexture);
            }
        }
        else
        {
            // NOTE: if we are the default render target and not antialiased, 
            // rendering will go directly to the backbuffer, so there's no
            // need to allocate a render target
        }
    }
    else
    {
        // need to create an extra resolve texture, create the
        // actual render target directly as surface
        hr = d3d9Dev->CreateRenderTarget(this->width,                   // Width
                                         this->height,                  // Height
                                         this->d3d9ColorBufferFormat,   // Format
                                         this->d3d9MultiSampleType,     // MultiSample
                                         this->d3d9MultiSampleQuality,  // MultisampleQuality
                                         FALSE,                         // Lockable
                                         &(this->d3d9RenderTarget),     // ppSurface
                                         NULL);                         // pSharedHandle
        n_assert(SUCCEEDED(hr));
        n_assert(0 != this->d3d9RenderTarget);

        // create the resolve texture
        SizeT resolveWidth = this->resolveTextureDimensionsValid ? this->resolveTextureWidth : this->width;
        SizeT resolveHeight = this->resolveTextureDimensionsValid ? this->resolveTextureHeight : this->height;

        DWORD usage = D3DUSAGE_RENDERTARGET;
        D3DPOOL pool = D3DPOOL_DEFAULT;
        if (this->mipMapsEnabled)
        {
            usage |= D3DUSAGE_AUTOGENMIPMAP;
        }
        hr = d3d9Dev->CreateTexture(resolveWidth,                   // Width
                                    resolveHeight,                  // Height
                                    1,                              // Levels
                                    usage,                          // Usage
                                    this->d3d9ColorBufferFormat,    // Format
                                    pool,                           // Pool
                                    &(this->d3d9ResolveTexture),    // ppTexture
                                    NULL);                          // pSharedHandle
        
        n_assert(SUCCEEDED(hr));
        n_assert(0 != this->d3d9ResolveTexture);

        // create extra cpu texture for cpu look up (read-only)
        if (this->resolveCpuAccess)
        {
            usage = D3DUSAGE_DYNAMIC;        
            pool = D3DPOOL_SYSTEMMEM;
            hr = d3d9Dev->CreateTexture(resolveWidth,                   // Width
                resolveHeight,                  // Height
                1,                              // Levels
                usage,                          // Usage
                this->d3d9ColorBufferFormat,    // Format
                pool,                           // Pool
                &(this->d3d9CPUResolveTexture),    // ppTexture
                NULL);                          // pSharedHandle

            n_assert(SUCCEEDED(hr));
            n_assert(0 != this->d3d9CPUResolveTexture);
        }
    }

    // create the depth/stencil buffer if needed
    if (this->HasDepthStencilBuffer())
    {
        if (this->sharedDepthStencilBufferTarget.isvalid())
        {
            n_assert(0 != this->sharedDepthStencilBufferTarget->d3d9DepthStencil);
            n_assert(this->width == this->sharedDepthStencilBufferTarget->width);
            n_assert(this->height == this->sharedDepthStencilBufferTarget->height);
            n_assert(this->antiAliasQuality == this->sharedDepthStencilBufferTarget->antiAliasQuality);
            this->d3d9DepthStencil = this->sharedDepthStencilBufferTarget->d3d9DepthStencil;
            this->d3d9DepthStencil->AddRef();
        }
        else
        {
            hr = d3d9Dev->CreateDepthStencilSurface(this->width,                   // Width
                                                    this->height,                  // Height
                                                    D3DFMT_D24S8,                  // Format
                                                    this->d3d9MultiSampleType,     // MultiSample
                                                    this->d3d9MultiSampleQuality,  // MultisampleQuality
                                                    TRUE,                          // Discard
                                                    &(this->d3d9DepthStencil),     // ppSurface
                                                    NULL);                         // pSharedHandle
            n_assert(SUCCEEDED(hr));
            n_assert(0 != this->d3d9DepthStencil);
        }
    }

    // if a resolve texture exists, create a shared texture resource, so that
    // the texture is publicly visible
    if ((0 != this->d3d9ResolveTexture) && this->resolveTextureResId.IsValid())
    {
        this->resolveTexture = ResourceManager::Instance()->CreateUnmanagedResource(this->resolveTextureResId, Texture::RTTI).downcast<Texture>();  
        this->resolveTexture->SetupFromD3D9Texture(this->d3d9ResolveTexture);
        this->d3d9ResolveTexture->AddRef();
    }
    // if a cpu resolve texture exists, create a shared texture resource, so that
    // the texture is usable for a cpu lockup
    if ((0 != this->d3d9CPUResolveTexture) && this->resolveTextureResId.IsValid())
    {
        Resources::ResourceId resolveCPUTextureResId = Util::String(this->resolveTextureResId.AsString() + "_CPU_ReadOnly");
        this->resolveTexture = ResourceManager::Instance()->CreateUnmanagedResource(resolveCPUTextureResId, Texture::RTTI).downcast<Texture>();  
        this->resolveCPUTexture->SetUsage(Texture::UsageDynamic);
        this->resolveCPUTexture->SetAccess(Texture::AccessRead);               
        this->resolveCPUTexture->SetupFromD3D9Texture(this->d3d9CPUResolveTexture);
        this->d3d9CPUResolveTexture->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9RenderTarget::Discard()
{
    RenderTargetBase::Discard();
    if (0 != this->d3d9RenderTarget)
    {
        this->d3d9RenderTarget->Release();
        this->d3d9RenderTarget = 0;
    }
    if (0 != this->d3d9DepthStencil)
    {
        this->d3d9DepthStencil->Release();
        this->d3d9DepthStencil = 0;
    }
    if (0 != this->d3d9ResolveTexture)
    {
        this->d3d9ResolveTexture->Release();
        this->d3d9ResolveTexture = 0;
    }
    if (0 != this->d3d9CPUResolveTexture)
    {
        this->d3d9CPUResolveTexture->Release();
        this->d3d9CPUResolveTexture = 0;
    }
    this->sharedPixelSize = 0;
    this->sharedHalfPixelSize = 0;
}

//------------------------------------------------------------------------------
/**
    Select the antialias parameters that most closely resembly 
    the preferred settings in the DisplayDevice object.
*/
void
D3D9RenderTarget::SetupMultiSampleType()
{
    n_assert(D3DFMT_UNKNOWN != this->d3d9ColorBufferFormat);
    D3D9RenderDevice* renderDevice = D3D9RenderDevice::Instance();
    IDirect3D9* d3d9 = renderDevice->GetDirect3D();

    #if NEBULA3_DIRECT3D_DEBUG
        this->d3d9MultiSampleType = D3DMULTISAMPLE_NONE;
        this->d3d9MultiSampleQuality = 0;
    #else
        // convert Nebula3 antialias quality into D3D type
        this->d3d9MultiSampleType = D3D9Types::AsD3D9MultiSampleType(this->antiAliasQuality);
        
        // check if the multisample type is compatible with the selected display mode
        DWORD availableQualityLevels = 0;
        HRESULT renderTargetResult = d3d9->CheckDeviceMultiSampleType(0, 
                                     NEBULA3_DIRECT3D_DEVICETYPE,
                                     this->d3d9ColorBufferFormat,
                                     FALSE,
                                     this->d3d9MultiSampleType,
                                     &availableQualityLevels);
        HRESULT depthBufferResult = d3d9->CheckDeviceMultiSampleType(0,
                                    NEBULA3_DIRECT3D_DEVICETYPE,
                                    D3DFMT_D24S8,
                                    FALSE,
                                    this->d3d9MultiSampleType,
                                    NULL);
        if ((D3DERR_NOTAVAILABLE == renderTargetResult) || (D3DERR_NOTAVAILABLE == depthBufferResult))
        {
            // reset to no multisampling
            this->d3d9MultiSampleType = D3DMULTISAMPLE_NONE;
            this->d3d9MultiSampleQuality = 0;
        }
        else
        {
            n_assert(SUCCEEDED(renderTargetResult) && SUCCEEDED(depthBufferResult));
        }

        // clamp multisample quality to the available quality levels
        if (availableQualityLevels > 0)
        {
            this->d3d9MultiSampleQuality = availableQualityLevels - 1;
        }
        else
        {
            this->d3d9MultiSampleQuality = 0;
        }
    #endif
}  

//------------------------------------------------------------------------------
/**
*/
void
D3D9RenderTarget::BeginPass()
{
    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = D3D9RenderDevice::Instance()->GetDirect3DDevice();

    // apply the render target (may be the back buffer)
    if (0 != this->d3d9RenderTarget)
    {
        d3d9Dev->SetRenderTarget(this->mrtIndex, this->d3d9RenderTarget);
    }
    else
    {
        n_assert(this->IsDefaultRenderTarget());
        IDirect3DSurface9* backBuffer = 0;
        hr = d3d9Dev->GetBackBuffer(0,0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
        n_assert(SUCCEEDED(hr));
        hr = d3d9Dev->SetRenderTarget(this->mrtIndex, backBuffer);
        n_assert(SUCCEEDED(hr));
        backBuffer->Release();        
    }

    // apply the depth stencil buffer
    if (0 != this->d3d9DepthStencil)
    {
        hr = d3d9Dev->SetDepthStencilSurface(this->d3d9DepthStencil);
        n_assert(SUCCEEDED(hr));
    }
    else
    {
        // for default rendertarget, 
        // always clear depthstencil which could be set in previous offscreen passes
        // this avoids use of possible MSAA depthstencil surface with a always Non-MSAA Backbuffer
        if (this->IsDefaultRenderTarget())
        {
            if (DisplayDevice::Instance()->GetAntiAliasQuality() != AntiAliasQuality::None)
            {
                d3d9Dev->SetDepthStencilSurface(NULL);            
            }               
        }        
    }

    RenderTargetBase::BeginPass();
    
    // clear render target if requested
    DWORD d3d9ClearFlags = 0;
    if (0 != (this->clearFlags & ClearColor))
    {
        d3d9ClearFlags |= D3DCLEAR_TARGET;
    }
    if (0 != this->d3d9DepthStencil)
    {
        if (0 != (this->clearFlags & ClearDepth))
        {
            d3d9ClearFlags |= D3DCLEAR_ZBUFFER;
        }
        if (0 != (this->clearFlags & ClearStencil))
        {
            d3d9ClearFlags |= D3DCLEAR_STENCIL;
        }
    }
    if (0 != d3d9ClearFlags)
    {
        hr = d3d9Dev->Clear(0,             // Count
                            NULL,          // pRects
                            d3d9ClearFlags,    // Flags
                            D3DCOLOR_COLORVALUE(this->clearColor.x(), 
                                this->clearColor.y(), 
                                this->clearColor.z(), 
                                this->clearColor.w()),  // Color
                            this->clearDepth,           // Z
                            this->clearStencil);        // Stencil
        n_assert(SUCCEEDED(hr));
    }

    // get shared pixel size
    if (!this->sharedPixelSize.isvalid())
    {
        ShaderVariable::Semantic semPixelSize(NEBULA3_SEMANTIC_PIXELSIZE);
        this->sharedPixelSize = CoreGraphics::ShaderServer::Instance()->GetSharedVariableBySemantic(semPixelSize);
        n_assert(this->sharedPixelSize.isvalid());
    }
    SizeT w = this->GetWidth();
    SizeT h = this->GetHeight();
    Math::float4 pixelSize(1.0f / float(w), 1.0f / float(h), 0.0f, 0.0f);
    this->sharedPixelSize->SetFloat4(pixelSize);
      
    // get shared half pixel size
    if (!this->sharedHalfPixelSize.isvalid())
    {
        ShaderVariable::Semantic semPixelSize(NEBULA3_SEMANTIC_HALFPIXELSIZE);
        this->sharedHalfPixelSize = CoreGraphics::ShaderServer::Instance()->GetSharedVariableBySemantic(semPixelSize);
        n_assert(this->sharedHalfPixelSize.isvalid());
    }
    // set half pixel size for lookup in pixelshaders into screen mapped textures 
    float xHalfSize = 0.5f / float(w);
    float yHalfSize = 0.5f / float(h);
    this->sharedHalfPixelSize->SetFloat4(Math::float4(xHalfSize, yHalfSize,0,0));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9RenderTarget::EndPass()
{
    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = D3D9RenderDevice::Instance()->GetDirect3DDevice();

    // if necessary need to resolve the render target, either
    // into our resolve texture, or into the back buffer
    if (this->needsResolve)
    {        
        RECT destRect;
        CONST RECT* pDestRect = NULL;
        if (this->resolveRectValid)
        {
            destRect.left   = this->resolveRect.left;
            destRect.right  = this->resolveRect.right;
            destRect.top    = this->resolveRect.top;
            destRect.bottom = this->resolveRect.bottom;
            pDestRect = &destRect;
        }
        IDirect3DSurface9* resolveSurface = 0;
        hr = this->d3d9ResolveTexture->GetSurfaceLevel(0, &resolveSurface);
        hr = d3d9Dev->StretchRect(this->d3d9RenderTarget, NULL, resolveSurface, pDestRect, D3DTEXF_NONE);
        n_assert(SUCCEEDED(hr));
        
        // need cpu access, copy from gpu mem to sys mem
        if (this->resolveCpuAccess)
        {
            HRESULT hr;   
            D3DLOCKED_RECT dstLockRect;
            D3DLOCKED_RECT srcLockRect;
            IDirect3DSurface9* dstSurface = 0;
            hr = this->d3d9CPUResolveTexture->GetSurfaceLevel(0, &dstSurface);
            n_assert(SUCCEEDED(hr));
            hr = dstSurface->LockRect(&dstLockRect, 0, 0);
            n_assert(SUCCEEDED(hr));
            hr = resolveSurface->LockRect(&srcLockRect, 0, D3DLOCK_READONLY);
            n_assert(SUCCEEDED(hr));
            Memory::Copy(srcLockRect.pBits, dstLockRect.pBits, dstLockRect.Pitch * this->resolveCPUTexture->GetWidth());
            dstSurface->Release();
        }
        resolveSurface->Release();
    }
    else if (this->resolveCpuAccess)
    {
        HRESULT hr;
        // copy data
        IDirect3DSurface9* resolveSurface = 0;
        hr = this->d3d9CPUResolveTexture->GetSurfaceLevel(0, &resolveSurface);
        n_assert(SUCCEEDED(hr));
        hr = d3d9Dev->GetRenderTargetData(this->d3d9RenderTarget, resolveSurface);
        n_assert(SUCCEEDED(hr));
        resolveSurface->Release();
    }
    // unset multiple rendertargets
    if (this->mrtIndex > 0)
    {
        d3d9Dev->SetRenderTarget(this->mrtIndex, 0);
    }    
    RenderTargetBase::EndPass();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9RenderTarget::GenerateMipLevels()
{
    n_assert(0 != this->d3d9ResolveTexture);
    n_assert(this->mipMapsEnabled);
    this->d3d9ResolveTexture->GenerateMipSubLevels();
}

} // namespace Direct3D9

