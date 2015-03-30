//------------------------------------------------------------------------------
//  streamingresourcemapper.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resource.h"
#include "streamingresourcemapper.h"
#include "resourceinfo.h"
#include "loadingresource.h"
#include "resources/managedresource.h"
#include "io/uri.h"
#include "resourcecreator.h"
#include "resources/resourceloader.h"
#include "resourcerequestinfo.h"
#include "resourcedictionaryreader.h"
#include "timing/time.h"
#include "timing/timer.h"
#include "resources/resourcemanager.h"
#include "resourcescheduler.h"

namespace Resources
{
    __ImplementClass(Resources::StreamingResourceMapper, 'SRMP', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
StreamingResourceMapper::StreamingResourceMapper(void) :
    maxSize(-1),
    frameIdx(-1),
    resType(0),
    maxNumLoadsParallel(1),
    autoMipMappingEnabled(false)
{
}

//------------------------------------------------------------------------------
/**
*/
StreamingResourceMapper::~StreamingResourceMapper(void)
{
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::SetScheduler(Ptr<ResourceScheduler> scheduler)
{
    if (this->scheduler.isvalid())
    {
        this->scheduler->SetMapper(0);
    }
    this->scheduler = scheduler;
    if (this->scheduler.isvalid())
    {
        this->scheduler->SetMapper(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
SizeT
StreamingResourceMapper::GetNumPendingResources() const
{
    return this->loadingQueue.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::OnAttachToResourceManager()
{
    ResourceMapper::OnAttachToResourceManager();

    // create a placeholder texture every other texture can reference to
    this->placeholder = this->resourceCreator->CreateResource(this->resourceDictionary[this->placeholderResourceId]);
    this->placeholder->SetResourceId(this->placeholderResourceId);
    this->placeholder->SetLoader((ResourceLoader*)this->defaultLoaderClass->Create());
    this->placeholder->SetAsyncEnabled(false);
    ResourceManager::Instance()->RegisterUnmanagedResource(this->placeholder);
    this->placeholder->Load();
    if (!this->placeholder->IsLoaded())
    {
        n_error("StreamingResourceMapper::OnAttachToServer(): could not not load placeholder resource '%s' of class '%s'!\n",
            this->placeholderResourceId.Value(), this->resType->GetName().AsCharPtr());
    }
    //if (Resource::Loaded != this->placeholder->GetState())
    //{
    //    this->placeholder->SetAsyncEnabled(false);
    //    Resource::State result = this->placeholder->Load();
    //    n_assert(Resource::Loaded == result);
    //}

    n_assert(this->resourceDictionary.Size() > 0);
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::OnRemoveFromResourceManager()
{
    // clean up placeholder
    n_assert(this->placeholder.isvalid());
    ResourceManager::Instance()->UnregisterUnmanagedResource(this->placeholder);
    this->placeholder = 0;

    this->SetScheduler(0);

    // clean up resource dict
    this->resourceDictionary.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ManagedResource>
StreamingResourceMapper::OnCreateManagedResource(const ResourceRequestInfo* requestInfo, const Ptr<ResourceLoader>& optResourceLoader)
{
    Ptr<ManagedResource> outputResource;

    // look up resource in active resources if it's already loaded
    IndexT activeResourceIdx = this->activeResources.FindIndex(requestInfo->GetResourceId());
    if (activeResourceIdx != InvalidIndex)
    {
        n_error("Resource already loaded but ResourceManager didn't recognize...");
        // resource is registered in a pool

        // check if it's in loading queue and eventually increase loading-priority
        this->IncreaseLoadingPriority(requestInfo->GetResourceId());

        outputResource = this->activeResources[requestInfo->GetResourceId()];
        n_assert(!outputResource->IsPlaceholder());

        if (outputResource->IsAutoManaged())
        {
            // set to NOT auto-managed if requestInfo says so
            outputResource->SetAutoManaged(requestInfo->IsAutoManaged());
        }
    }
    else
    {
        // resource not loaded nor loading
        // setup new ManagedResource-Ptr
        outputResource = ManagedResource::Create();
        outputResource->IncrClientCount();
        outputResource->SetResourceId(requestInfo->GetResourceId());
        outputResource->SetPlaceholder(this->placeholder);

        this->OnRequestManagedResource(outputResource, requestInfo);
    }
    outputResource->SetFrameId(this->frameIdx);
    return outputResource;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ManagedResource>
StreamingResourceMapper::OnCreateManagedResource(const Core::Rtti& resType, const ResourceId& id, const Ptr<ResourceLoader>& optResourceLoader)
{
    ResourceRequestInfo info(id);
    return this->OnCreateManagedResource(&info, optResourceLoader);
}

//------------------------------------------------------------------------------
/**
    Pass all management logic to scheduler as this enables different scheduling behaviour by switching just the scheduler.
*/
bool
StreamingResourceMapper::OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo)
{
    return this->scheduler->OnRequestManagedResource(managedResource, requestInfo);
}

//------------------------------------------------------------------------------
/**
*/
bool
StreamingResourceMapper::OnRequestManagedResource(const Ptr<ManagedResource>& managedResource)
{
    ResourceRequestInfo info(managedResource->GetResourceId());
    return this->OnRequestManagedResource(managedResource, &info);
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::OnAutoManageResource(const ResourceId& resourceId, bool autoManage)
{
    IndexT resourceIdx = this->activeResources.FindIndex(resourceId);
    if (resourceIdx != InvalidIndex)
    {
        this->activeResources.ValueAtIndex(resourceIdx)->SetAutoManaged(autoManage);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource)
{
    n_assert(managedResource->GetClientCount() > 0);
    if (false == managedResource->GetResource().isvalid())
    {
        managedResource->DecrClientCount();
        n_assert(false == this->activeResources.Contains(managedResource->GetResourceId()));
        return;
    }
    n_assert(this->activeResources.Contains(managedResource->GetResourceId()));
    n_assert(managedResource->GetResourceType() == &this->GetResourceType());
    n_assert(managedResource->GetResource()->IsInstanceOf(*(this->resType)));

    managedResource->DecrClientCount();
    if (managedResource->GetClientCount() == 0)
    {
        IndexT resourceIdx = this->activeResources.FindIndex(managedResource->GetResourceId());
        if (resourceIdx != InvalidIndex)
        {
            this->activeResources.ValueAtIndex(resourceIdx)->SetAutoManaged(true);
            this->activeResources.EraseAtIndex(resourceIdx);
        }
        // abort loading
        IndexT i;
        for (i = 0; i < this->loadingQueue.Size(); i++)
        {
            if (this->loadingQueue[i]->GetManagedResource()->GetResourceId() == managedResource->GetResourceId())
            {
                this->loadingQueue[i]->OnCancelRequest();
                this->loadingQueue.EraseIndex(i);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::InitResourceDict(const IO::URI& fileName)
{
    // read resource-dictionary
    Ptr<ResourceDictionaryReader> resReader = ResourceDictionaryReader::Create();
    resReader->CreateResourceDictionary(fileName, this->resourceDictionary);
}

//------------------------------------------------------------------------------
/**
    Clean up the loading queue (remove loaded items and items whose loading request is out of date)
    and load/unload certain resource details (DoResourceLOD)
*/
void
StreamingResourceMapper::OnPrepare(bool waiting)
{
    this->CleanUpLoadingQueue();
    if (!waiting)
    {
        IndexT resIdx;
        if (this->autoMipMappingEnabled)
        {
            for (resIdx = 0; resIdx < this->activeResources.Size(); resIdx++)
            {
                this->scheduler->DoResourceLOD(this->activeResources.ValueAtIndex(resIdx));
                this->activeResources.ValueAtIndex(resIdx)->ClearRenderStats();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::OnUpdate(IndexT frameIndex)
{
    this->frameIdx = frameIndex;
    this->LoadResources();
}

//------------------------------------------------------------------------------
/**
Cuts all activeResources-connections to Resources and aborts loading of pending resources.
*/
void
StreamingResourceMapper::Reset()
{
    IndexT i;
    // stop loading
    for (i = 0; i < this->loadingQueue.Size(); i++)
    {
        if (Resource::Pending == this->loadingQueue[i]->GetManagedResource()->GetState())
        {
            this->loadingQueue[i]->OnCancelRequest();
        }
    }
    // cut connections of managedResources to Resources
    for (i = 0; i < this->activeResources.Size(); i++)
    {
        this->activeResources.ValueAtIndex(i)->SetResource(0);
    }
    // remove managed resources from active-resources-list
    this->activeResources.Clear();
    this->loadingQueue.Clear();
}

//------------------------------------------------------------------------------
/**
*/
uint
StreamingResourceMapper::GetAllocatedMemory()
{
    n_error("StreamingResourceMapper::GetAllocatedMemory called. Overwrite this method in subclass");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
uint
StreamingResourceMapper::GetUsedMemory()
{
    n_error("StreamingResourceMapper::GetUsedMemory called. Overwrite this method in subclass");
    return 0;
}


//------------------------------------------------------------------------------
/**
*/
const ResourceInfo*
StreamingResourceMapper::GetResourceInfo(const ResourceId& resourceId)
{
    IndexT resourceIdx = this->resourceDictionary.FindIndex(resourceId);
    n_assert2(InvalidIndex != resourceIdx, ("Couldn't find " + resourceId.AsString() + " in resourceDictionary. Check work folder and ensure ResourceDictionaryBuilder created new .dic (check if zip-archive exists)").AsCharPtr());
    return this->resourceDictionary.ValueAtIndex(resourceIdx);
}

//------------------------------------------------------------------------------
/**
    This method starts loading-process of items in the loadingQueue.
    The frameId is used as frameId of currently loaded resources so they
    will have at least the frameId of their ready-to-use time.
*/
void
StreamingResourceMapper::LoadResources()
{
    IndexT i = 0, activeLoads = 0;
    if (0 == this->loadingQueue.Size())
    {
        return;
    }

    while (activeLoads < this->maxNumLoadsParallel && i < this->loadingQueue.Size())
    {
        Resource::State state = this->loadingQueue[i]->GetTargetResource()->GetState();

        // clean up of loaded resources is performed in CleanUpLoadingQueue - not here!
        if (state == Resource::Initial || state == Resource::Pending)
        {
            if (Resource::Pending == this->loadingQueue[i]->GetTargetResource()->Load())
            {
                activeLoads++;
            }
        }
        i++;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::ResourceLoadException(const Ptr<ManagedResource>& resource) const
{
    if (Resource::Failed == resource->GetState())
    {
	n_printf("WARNING: resource '%s' has failed to load!\n", resource->GetResourceId().AsString().AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<LoadingResource>&
StreamingResourceMapper::AppendLoadingResource(const Core::Rtti& loadingType, const Ptr<ManagedResource>& managedResource, const Ptr<Resource>& resourceToLoad)
{
    return this->scheduler->AppendLoadingResource(loadingType, managedResource, resourceToLoad);
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::CleanUpLoadingQueue()
{
    this->scheduler->CleanUpLoadingQueue();
}

//------------------------------------------------------------------------------
/**
*/
void
StreamingResourceMapper::IncreaseLoadingPriority(const ResourceId& resourceId)
{
    if (this->loadingQueue.Size() > 1)
    {
        IndexT resourceIdx;
        for (resourceIdx = 0; resourceIdx < this->loadingQueue.Size(); resourceIdx++)
        {
            if (this->loadingQueue[resourceIdx]->GetManagedResource()->GetResourceId() == resourceId)
            {
                this->loadingQueue[resourceIdx]->SetPriority(this->loadingQueue[resourceIdx]->GetPriority() + 1);
                this->loadingQueue.Sort();
                break;
            }
        }
    }
}
} // namespace Resources
//------------------------------------------------------------------------------
