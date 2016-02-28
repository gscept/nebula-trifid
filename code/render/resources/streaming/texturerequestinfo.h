#pragma once

/*
    @class Resources::TextureRequestInfo

    A subclass of ResourceRequestInfo specialized for textures.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file   
*/
#include "resourcerequestinfo.h"

//------------------------------------------------------------------------------
namespace Resources
{
class TextureRequestInfo : public Resources::ResourceRequestInfo
{
public:

    /// constructor
    TextureRequestInfo();
    /// destructor
    ~TextureRequestInfo();
    /// gets the requested minimum mipLevel
    int GetMipLevel() const;
    /// sets the requested minimum mipLevel
    void SetMipLevel(SizeT mipLevel);

protected:
    int mipLevel;
};

//------------------------------------------------------------------------------
/**
*/
inline
TextureRequestInfo::TextureRequestInfo() :
    mipLevel(-1)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
TextureRequestInfo::~TextureRequestInfo()
{
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
TextureRequestInfo::GetMipLevel() const
{
    return this->mipLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TextureRequestInfo::SetMipLevel(SizeT mipLevel)
{
    this->mipLevel = mipLevel;
}
} // namespace Resources
//------------------------------------------------------------------------------