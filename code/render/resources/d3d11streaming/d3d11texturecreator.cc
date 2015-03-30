//------------------------------------------------------------------------------
//  d3d11texturecreator.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"

#ifdef __DX11__
#include "d3d11texturecreator.h"
#include "resources/streaming/textureinfo.h"
#include "coregraphics/texture.h"
#include "coregraphics/d3d11/d3d11renderdevice.h"

namespace Resources
{
__ImplementClass(Resources::D3D11TextureCreator, 'WTCR', Resources::ResourceCreator);

//------------------------------------------------------------------------------
/**
*/
D3D11TextureCreator::D3D11TextureCreator()
{}

//------------------------------------------------------------------------------
/**
*/
D3D11TextureCreator::~D3D11TextureCreator()
{}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
D3D11TextureCreator::CreateResource(const TextureInfo* texInfo)
{
    ID3D11Device* d3d11Dev = Direct3D11::D3D11RenderDevice::Instance()->GetDirect3DDevice();
	ID3D11DeviceContext* context = Direct3D11::D3D11RenderDevice::Instance()->GetDirect3DDeviceContext();
    n_assert(0 != d3d11Dev);
    Ptr<CoreGraphics::Texture> returnResource = CoreGraphics::Texture::Create();
    ID3D11Texture2D* newTexP;
    ID3D11Texture2D* newCubeTexP;

    if (CoreGraphics::Texture::Texture2D == texInfo->GetType())
    {
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = texInfo->GetWidth();
		desc.Height = texInfo->GetHeight();
		desc.MipLevels = texInfo->GetMipLevels();
		desc.Format = Direct3D11::D3D11Types::AsD3D11PixelFormat(texInfo->GetPixelFormat());
		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		HRESULT hr = d3d11Dev->CreateTexture2D(&desc, NULL, &newTexP);
		/* DX 9 stuff
        HRESULT hr = d3d11Dev->CreateTexture(texInfo->GetWidth(), texInfo->GetHeight(), texInfo->GetMipLevels(),
            0, Direct3D11::D3D11Types::AsD3D11PixelFormat(texInfo->GetPixelFormat()), D3DPOOL_MANAGED, &newTexP, 0);
			*/
        n_assert(SUCCEEDED(hr));
        returnResource->SetupFromD3D11Texture(newTexP, false);
    }
    else
    {
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = texInfo->GetWidth();
		desc.Height = texInfo->GetHeight();
		desc.MipLevels = texInfo->GetMipLevels();
		desc.Format = Direct3D11::D3D11Types::AsD3D11PixelFormat(texInfo->GetPixelFormat());
		desc.ArraySize = 6;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		HRESULT hr = d3d11Dev->CreateTexture2D(&desc, NULL, &newCubeTexP);
		/* DX 9 stuff
        HRESULT hr = d3d11Dev->CreateCubeTexture(texInfo->GetWidth(), texInfo->GetMipLevels(),
            0, Direct3D11::D3D11Types::AsD3D11PixelFormat(texInfo->GetPixelFormat()), D3DPOOL_MANAGED, &newCubeTexP, 0);
			*/
        n_assert(SUCCEEDED(hr));
        returnResource->SetupFromD3D11CubeTexture(newCubeTexP);
    }
    returnResource->SetAsyncEnabled(true);
    returnResource->SetResourceId("not set");
    return Ptr<Resource>(returnResource);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
D3D11TextureCreator::CreateResource(const ResourceInfo* resInfo)
{
    return this->CreateResource((const TextureInfo*)resInfo);
}
} // namespace Resources
//------------------------------------------------------------------------------

#endif