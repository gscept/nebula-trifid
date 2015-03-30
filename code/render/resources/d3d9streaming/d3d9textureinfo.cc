//------------------------------------------------------------------------------
//  d3d9textureinfo.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#ifndef __DX11__
#include "d3d9textureinfo.h"
#include "coregraphics/pixelformat.h"
#include "d3dx9tex.h"
#include "render/coregraphics/win360/d3d9types.h"
#include "coregraphics/texture.h"
#include "util/string.h"

using namespace Util;
using namespace CoreGraphics;

namespace Resources
{
//------------------------------------------------------------------------------
/**
*/
D3D9TextureInfo::D3D9TextureInfo(void) :
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
D3D9TextureInfo::D3D9TextureInfo(const D3D9TextureInfo* info) :
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
D3D9TextureInfo::SetInfo(const D3DXIMAGE_INFO& imageInfo)
{
    this->width = imageInfo.Width;
    this->height = imageInfo.Height;
    this->depth = imageInfo.Depth;
    this->numMipLevels = imageInfo.MipLevels;
    this->pixelFormat = Win360::D3D9Types::AsNebulaPixelFormat(imageInfo.Format);
    switch (imageInfo.ResourceType)
    {
    case D3DRTYPE_TEXTURE:
        this->textureType = Texture::Texture2D;
        break;
    case D3DRTYPE_VOLUMETEXTURE:
        this->textureType = Texture::Texture3D;
        break;
    case D3DRTYPE_CUBETEXTURE:
        this->textureType = Texture::TextureCube;
        break;
    default:
        n_error("unknown texture-type: %i", imageInfo.ResourceType);
        break;
    }
}
} // namespace Resources
//------------------------------------------------------------------------------

#endif