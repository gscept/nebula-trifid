//------------------------------------------------------------------------------
//  resourcemanager.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourcemanager.h"
#include "resources/resourcemapper.h"
#include "resources/managedresource.h"
#include "timing/timer.h"
#include "coregraphics/texture.h"
#include "streaming/resourcerequestinfo.h"
#include "resource.h"
#include "core/rtti.h"
#if __WIN32__
#include "resources/streaming/poolresourcemapper.h"
#endif

namespace Resources
{
__ImplementClass(Resources::ResourceManager, 'RMGR', Core::RefCounted);
__ImplementSingleton(Resources::ResourceManager);

using namespace Core;
using namespace Util;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
ResourceManager::ResourceManager() :
    isOpen(false),
    frameIdx(0),
    resourcesHolded(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ResourceManager::~ResourceManager()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::Open()
{
    n_assert(!this->IsOpen());
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::Close()
{
    n_assert(this->IsOpen());
    this->RemoveAllMappers();
    this->isOpen = false;
    this->frameIdx = 0;

    // discard all unmanaged resources
    IndexT i;
    for (i = 0; i < this->unmanagedResources.Size(); i++)
    {
        const Ptr<Resource>& res = this->unmanagedResources.ValueAtIndex(i);
        res->Unload();
        res->SetLoader(0);
        res->SetSaver(0);
    }
    this->unmanagedResources.Clear();

    for (i = 0; i < this->managedResources.Size(); i++)
    {
		n_printf("Managed resource:%s is still active!\n", managedResources.ValueAtIndex(i)->GetResourceId().AsString().AsCharPtr());
    }
    //// shutdown resource dictionary
    //if (this->resourceDictionary->IsValid())
    //{
    //    this->resourceDictionary->Unload();
    //}
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::AttachMapper(const Ptr<ResourceMapper>& mapper)
{
    n_assert(this->IsOpen());
    n_assert(mapper.isvalid());
    n_assert(!this->mappers.Contains(&(mapper->GetResourceType())));
    this->mappers.Add(&(mapper->GetResourceType()), mapper);
    mapper->OnAttachToResourceManager();
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::RemoveMapper(const Core::Rtti& resType)
{
    n_assert(this->IsOpen());
    n_assert(this->HasMapper(resType));
    this->mappers[&resType]->OnRemoveFromResourceManager();
    this->mappers.Erase(&resType);
}
   
//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::RemoveAllMappers()
{
    n_assert(this->IsOpen());
    while (this->mappers.Size() > 0)
    {
        this->mappers.ValueAtIndex(0)->OnRemoveFromResourceManager();
        this->mappers.EraseAtIndex(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceManager::HasMapper(const Rtti& resType) const
{
    n_assert(this->IsOpen());
    return this->mappers.Contains(&resType);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceMapper>&
ResourceManager::GetMapperByResourceType(const Rtti& resType) const
{
    n_assert(this->IsOpen());
    n_assert(this->HasMapper(resType));
    return this->mappers[&resType];
}

//------------------------------------------------------------------------------
/**
    This method must be called per-frame before rendering begins. This will 
    call the OnPrepare() method on all attached resource mappers, which will 
    at least reset the render statistics in the managed resource.
*/
void
ResourceManager::Prepare(bool waiting)
{
    n_assert(this->IsOpen());
    IndexT i;
    for (i = 0; i < this->mappers.Size(); i++)
    {
        this->mappers.ValueAtIndex(i)->OnPrepare(waiting);
    }
}

//------------------------------------------------------------------------------
/**
    This method must be called by the application after render statistics 
    have been gathered and before the actual rendering. The ResourceManager
    will call the OnUpdate() method on all attached resource mappers. This is 
    the place where the actual resource management will happen. This is also where
	watched resources are checked whether or not their resource has been altered, and if such
	the resource will be tagged for reloading.
*/
void
ResourceManager::Update(IndexT frameIdx)
{
    n_assert(this->IsOpen());
    this->frameIdx = frameIdx;
    IndexT i;
    for (i = 0; i < this->mappers.Size(); i++)
    {
        this->mappers.ValueAtIndex(i)->OnUpdate(this->frameIdx);
    }
}

//------------------------------------------------------------------------------
/**
    Create a shared ResourceManager object. If a managed resource with the same
    resource name already exists, its client count will be incremented
    and it will be returned. Otherwise the right ResourceMapper will
    be asked to create a new managed resource.
*/
Ptr<ManagedResource>
ResourceManager::CreateManagedResource(const Core::Rtti& resType, const ResourceId& resId, const Ptr<ResourceLoader>& optResourceLoader, bool forceSync)
{
    n_assert(this->IsOpen());
    // HACK: for unconverted N2 textures using textures: - assignment instead of tex:
    // and systex: (--> tex:)
    Util::String resString = resId.AsString();
    resString.SubstituteString("textures:", "tex:");
    resString.SubstituteString("systex:", "tex:");

    ResourceId hackId(resString);

    IndexT i = this->managedResources.FindIndex(hackId);
    if (InvalidIndex != i)
    {
        // yes exists, increment client count and return existing managed resource
        const Ptr<ManagedResource>& managedResource = this->managedResources.ValueAtIndex(i);
        n_assert(managedResource.isvalid());
        n_assert(&resType == managedResource->GetResourceType());
        managedResource->IncrClientCount();
        return managedResource;
    }
    else
    {
        // managed resource doesn't exist yet, ask the right resource mapper to create a new one
        n_assert(this->HasMapper(resType));
        Ptr<ManagedResource> managedResource = this->mappers[&resType]->OnCreateManagedResource(resType, hackId, optResourceLoader, forceSync);
        n_assert(managedResource.isvalid());
		//managedResource->IncrClientCount();
        this->managedResources.Add(hackId, managedResource);
        return managedResource;
    }
}

//------------------------------------------------------------------------------
/**
    Discard a shared ManagedResource object. This will decrement the
    client count. If the client count reaches zero, the ManagedResource
    object will be released as well.
*/
void
ResourceManager::DiscardManagedResource(const Ptr<ManagedResource>& managedResource)
{
    n_assert(this->IsOpen());
    n_assert(this->managedResources.Contains(managedResource->GetResourceId()));
    this->mappers[managedResource->GetResourceType()]->OnDiscardManagedResource(managedResource);
    if (managedResource->GetClientCount() == 0)
    {
        this->managedResources.Erase(managedResource->GetResourceId());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceManager::HasManagedResource(const ResourceId& resId) const
{
    n_assert(this->IsOpen());
    return this->managedResources.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceManager::IsResourceUnmanaged(const ResourceId& resId) const
{
	n_assert(this->IsOpen());
	return this->unmanagedResources.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ManagedResource>&
ResourceManager::LookupManagedResource(const ResourceId& resId) const
{
    n_assert(this->IsOpen());
    return this->managedResources[resId];
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::AutoManageManagedResource(const ResourceId& id, bool autoManage)
{
    IndexT resIdx = this->managedResources.FindIndex(id);
    if (InvalidIndex != resIdx)
    {
        this->managedResources.ValueAtIndex(resIdx)->SetAutoManaged(autoManage);
    }
}

//------------------------------------------------------------------------------
/**
    This method tests if there are pending resource which haven't been loaded
    yet. Returns true if there are no pending resources (everything has 
    loaded), false if pending resources exist.
*/
bool
ResourceManager::CheckPendingResources()
{
    SizeT numPending = 0;
    IndexT i;
    for (i = 0; i < this->mappers.Size(); i++)
    {
        numPending += this->mappers.ValueAtIndex(i)->GetNumPendingResources();
    }
    if (0 == numPending)
    {
        // everything has been loaded
        return true;
    }
    else
    {
        // pending resources exist
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method blocks until all pending resources are loaded, or until
    a time-out occurs. If a time-out occurs the method will return false,
    otherwise true.
*/
bool
ResourceManager::WaitForPendingResources(Time timeOut)
{
    // this basically runs a local resource update loop until 
    // everything is loaded or the timeout is reached
    Timer timer;                                    // not necessary to use slavetime here, just a local timer
    timer.Start();
    bool timedOut = false;
    bool allLoaded = false;
    do
    {
        this->Prepare(true);
        this->Update(this->frameIdx);
        if (this->CheckPendingResources())
        {
            allLoaded = true;
        }
        timedOut = (timeOut != 0.0) && (timer.GetTime() > timeOut);
        Timing::Sleep(0.01);
    }
    while (!(allLoaded || timedOut));
    return allLoaded;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::RequestResourceForLoading(const Ptr<ManagedResource>& managedResource)
{
	// empty (remove?) o
}

//------------------------------------------------------------------------------
/**
    Look up resource in registered unmanaged and managed resources.
*/
bool
ResourceManager::HasResource(const ResourceId& resId) const
{
    n_assert(resId.IsValid());
    // @todo: return false if ManagedResource is valid but not loaded?
    return this->unmanagedResources.Contains(resId) || this->managedResources.Contains(resId);
}

//------------------------------------------------------------------------------
/**
    Look up resource in registered unmanaged and managed resources.
*/
const Ptr<Resource>&
ResourceManager::LookupResource(const ResourceId& resId) const
{
    IndexT resIdx = this->unmanagedResources.FindIndex(resId);
    if (InvalidIndex != resIdx)
    {
        return this->unmanagedResources.ValueAtIndex(resIdx);
    }
    else
    {
        resIdx = this->managedResources.FindIndex(resId);
        n_assert(InvalidIndex != resIdx);
        return this->managedResources.ValueAtIndex(resIdx)->GetResource();
    }    
}

//------------------------------------------------------------------------------
/**
    Register an existing resource object as unmanaged resource. If the resource
    already has been registered, an assertion will be thrown. This will
    increment the use count of the resource by one.
*/
void
ResourceManager::RegisterUnmanagedResource(const Ptr<Resource>& res)
{
    n_assert(res.isvalid());
    n_assert(res->GetResourceId().IsValid());
    n_assert(!this->unmanagedResources.Contains(res->GetResourceId()));
    res->IncrUseCount();
    this->unmanagedResources.Add(res->GetResourceId(), res);
}

//------------------------------------------------------------------------------
/**
    Unregister an unmanaged resource. This will decrement the use count of the
    resource. If the use count has reached zero, the resource will be
    discarded (unloaded and removed from the unmanaged resource pool).
*/
void
ResourceManager::UnregisterUnmanagedResource(const Ptr<Resource>& res)
{
    n_assert(res.isvalid());
    n_assert(res->GetResourceId().IsValid());
    n_assert(this->unmanagedResources.Contains(res->GetResourceId()));
    n_assert(res->GetUseCount() > 0);
    res->DecrUseCount();
    if (res->GetUseCount() == 0)
    {
        res->Unload();
        res->SetLoader(0);
        res->SetSaver(0);
        this->unmanagedResources.Erase(res->GetResourceId());
    }
}

//------------------------------------------------------------------------------
/**
    Unregister an unmanaged resource by resource id.
*/
void
ResourceManager::UnregisterUnmanagedResource(const ResourceId& id)
{
    n_assert(this->HasResource(id));

    // IMPORTANT: need to make a copy of the smart pointer, since 
    //  array layout will change!
    Ptr<Resource> res = this->unmanagedResources[id];
    this->UnregisterUnmanagedResource(res);
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::HoldResources()
{
    n_assert(!this->resourcesHolded);
    IndexT i;
    for (i = 0; i < this->unmanagedResources.Size(); i++)
    {
        this->unmanagedResources.ValueAtIndex(i)->IncrUseCount();
        this->holdedResources.Append(this->unmanagedResources.ValueAtIndex(i));
    }

    this->resourcesHolded = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::ReleaseResources()
{
    n_assert(this->resourcesHolded);
    IndexT i;
    for (i = 0; i < this->holdedResources.Size(); i++)
    {
        this->UnregisterUnmanagedResource(this->holdedResources[i]);
    }
    this->resourcesHolded = false;
}

//------------------------------------------------------------------------------
/**
    Create a unmanaged resource object. If the resource already exists, its
    use count will be increased and the resource will be returned.
    If the resource doesn't exist yet, a new resource object will be created
    and registered as shared resource.
    Please note that you must call UnregisterUnmanagedResource() when the
    resource is no longer needed in order to manage the use count properly.
*/
Ptr<Resource>
ResourceManager::CreateUnmanagedResource(const ResourceId& resId, const Rtti& resClass, const Ptr<ResourceLoader>& loader, const Ptr<ResourceSaver>& saver)
{
    n_assert(resId.IsValid());
    if (this->unmanagedResources.Contains(resId))
    {
        // return existing resource
        const Ptr<Resource>& res = this->unmanagedResources[resId];
        n_assert(res->IsInstanceOf(resClass));
        n_assert(res->GetResourceId() == resId);
        if (loader.isvalid())
        {
            n_assert(res->GetLoader().isvalid() && res->GetLoader()->IsInstanceOf(*loader->GetRtti()));
        }
        if (saver.isvalid())
        {
            n_assert(res->GetSaver().isvalid() && res->GetSaver()->IsInstanceOf(*saver->GetRtti()));
        }
        res->IncrUseCount();
        return res;
    }
    else
    {
        // resource doesn't exist yet, create new one
        Ptr<Resource> res = (Resource*) resClass.Create();
        res->SetResourceId(resId);
        if (loader.isvalid())
        {
            res->SetLoader(loader);
        }
        if (saver.isvalid())
        {
            res->SetSaver(saver);
        }
        this->RegisterUnmanagedResource(res);
        return res;
    }
}

//------------------------------------------------------------------------------
/**
    Returns an array of unmanaged resources by type. This is a slow method.
*/
Array<Ptr<Resource> >
ResourceManager::GetResourcesByType(const Core::Rtti& type) const
{
    Array<Ptr<Resource> > result;
    IndexT i;
    for (i = 0; i < this->unmanagedResources.Size(); i++)
    {
        if (this->unmanagedResources.ValueAtIndex(i)->IsA(type))
        {
            result.Append(unmanagedResources.ValueAtIndex(i));
        }
    }
    for (i = 0; i < this->managedResources.Size(); i++)
    {
        const Ptr<Resource>& resource = this->managedResources.ValueAtIndex(i)->GetResource();
        if (!resource.isvalid())
        {
            continue;
        }
        if (resource->IsA(type))
        {
            result.Append(resource);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceManager::SetMappersAsync(bool b)
{
	IndexT i;
	for (i = 0; i < this->mappers.Size(); i++)
	{
		this->mappers.ValueAtIndex(i)->SetAsyncEnabled(b);
	}
}
} // namespace Resources
//------------------------------------------------------------------------------