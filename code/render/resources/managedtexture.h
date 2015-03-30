#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ManagedTexture
    
    A specialized managed resource for texture resources.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "resources/managedresource.h"
#include "coregraphics/texture.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ManagedTexture : public ManagedResource
{
    __DeclareClass(ManagedTexture);
public:
    /// get contained texture resource or placeholder if resource is invalid or not loaded
    const Ptr<CoreGraphics::Texture>& GetTexture() const;

    /// get contained texture resource and under no circumstances the placeholder - asserts resource is valid!
    const Ptr<CoreGraphics::Texture>& GetTextureUnloaded() const;

};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
ManagedTexture::GetTexture() const
{
    return this->GetLoadedResource().downcast<CoreGraphics::Texture>();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
ManagedTexture::GetTextureUnloaded() const
{
    n_assert(this->GetResource().isvalid());
    return this->GetResource().downcast<CoreGraphics::Texture>();
}

} // namespace Resources
//------------------------------------------------------------------------------
    