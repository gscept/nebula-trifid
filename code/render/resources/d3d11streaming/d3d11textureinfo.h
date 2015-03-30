#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::D3D11TextureInfo

    D3D11TextureInfo holds several informations of a texture, like height, width, ...

    (C) 2010 Radon Labs GmbH
    (C) 2013 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/streaming/resourceinfo.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/texture.h"
#include "coregraphics/d3d11/d3d11types.h"
#include "d3dx11tex.h"

//------------------------------------------------------------------------------
namespace Resources
{

class D3D11TextureInfo : public Resources::ResourceInfo
{
public:
    /// constructor
    D3D11TextureInfo(void);
    D3D11TextureInfo(const D3D11TextureInfo* info);
    /// destructor
    ~D3D11TextureInfo(void);

    /// returns true if elements are equal
    virtual bool IsEqual(const ResourceInfo* info) const;
    /// returns true if elements are equal
    virtual bool IsEqual(const D3D11TextureInfo* info) const;

    /// get texture type
    CoreGraphics::Texture::Type GetType() const;
    /// get width of texture 
    uint GetWidth() const;
    /// get height of texture (if 2d or 3d texture)
    uint GetHeight() const;
    /// get depth of texture (if 3d texture)
    uint GetDepth() const;
    /// get number of mip levels
    uint GetMipLevels() const;
    /// get pixel format of the texture
    CoreGraphics::PixelFormat::Code GetPixelFormat() const;

    void SetInfo(const D3DX11_IMAGE_INFO& imageInfo);

    /// set texture type
    void SetType(CoreGraphics::Texture::Type t);
    /// set texture width
    void SetWidth(uint w);
    /// set texture height
    void SetHeight(uint h);
    /// set texture depth
    void SetDepth(uint d);
    /// set number of mip levels
    void SetMipLevels(uint n);
    /// set pixel format
    void SetPixelFormat(CoreGraphics::PixelFormat::Code f);

protected:
    uint width;
    uint height;
    uint depth;
    uint numMipLevels;
    CoreGraphics::PixelFormat::Code pixelFormat;
    CoreGraphics::Texture::Type textureType;
};

//------------------------------------------------------------------------------
/**
*/
inline
D3D11TextureInfo::~D3D11TextureInfo()
{}

//------------------------------------------------------------------------------
/**
*/
inline bool
D3D11TextureInfo::IsEqual(const D3D11TextureInfo* info) const
{
    return this->width == info->width &&
        this->height == info->height &&
        this->depth == info->depth &&
        this->numMipLevels == info->numMipLevels &&
        this->textureType == info->textureType &&
        this->pixelFormat == info->pixelFormat;        
}

//------------------------------------------------------------------------------
/**
*/
inline bool
D3D11TextureInfo::IsEqual(const ResourceInfo* info) const
{
    return this->IsEqual((const D3D11TextureInfo*)(info));        
}


//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetType(CoreGraphics::Texture::Type t)
{
    this->textureType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::Texture::Type
D3D11TextureInfo::GetType() const
{
    return this->textureType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetWidth(uint w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
D3D11TextureInfo::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetHeight(uint h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
D3D11TextureInfo::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetDepth(uint d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
D3D11TextureInfo::GetDepth() const
{
    return this->depth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetMipLevels(uint n)
{
    this->numMipLevels = n;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
D3D11TextureInfo::GetMipLevels() const
{
    return this->numMipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D11TextureInfo::SetPixelFormat(CoreGraphics::PixelFormat::Code f)
{
    this->pixelFormat = f;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::PixelFormat::Code
D3D11TextureInfo::GetPixelFormat() const
{
    return this->pixelFormat;
}


} // namespace Resources
//------------------------------------------------------------------------------