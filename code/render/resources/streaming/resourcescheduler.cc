//------------------------------------------------------------------------------
//  resourcescheduler.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcescheduler.h"
#include "streamingresourcemapper.h"
#include "loadingresource.h"

namespace Resources
{
    __ImplementClass(Resources::ResourceScheduler, 'RSSC', Core::RefCounted);


//------------------------------------------------------------------------------
/**
*/
void
ResourceScheduler::SetMapper(const Ptr<StreamingResourceMapper>& mapper)
{
    this->mapper = mapper;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<LoadingResource>&
ResourceScheduler::AppendLoadingResource(const Core::Rtti& loadingType, const Ptr<ManagedResource>& managedResource, const Ptr<Resource>& resourceToLoad)
{
    Ptr<LoadingResource> newLoadingResource = (LoadingResource*)loadingType.Create();
    newLoadingResource->SetManagedResource(managedResource);
    newLoadingResource->SetTargetResource(resourceToLoad);
    newLoadingResource->SetPriority(managedResource->GetPriority());
    IndexT i;
    // @todo: overwrite < operator for array.sort (?)
    for (i = 0; i < this->mapper->loadingQueue.Size(); i++)
    {
        if (newLoadingResource->GetPriority() > this->mapper->loadingQueue[i]->GetPriority())
        {
            break;
        }
    }
    n_assert(resourceToLoad->GetLoader().isvalid());
    this->mapper->loadingQueue.Insert(i, newLoadingResource);
    if (!this->mapper->activeResources.Contains(resourceToLoad->GetResourceId()))
    {
        this->mapper->activeResources.Add(resourceToLoad->GetResourceId(), managedResource);
    }
    return this->mapper->loadingQueue[i];
}

//------------------------------------------------------------------------------
/**
    As a very basic this method cleans up loadingQueue and removes
    loaded Resources. If loading failed or canceled StreamingResourceMapper::ResourceLoadException
    is called appropriate Resource.
    Overload this method in subclasses to perform additional clean-ups of the loadingQueue
    like kicking out requests which are out of date.
*/
void
ResourceScheduler::CleanUpLoadingQueue()
{
    // clean up loadingQueue
    IndexT i = 0;
    while (i < this->mapper->loadingQueue.Size())
    {
        const Ptr<ManagedResource>& res = this->mapper->loadingQueue[i]->GetManagedResource();
        switch (this->mapper->loadingQueue[i]->GetTargetResource()->GetState())
        {
        case Resource::Initial:
        case Resource::Pending:
            i++;
            break;
        case Resource::Loaded:
            this->mapper->loadingQueue[i]->OnSuccessRequest();
            this->mapper->loadingQueue.EraseIndex(i);
            break;
        case Resource::Cancelled:
        case Resource::Failed:
            this->mapper->ResourceLoadException(res);
            this->mapper->loadingQueue[i]->OnCancelRequest();
            this->mapper->loadingQueue.EraseIndex(i);
            break;
        default:
            n_error("invalid LoadingState");
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    In subclasses this Method may auto-manage e.g. changement of mip Maps.
*/
void
ResourceScheduler::DoResourceLOD(const Ptr<ManagedResource>& managedResource)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Overwrite this method in subclasses!
*/
bool
ResourceScheduler::OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo)
{
    n_error("ResourceScheduler::OnRequestManagedResource called. Overwrite this method in subclass.");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceScheduler::OnRemoveFromMapper()
{
    this->mapper = 0;
}
} // namespace Resources
//------------------------------------------------------------------------------
