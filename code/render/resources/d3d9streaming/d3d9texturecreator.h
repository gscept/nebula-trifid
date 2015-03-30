#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::D3D9TexturCreator

    A specialized creator for directX9-textures.

    (C) 2010 Radon Labs GmbH
*/

#include "resources/streaming/resourcecreator.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ResourceInfo;
class TextureInfo;

class D3D9TextureCreator : public Resources::ResourceCreator
{
    __DeclareClass(D3D9TextureCreator);
public:
    /// constructor
    D3D9TextureCreator();
    /// destructor
    ~D3D9TextureCreator();

    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const TextureInfo* texInfo);
    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const ResourceInfo* resInfo); 
};
} // namespace Resource
//------------------------------------------------------------------------------