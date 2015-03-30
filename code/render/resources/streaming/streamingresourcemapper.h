#pragma once

/*
    @class Resources::StreamingResourceMapper

    A StreamingResourceMapper can be used for all dynamically streamed resources.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file   
*/

#include "resources/resourcemapper.h"
#include "core/refcounted.h"
#include "resources/resourceid.h"
#include "resourceinfo.h"
#include "resources/managedresource.h"
#include "io/uri.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceRequestInfo;
class LoadingResource;
class Resource;
class ResourceCreator;
class ResourceScheduler;

class StreamingResourceMapper : public Resources::ResourceMapper
{
    __DeclareClass(StreamingResourceMapper);

    friend class ResourceScheduler;
    friend class TexturePoolMapperScheduler;

public:
    /// constructor
    StreamingResourceMapper(void);
    /// destructor
    ~StreamingResourceMapper(void);

    /// return the number of currently pending resources
    virtual SizeT GetNumPendingResources() const;
    /// get resource type handled by this resource mapper
    virtual const Core::Rtti& GetResourceType() const;
    /// sets the resource type
    virtual void SetResourceType(const Core::Rtti& resType);
    /// returns the current frame Index
    IndexT GetFrameIndex() const;
    /// sets the default loader class
    void SetResourceLoaderClass(const Core::Rtti& loaderType);
    /// set the resourceScheduler this mapper shall use for management
    void SetScheduler(Ptr<ResourceScheduler> scheduler);

    /// called from resource manager when mapper is attached
    virtual void OnAttachToResourceManager(); 
    /// called from resource manager when mapper is removed
    virtual void OnRemoveFromResourceManager();
    /// called when a managed resource should be created (
    virtual Ptr<ManagedResource> OnCreateManagedResource(const ResourceRequestInfo* requestInfo, const Ptr<ResourceLoader>& optResourceLoader);
    /// called when a managed resource should be created (wrapper)
    virtual Ptr<ManagedResource> OnCreateManagedResource(const Core::Rtti& resType, const ResourceId& resourceId, const Ptr<ResourceLoader>& optResourceLoader);
    /// tries to load a resource and returns true if request was successful (calls scheduler)
    virtual bool OnRequestManagedResource(const Ptr<ManagedResource>& managedResource, const ResourceRequestInfo* requestInfo);
    /// tries to load a resource and returns true if request was successful (wrapper)
    virtual bool OnRequestManagedResource(const Ptr<ManagedResource>& managedResource);

    /// sets auto-manage-flag of appropriate slot to given value
    virtual void OnAutoManageResource(const ResourceId& resourceId, bool autoManage = true);
    /// called when a managed resource should be discarded
    virtual void OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource);

    /// loads the given resource dictionary for this mapper reading from given file
    virtual void InitResourceDict(const IO::URI& fileName);
    /// called before gathering render stats
    virtual void OnPrepare(bool waiting);
    /// called after gathering render stats to perform resource management
    virtual void OnUpdate(IndexT frameIndex);
    /// resets all resource-connections and restores initial-state
    virtual void Reset();

    /// interface for subclasses - do not call directly!
    /// returns allocated resource space of all pools in bytes
    virtual uint GetAllocatedMemory();
    /// interface for subclasses - do not call directly!
    /// returns used memory of all pools in bytes
    virtual uint GetUsedMemory();
    /// set if mip maps should be managed automagically
    void SetAutoMipMapping(bool enabled);

protected:
    /// returns appropriate ResourceInfo for given ResourceId by looking it up in info-table
    virtual const ResourceInfo* GetResourceInfo(const ResourceId& resourceId);

    /// starts loading of one or more items in the loadingQueue (called per frame)
    virtual void LoadResources();
    /// this is called if loading a resource is canceled or failed (currently printing some debug-informations)
    virtual void ResourceLoadException(const Ptr<ManagedResource>& resource) const;

    /// appends resource to loading-queue
    virtual const Ptr<LoadingResource>& AppendLoadingResource(const Core::Rtti& loadingType, const Ptr<ManagedResource>& managedResource, const Ptr<Resource>& resourceToLoad);
    /// cleans the loading queue
    virtual void CleanUpLoadingQueue();
    /// checks if the given resource is in loading queue and if so it may increase loading-priority 
    virtual void IncreaseLoadingPriority(const ResourceId& resourceId);

    /// maximum total size in bytes
    uint maxSize;
    const Core::Rtti* defaultLoaderClass;
    Ptr<Resource> placeholder;
    IndexT frameIdx;
    const Core::Rtti* resType;
    Ptr<ResourceScheduler> scheduler;

    /// resource creator used for allocating memory on initialization
    Ptr<ResourceCreator> resourceCreator;

    Util::Dictionary<ResourceId, Ptr<ManagedResource>> activeResources;
    Util::Array<Ptr<LoadingResource>> loadingQueue;
    Util::Dictionary<ResourceId, ResourceInfo*> resourceDictionary;
    bool autoMipMappingEnabled;
    SizeT maxNumLoadsParallel;
};

//------------------------------------------------------------------------------
/**
*/
const inline Core::Rtti&
StreamingResourceMapper::GetResourceType() const
{
    return *this->resType;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
StreamingResourceMapper::GetFrameIndex() const
{
    return this->frameIdx;
}

//------------------------------------------------------------------------------
/**
*/
inline void
StreamingResourceMapper::SetResourceType(const Core::Rtti& resType)
{
    this->resType = &resType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
StreamingResourceMapper::SetResourceLoaderClass(const Core::Rtti& loaderType)
{
    this->defaultLoaderClass = &loaderType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
StreamingResourceMapper::SetAutoMipMapping(bool enabled)
{
    this->autoMipMappingEnabled = enabled;
}


} // namespace Resources
//------------------------------------------------------------------------------
