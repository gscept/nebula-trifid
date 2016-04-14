#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::TexturePoolMapperScheduler

    A simple (example) for a ResourceScheduler for TexturePools
    based on NRU (not recently used) algorithm.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
#include "resourcescheduler.h"

namespace Resources
{
class ManagedTexture;
class TextureRequestInfo;
class PoolResourceMapper;

class TexturePoolMapperScheduler : public Resources::ResourceScheduler
{
    __DeclareClass(TexturePoolMapperScheduler);

public:
    /// constructor
    TexturePoolMapperScheduler();
    /// destructor
    ~TexturePoolMapperScheduler();

    /// sets the ResourceMapper and the PoolResourceMapper (downcasts the ResourceMapper)
    virtual void SetMapper(const Ptr<StreamingResourceMapper>& mapper);

    /// analyzes and reacts on changes of the distance of given ManagedResource
    virtual void DoResourceLOD(const Ptr<ManagedResource>& managedResource);
    /// tries to load a resource and returns true if request was successful
    virtual bool OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo);

    /// call this as the scheduler is removed from its mapper
    virtual void OnRemoveFromMapper();

protected:
    /// tries to copy as much texture data from texture in memory to another slot fitting new mipMap level
    virtual bool OnRequestOtherMipMap(const Ptr<ManagedTexture>& managedTexture, const TextureRequestInfo* requestInfo);
    
    /// points to same target as ResourceScheduler::mapper but we do need this as we want to have some
    /// PoolResourceMapper specific functionalities
    Ptr<PoolResourceMapper> poolMapper;
};
} // namespace Resources
//------------------------------------------------------------------------------
