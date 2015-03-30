#pragma once
#ifndef TOOLS_SHADERSAMPLER_H
#define TOOLS_SHADERSAMPLER_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderSampler
  
    Describes a texture sampler definitions. NOTE: optional parameters
    are represented as empty strings if they haven't been defined in 
    the XML stream.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/xmlreader.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderSampler : public Core::RefCounted
{
    __DeclareClass(ShaderSampler);
public:
    /// constructor
    ShaderSampler();
    /// destructor
    virtual ~ShaderSampler();

    /// set the file URI this sampler has been defined in
    void SetFileURI(const IO::URI& uri);
    /// get the file URI this sampler has been defined in
    const IO::URI& GetFileURI() const;
    /// parse the sampler definition from an open XmlReader
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);

    /// get the sampler's name
    const Util::String& GetName() const;
    /// get the texture parameter name
    const Util::String& GetTextureParamName() const;
    /// get U addressing mode
    const Util::String& GetAddrU() const;
    /// get V addressing mode
    const Util::String& GetAddrV() const;
    /// get W addressing mode
    const Util::String& GetAddrW() const;
    /// get border color
    const Util::String& GetBorderColor() const;
    /// get magnifying filter
    const Util::String& GetMagFilter() const;
    /// get minifying filter
    const Util::String& GetMinFilter() const;
    /// get mipmap filter
    const Util::String& GetMipFilter() const;
    /// get max anisotropy
    const Util::String& GetMaxAnisotropy() const;
    /// get max mipmapping level
    const Util::String& GetMaxMipLevel() const;
    /// get mipmapping LOD bias
    const Util::String& GetMipLodBias() const;
    /// get SRGB texture parameter
    const Util::String& GetSRGBTexture() const;

private:
    IO::URI fileUri;
    Util::String name;
    Util::String textureParamName;
    Util::String addrU;
    Util::String addrV;
    Util::String addrW;
    Util::String borderColor;
    Util::String magFilter;
    Util::String minFilter;
    Util::String mipFilter;
    Util::String maxAnisotropy;
    Util::String maxMipLevel;
    Util::String mipLodBias;
    Util::String srgbTexture;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderSampler::SetFileURI(const IO::URI& uri)
{
    this->fileUri = uri;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShaderSampler::GetFileURI() const
{
    return this->fileUri;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetTextureParamName() const
{
    return this->textureParamName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetAddrU() const
{
    return this->addrU;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetAddrV() const
{
    return this->addrV;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetAddrW() const
{
    return this->addrW;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetBorderColor() const
{
    return this->borderColor;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMagFilter() const
{
    return this->magFilter;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMinFilter() const
{
    return this->minFilter;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMipFilter() const
{
    return this->mipFilter;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMaxAnisotropy() const
{
    return this->maxAnisotropy;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMaxMipLevel() const
{
    return this->maxMipLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetMipLodBias() const
{
    return this->mipLodBias;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderSampler::GetSRGBTexture() const
{
    return this->srgbTexture;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif

