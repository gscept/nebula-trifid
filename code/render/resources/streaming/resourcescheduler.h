#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceScheduler

    A ResourceScheduler manages all incoming resource requests for
    a certain StreamingResourceMapper.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
#include "core/refcounted.h"

namespace Resources
{
class ManagedResource;
class Resource;
class StreamingResourceMapper;
class ResourceRequestInfo;
class LoadingResource;

class ResourceScheduler : public Core::RefCounted
{
    __DeclareClass(ResourceScheduler);

public:
    /// sets the StreamingResourceMapper to manage
    virtual void SetMapper(const Ptr<StreamingResourceMapper>& mapper);

    /// appends resource to loading-queue
    virtual const Ptr<LoadingResource>& AppendLoadingResource(const Core::Rtti& loadingType, const Ptr<ManagedResource>& managedResource, const Ptr<Resource>& resourceToLoad);
    /// cleans the loading queue
    virtual void CleanUpLoadingQueue();
    /// analyzes and reacts on changes of the distance of given ManagedResource
    virtual void DoResourceLOD(const Ptr<ManagedResource>& managedResource);
    /// tries to load a resource and returns true if request was successful
    virtual bool OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo);

    /// call this as the scheduler is removed from its mapper
    virtual void OnRemoveFromMapper();

protected:
    Ptr<StreamingResourceMapper> mapper;
};
} // namespace Resources
//------------------------------------------------------------------------------
