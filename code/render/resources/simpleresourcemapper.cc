//------------------------------------------------------------------------------
//  simpleresourcemapper.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/simpleresourcemapper.h"
#include "resources/resourcemanager.h"

namespace Resources
{
__ImplementClass(Resources::SimpleResourceMapper, 'SRSM', Resources::ResourceMapper);

using namespace Core;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
SimpleResourceMapper::SimpleResourceMapper() :
    resourceClass(0),
    resLoaderClass(0),
    managedResourceClass(0),
    placeholderResource(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SimpleResourceMapper::~SimpleResourceMapper()
{
    n_assert(!this->IsAttachedToResourceManager());
}

//------------------------------------------------------------------------------
/**
*/
const Rtti&
SimpleResourceMapper::GetResourceType() const
{
    n_assert2(0 != this->resourceClass, "SimpleResourceMapper::SetResourceType() method not called!");
    return *(this->resourceClass);
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnAttachToResourceManager()
{
    n_assert2(0 != this->resourceClass, "SimpleResourceMapper::SetResourceClass() not called!");
    n_assert2(0 != this->managedResourceClass, "SimpleResourceMapper::SetManagedResourceClass() not called!");
    n_assert2(0 != this->resLoaderClass, "SimpleResourceMapper::SetResourceLoaderClass() not called!");

    // call parent class
    ResourceMapper::OnAttachToResourceManager();
    n_assert(this->managedResources.IsEmpty());
    n_assert(this->pendingResources.IsEmpty());

    // load placeholder resource
    if (this->placeholderResourceId.IsValid())
    {
        // @todo: set autoManaged to false before requesting so it will be
        // appended at the end of the resource-queue and not constantly checked for removal
        this->placeholderResource = (Resource*) this->resourceClass->Create();
        this->placeholderResource->SetResourceId(this->placeholderResourceId);
        this->placeholderResource->SetLoader((ResourceLoader*)this->resLoaderClass->Create());
        this->placeholderResource->SetAsyncEnabled(false);
        this->placeholderResource->Load();
        if (!this->placeholderResource->IsLoaded())
        {
            n_error("SimpleResourceMapper::OnAttachToServer(): could not not load placeholder resource '%s' of class '%s'!\n",
                this->placeholderResourceId.Value(), this->resourceClass->GetName().AsCharPtr());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnRemoveFromResourceManager()
{
    n_assert(this->IsAttachedToResourceManager());

    // discard all remaining resources
    while (this->managedResources.Size() > 0)
    {
        this->OnDiscardManagedResource(this->managedResources.ValueAtIndex(this->managedResources.Size() - 1));
    }
    n_assert(this->pendingResources.IsEmpty());

    // unload the placeholder resource
    if (this->placeholderResource.isvalid())
    {
        this->placeholderResource->Unload();
        this->placeholderResource->SetLoader(0);
        this->placeholderResource = 0;
    }

    // finally call parent class
    ResourceMapper::OnRemoveFromResourceManager();
}

//------------------------------------------------------------------------------
/**
    This method is called by the ResourceManager when a new
    ManagedResource must be created. The resource manager will only call this
    method if this is the first request for the resource name.
*/
Ptr<ManagedResource>
SimpleResourceMapper::OnCreateManagedResource(const Rtti& resType, const ResourceId& resId, const Ptr<ResourceLoader>& optResourceLoader, bool forceSync)
{
    n_assert(0 != this->managedResourceClass);

    // ResourceManager should call this method only when 
    // a managed resource with the same resource id doesn't exist yet:
    n_assert(!this->managedResources.Contains(resId));

    // for the same reason the resource shouldn't be pending already:
    n_assert(!this->pendingResources.Contains(resId));

    n_assert(!ResourceManager::Instance()->HasResource(resId));
    // create a new managed resource, and set the placeholder resource on it
    Ptr<ManagedResource> managedResource = (ManagedResource*) this->managedResourceClass->Create();
    managedResource->IncrClientCount();
    managedResource->SetResourceId(resId);
    managedResource->SetResourceType(&this->GetResourceType());
    if (this->placeholderResource.isvalid())
    {
        managedResource->SetPlaceholder(this->placeholderResource);
    }
    this->managedResources.Add(resId, managedResource);

    // check if the resource already exists as shared resource
    const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
    Ptr<Resource> resource;

    // previously the SharedResourceServer was requested for resource creation
    // but now we'll create our own ones as this point should only be entered as the resource doesn't exist yet
    resource = (Resource*)this->resourceClass->Create();
    resource->SetResourceId(resId);
    if (optResourceLoader.isvalid())
    {
        resource->SetLoader(optResourceLoader);
    }
    else
    {
        resource->SetLoader((ResourceLoader*)this->resLoaderClass->Create());
    }
    // initiate an synchronous or asynchronous load (depending on setting)
    resource->SetAsyncEnabled(this->asyncEnabled && !forceSync);
    resource->Load();

    // if loading failed, keep the resource pointer as 0
    if (resource->IsPending())
    {
        // asynchronous load initiayed and pending
        this->pendingResources.Add(resId, resource);
    }   
	else if (resource->IsLoaded())
	{
		// synchronous loading succeeded (or failed)
		managedResource->SetResource(resource);
	}
    return managedResource;
}

//------------------------------------------------------------------------------
/**
    This method will be called by the ResourceManager whenever a ManagedResource
    should be discarded. 
*/
void
SimpleResourceMapper::OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource)
{
    n_assert(managedResource->GetClientCount() > 0);
    n_assert(this->managedResources.Contains(managedResource->GetResourceId()));
    n_assert(managedResource->GetResourceType() == &this->GetResourceType());
    n_assert(managedResource->IsInstanceOf(*(this->managedResourceClass)));

    managedResource->DecrClientCount();
    if (managedResource->GetClientCount() == 0)
    {        
        // drop pending resource
        if (this->pendingResources.Contains(managedResource->GetResourceId()))
        {
            this->pendingResources.Erase(managedResource->GetResourceId());
        }
		else
		{
			if(!managedResource->IsPlaceholder())
			{
				n_assert(managedResource->GetResource()->GetUseCount() == 0);
				managedResource->GetResource()->Unload();
				managedResource->GetResource()->SetLoader(0);
				managedResource->GetResource()->SetSaver(0);
			}			
		} 

        // clear the contained resource
        managedResource->Clear();

        // finally drop the managed resource itself, this must be the last
        // call in this method, since the managedResource pointer
        // may become invalid at this point
        this->managedResources.Erase(managedResource->GetResourceId());
    }
}

//------------------------------------------------------------------------------
/**
    This method will go through all ManagedResources and reset their
    render statistics. It will also check whether pending resources
    have finished loading, and update the associated managed resources 
    accordingly.
*/
void
SimpleResourceMapper::OnPrepare(bool waiting)
{
    // first reset render statistics
    IndexT resIndex;
    for (resIndex = 0; resIndex < this->managedResources.Size(); resIndex++)
    {
        this->managedResources.ValueAtIndex(resIndex)->ClearRenderStats();
    }

    // now check pending resources...
    IndexT pendingIndex;
    for (pendingIndex = 0; pendingIndex < this->pendingResources.Size();)
    {
        Ptr<Resource>& resource = this->pendingResources.ValueAtIndex(pendingIndex);
        n_assert(resource->IsPending());

        // try load...
        resource->Load();
        if (resource->IsLoaded() || resource->LoadFailed())
        {
            // resource has finished loading, or failed to load
            if (resource->LoadFailed())
            {
                // resolve cyclic dependency since the resource object
                // will be dropped
                resource->SetLoader(0);
                resource->SetSaver(0);

                // loading the resource has failed, this is a hard error
                /*
                n_error("SimpleResourceMapper: failed to load resource '%s'!\n", resource->GetResourceId().Value());
                */

                // FIXME: this is the "soft case"
                if (this->placeholderResource.isvalid())
                {
					// FIXME: why would you do that? setting it to the placeholder defeats the whole purpose and messes up refcounting of placeholders
                    // load has failed, set the place holder resource as the actual resource
                    // this->managedResources[resource->GetResourceId()]->SetResource(this->placeholderResource);

					// mark the managed resource as failed so that we know to use placeholder
					// setting the resource suggests the managed resource is successful
					this->managedResources[resource->GetResourceId()]->SetFailed(true);
                    n_printf("SimpleResourceMapper: failed to load resource '%s'!\n", resource->GetResourceId().Value());
                }
                else
                {
                    n_error("SimpleResourceMapper: no placeholder resource set for resource type '%s'!\n",
                        this->managedResourceClass->GetName().AsCharPtr());
                }
            }
            else
            {
                // load succeeded, set the actual resource
                this->managedResources[resource->GetResourceId()]->SetResource(resource);
            }
            this->pendingResources.EraseAtIndex(pendingIndex);
        }
        else
        {
            // still pending...
            n_assert(resource->IsPending());
            pendingIndex++;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SimpleResourceMapper::OnUpdate(IndexT frameIndex)
{
    // @todo: do we need to do anything here?
}

//------------------------------------------------------------------------------
/**
*/
SizeT
SimpleResourceMapper::GetNumPendingResources() const
{
    return this->pendingResources.Size();
}

} // namespace Resources

