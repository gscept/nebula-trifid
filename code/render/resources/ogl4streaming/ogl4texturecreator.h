#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::OGL4TexturCreator

    A specialized creator for directX9-textures.

    (C) 2010 Radon Labs GmbH
	(C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "resources/streaming/resourcecreator.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ResourceInfo;
class TextureInfo;

class OGL4TextureCreator : public Resources::ResourceCreator
{
    __DeclareClass(OGL4TextureCreator);
public:
    /// constructor
    OGL4TextureCreator();
    /// destructor
    ~OGL4TextureCreator();

    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const TextureInfo* texInfo);
    /// creates a new resources on a specific platform    
    virtual Ptr<Resource> CreateResource(const ResourceInfo* resInfo); 
};
} // namespace Resource
//------------------------------------------------------------------------------