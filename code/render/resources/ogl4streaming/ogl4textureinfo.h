#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::OGL4TextureInfo

    OGL4TextureInfo holds texture information such as like height, width, format etc ...

    (C) 2010 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "resources/streaming/resourceinfo.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/texture.h"
#include "coregraphics/ogl4/ogl4types.h"
#include "IL/il.h"

//------------------------------------------------------------------------------
namespace Resources
{

class OGL4TextureInfo : public Resources::ResourceInfo
{
public:
    /// constructor
    OGL4TextureInfo(void);

	/// copy constructor
    OGL4TextureInfo(const OGL4TextureInfo* info);

    /// destructor
    ~OGL4TextureInfo(void);

    /// returns true if elements are equal
    virtual bool IsEqual(const ResourceInfo* info) const;
    /// returns true if elements are equal
    virtual bool IsEqual(const OGL4TextureInfo* info) const;

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

	/// set info from IL image
    void SetInfo(ILint image);

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
OGL4TextureInfo::~OGL4TextureInfo()
{}

//------------------------------------------------------------------------------
/**
*/
inline bool
OGL4TextureInfo::IsEqual(const OGL4TextureInfo* info) const
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
OGL4TextureInfo::IsEqual(const ResourceInfo* info) const
{
    return this->IsEqual((const OGL4TextureInfo*)(info));        
}


//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetType(CoreGraphics::Texture::Type t)
{
    this->textureType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::Texture::Type
OGL4TextureInfo::GetType() const
{
    return this->textureType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetWidth(uint w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
OGL4TextureInfo::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetHeight(uint h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
OGL4TextureInfo::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetDepth(uint d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
OGL4TextureInfo::GetDepth() const
{
    return this->depth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetMipLevels(uint n)
{
    this->numMipLevels = n;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
OGL4TextureInfo::GetMipLevels() const
{
    return this->numMipLevels;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4TextureInfo::SetPixelFormat(CoreGraphics::PixelFormat::Code f)
{
    this->pixelFormat = f;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::PixelFormat::Code
OGL4TextureInfo::GetPixelFormat() const
{
    return this->pixelFormat;
}


} // namespace Resources
//------------------------------------------------------------------------------