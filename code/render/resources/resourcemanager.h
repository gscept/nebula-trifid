#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceManager
    
    The ResourceManager adds a management layer between resource using clients
    and actual resource objects. The main purpose of the manager is
    resource streaming for large seamless worlds. Resource users request 
    a ManagedResource object from the ResourceManager. A ManagedResource is 
    a wrapper around an actual Resource object, which may change based on 
    the resource management strategy implemented by the manager. The 
    main job of a resource manager is to provide all the resource required for 
    rendering while making the best use of limited resource memory. It will also 
    care about background loading of resources, and provide placeholder
    resources if a resource hasn't finished loading.

    While ManagedResources are managed by their appropriate Mappers
    unmanaged resources are externally created and a reference is stored by the
    ResourceManager. Primarily these are RenderTargets or other Resources which
    are 'never' thrown away.
    
    The actual resource management strategies for different resource types
    are customizable by attaching ResourceMapper objects to the 
    ResourceManager. A ResourceMapper analyzes the usage statistics of existing 
    ManagedResource objects and implements a specific resource management pattern 
    using the following basic operations:

    - Load(pri, lod): asynchronously load a resource from external 
      storage into memory given a priority and a level-of-detail.
    - Discard: completely unload a resource, freeing up limited
      resource memory.
    - Upgrade(lod): upgrade a loaded resource to a higher level-of-detail
    - Degrade(lod): degrade a loaded resource to a lower level-of-detail

    If ResourceMapper is a subclass of StreamingResourceMapper a certain ResourceScheduler
    can be attached on the fly to change management strategy any time.
    
    (C) 2007 Radon Labs GmbH	
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "resources/resourceid.h"
#include "timing/time.h"
#include "resources/resourceloader.h"
#include "resources/resourcesaver.h"

namespace Core
{
class Rtti;
}

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ResourceMapper;
class ManagedResource;
class PoolResourceMapper;

class ResourceManager : public Core::RefCounted
{
    __DeclareClass(ResourceManager);
    __DeclareSingleton(ResourceManager);
public:
    /// constructor
    ResourceManager();
    /// destructor
    virtual ~ResourceManager();

    /// open the resource manager
    void Open();
    /// close the resource manager
    void Close();
    /// return true if resource manager is open
    bool IsOpen() const;

    /// register a resource mapper (resource type is defined by mapper)
    void AttachMapper(const Ptr<ResourceMapper>& mapper);
    /// unregister a resource mapper by resource type
    void RemoveMapper(const Core::Rtti& resourceType);
    /// unregister all mappers
    void RemoveAllMappers();
    /// return true if a mapper has been registered for the given resource type
    bool HasMapper(const Core::Rtti& resourceType) const;
    /// get the resource mapper registered with a resource type
    const Ptr<ResourceMapper>& GetMapperByResourceType(const Core::Rtti& resourceType) const;
	/// disables/enables asynchronous mapping
	void SetMappersAsync(bool b);

    /// create a ManagedResource object (bumps usecount on existing resource)
    Ptr<ManagedResource> CreateManagedResource(const Core::Rtti& resType, const ResourceId& id, const Ptr<ResourceLoader>& optResourceLoader = 0, bool forceSync = false);
    /// reloads an unloaded resource into cache
    void RequestResourceForLoading(const Ptr<ManagedResource>& managedResource);
    /// unregister a ManagedResource object
    void DiscardManagedResource(const Ptr<ManagedResource>& managedResource);
    /// return true if a managed resource exists
    bool HasManagedResource(const ResourceId& id) const;
	/// return true if an unmanaged resource exists
	bool IsResourceUnmanaged(const ResourceId& resId) const;
    /// lookup a managed resource (does not change usecount of resource)
    const Ptr<ManagedResource>& LookupManagedResource(const ResourceId& id) const;
    /// set if given resource whether should be autoManaged or not
    void AutoManageManagedResource(const ResourceId& id, bool autoManage);

    /// prepare stats gathering, call per frame
    void Prepare(bool waiting);
    /// perform actual resource management, call per frame
    void Update(IndexT frameIdx);
    /// test if any resources are pending, returns true if not resources are pending
    bool CheckPendingResources();
    /// wait until pending resources are loaded, or time-out is reached (returns false if time-out)
    bool WaitForPendingResources(Timing::Time timeOut);

    // --- unmanaged resources related ---
    Ptr<Resource> CreateUnmanagedResource(const ResourceId& resId, const Core::Rtti& resClass, const Ptr<ResourceLoader>& loader = 0, const Ptr<ResourceSaver>& saver = 0);
    // (functionalities of the previously used SharedResourceServer)
    /// register an existing resource object as shared resource
    void RegisterUnmanagedResource(const Ptr<Resource>& res);
    /// unregister a shared resource (necessary for managing the use count)
    void UnregisterUnmanagedResource(const Ptr<Resource>& res);
    /// unregister a shared resource by resource name
    void UnregisterUnmanagedResource(const ResourceId& id);

    // @todo: remove this method and related stuff as new resource management won't need this (at least it shouldn't ...)
    /// increments use count of all resources
    void HoldResources();
    // @todo: remove this method and related stuff as new resource management won't need this (at least it shouldn't ...)
    /// decrements use count of all resources
    void ReleaseResources();

    // --- debug related ---
    /// return true if a shared resource exists
    bool HasResource(const ResourceId& id) const;
    /// lookup a shared resource
    const Ptr<Resource>& LookupResource(const ResourceId& id) const;
    /// get shared resources by type (slow)
    Util::Array<Ptr<Resource> > GetResourcesByType(const Core::Rtti& type) const;

private:
    IndexT frameIdx;
    bool isOpen;
    Util::Dictionary<const Core::Rtti*,Ptr<ResourceMapper> > mappers;        //> resource schedulers by resource type
    Util::Dictionary<ResourceId,Ptr<ManagedResource> > managedResources;     //> managed resources by name
    Util::Dictionary<ResourceId,Ptr<Resource> > unmanagedResources;     //> unmanaged resource by name

    // @todo: remove this variables and related stuff as new resource management won't need this (at least it shouldn't ...)
    bool resourcesHolded;
    Util::Array<Ptr<Resource> > holdedResources;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceManager::IsOpen() const
{
    return this->isOpen;
}
} // namespace Resources
//------------------------------------------------------------------------------

