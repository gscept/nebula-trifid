//------------------------------------------------------------------------------
//  d3d11textureinfo.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"

#ifdef __DX11__
#include "d3d11textureinfo.h"
#include "coregraphics/pixelformat.h"
#include "d3dx11tex.h"

#include "render/coregraphics/d3d11/d3d11types.h"
#include "coregraphics/texture.h"
#include "util/string.h"

using namespace Util;
using namespace CoreGraphics;

namespace Resources
{
//------------------------------------------------------------------------------
/**
*/
D3D11TextureInfo::D3D11TextureInfo(void) :
    width(0),
    height(0),
    depth(0),
    numMipLevels(0),
    pixelFormat(PixelFormat::DXT1),
    textureType(Texture::Texture2D)
{}

//------------------------------------------------------------------------------
/**
*/
D3D11TextureInfo::D3D11TextureInfo(const D3D11TextureInfo* info) :
    ResourceInfo((const ResourceInfo*)(info)),
    width(info->width),
    height(info->height),
    depth(info->depth),
    numMipLevels(info->numMipLevels),
    pixelFormat(info->pixelFormat),
    textureType(info->textureType)
{}

//------------------------------------------------------------------------------
/**
*/
void
D3D11TextureInfo::SetInfo(const D3DX11_IMAGE_INFO& imageInfo)
{
    this->width = imageInfo.Width;
    this->height = imageInfo.Height;
    this->depth = imageInfo.Depth;
	
    this->numMipLevels = imageInfo.MipLevels;
    this->pixelFormat = Direct3D11::D3D11Types::AsNebulaPixelFormat(imageInfo.Format);
    switch (imageInfo.ResourceDimension)
    {
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		if (imageInfo.ArraySize == 1)
			this->textureType = Texture::Texture2D;
		else if(imageInfo.ArraySize == 6)
			this->textureType = Texture::TextureCube;
        break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        this->textureType = Texture::Texture3D;
        break;
    default:
        n_error("unknown texture-type: %i", imageInfo.ResourceDimension);
        break;
    }
}
} // namespace Resources
//------------------------------------------------------------------------------

#endif