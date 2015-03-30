#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUITexture

    Texture wrapper for crazy eddies GUI system.

    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "resources/managedtexture.h"
#include "cegui/include/CEGUITexture.h"
#include "cegui/include/CEGUISize.h"
#include "cegui/include/CEGUIVector.h"

//------------------------------------------------------------------------------
namespace CEUI
{
class CEUITexture : public CEGUI::Texture
{
public:
    /// constructor
    CEUITexture();
    /// constructor
    CEUITexture(const CEGUI::Size& size);
    /// constructor
    CEUITexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup);
    /// destructor
    virtual ~CEUITexture();

    // implement cegui texture interface

    // returns pixel size of the texture
    virtual const CEGUI::Size& getSize() const;
    // returns the original pixel size of data loaded into texture
    virtual const CEGUI::Size& getOriginalDataSize() const;
    // returns pixel to texel scale values that should be used for converting pixel values to texture co-ords.
    virtual const CEGUI::Vector2& getTexelScaling() const;
    // load texture from file
    virtual void loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup);
    // load texture from memory
    virtual void loadFromMemory(const void* buffer, const CEGUI::Size& buffer_size, PixelFormat pixel_format);
    // write texture to memory
    virtual void saveToMemory(void* buffer);
    
    // Returns the N3 texture object
    const Ptr<CoreGraphics::Texture>& GetTexture() const;
    // returns the N3 managed texture object
    const Ptr<Resources::ManagedTexture>& GetManagedTexture() const;
    // update texure size after texture is loaded
    void UpdateTexture();

    // unload texture
    virtual void Unload();

protected:
    static SizeT texNum;

    /// Update textureSize member from texture object
    void UpdateTextureSize();
    /// Updates cached scale value used to map pixels to texture co-ords.
    void UpdateCachedScaleValues();


    Ptr<Resources::ManagedTexture> texture;

    CEGUI::Size textureSize;
    CEGUI::Size originalDataSize;
    CEGUI::Vector2 texelScaling;
};

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Size&
CEUITexture::getSize() const
{
    n_assert(this->texture.isvalid());
    return this->textureSize;
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Size&
CEUITexture::getOriginalDataSize() const
{
    n_assert(this->texture.isvalid());
    return this->originalDataSize;
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Vector2&
CEUITexture::getTexelScaling() const
{
    n_assert(this->texture.isvalid());
    return this->texelScaling;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
CEUITexture::GetTexture() const
{
    n_assert(this->texture.isvalid());
    return this->texture->GetTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>&
CEUITexture::GetManagedTexture() const
{
    n_assert(this->texture.isvalid());
    return this->texture;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUITexture::UpdateTexture()
{
    n_assert(this->texture.isvalid());
    this->UpdateTextureSize();
    this->UpdateCachedScaleValues();
}

} // namespace CEUI
//------------------------------------------------------------------------------
