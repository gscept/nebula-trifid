#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::D3D11TexturCreator

    A specialized creator for directX11-textures.

    (C) 2010 Radon Labs GmbH
    (C) 2013 Individual contributors, see AUTHORS file
*/

#include "resources/streaming/resourcecreator.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ResourceInfo;
class TextureInfo;

class D3D11TextureCreator : public Resources::ResourceCreator
{
    __DeclareClass(D3D11TextureCreator);
public:
    /// constructor
    D3D11TextureCreator();
    /// destructor
    ~D3D11TextureCreator();

    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const TextureInfo* texInfo);
    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const ResourceInfo* resInfo); 
};
} // namespace Resource
//------------------------------------------------------------------------------