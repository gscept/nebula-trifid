//------------------------------------------------------------------------------
//  d3d9texturecreator.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#ifndef __DX11__
#include "d3d9texturecreator.h"
#include "resources/streaming/textureinfo.h"
#include "coregraphics/texture.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"

namespace Resources
{
__ImplementClass(Resources::D3D9TextureCreator, 'WTCR', Resources::ResourceCreator);

//------------------------------------------------------------------------------
/**
*/
D3D9TextureCreator::D3D9TextureCreator()
{}

//------------------------------------------------------------------------------
/**
*/
D3D9TextureCreator::~D3D9TextureCreator()
{}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
D3D9TextureCreator::CreateResource(const TextureInfo* texInfo)
{
    IDirect3DDevice9* d3d9Dev = Direct3D9::D3D9RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Dev);
    Ptr<CoreGraphics::Texture> returnResource = CoreGraphics::Texture::Create();
    IDirect3DTexture9* newTexP;
    IDirect3DCubeTexture9* newCubeTexP;

    if (CoreGraphics::Texture::Texture2D == texInfo->GetType())
    {
        HRESULT hr = d3d9Dev->CreateTexture(texInfo->GetWidth(), texInfo->GetHeight(), texInfo->GetMipLevels(),
            0, Win360::D3D9Types::AsD3D9PixelFormat(texInfo->GetPixelFormat()), D3DPOOL_MANAGED, &newTexP, 0);
        n_assert(SUCCEEDED(hr));
        returnResource->SetupFromD3D9Texture(newTexP, false);
    }
    else
    {
        HRESULT hr = d3d9Dev->CreateCubeTexture(texInfo->GetWidth(), texInfo->GetMipLevels(),
            0, Win360::D3D9Types::AsD3D9PixelFormat(texInfo->GetPixelFormat()), D3DPOOL_MANAGED, &newCubeTexP, 0);
        n_assert(SUCCEEDED(hr));
        returnResource->SetupFromD3D9CubeTexture(newCubeTexP);
    }
    returnResource->SetAsyncEnabled(true);
    returnResource->SetResourceId("not set");
    return Ptr<Resource>(returnResource);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
D3D9TextureCreator::CreateResource(const ResourceInfo* resInfo)
{
    return this->CreateResource((const TextureInfo*)resInfo);
}
} // namespace Resources
//------------------------------------------------------------------------------

#endif