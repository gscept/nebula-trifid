//------------------------------------------------------------------------------
//  resourceslot.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourceslot.h"
#include "resources/managedresource.h"
#include "resources/resourceloader.h"

namespace Resources
{
__ImplementClass(Resources::ResourceSlot, 'RSSL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceSlot::ResourceSlot()
{}

//------------------------------------------------------------------------------
/**
*/
ResourceSlot::~ResourceSlot()
{
    this->Unload();
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSlot::CreateLoader(const Core::Rtti& loaderClass)
{
    Ptr<ResourceLoader> loader = (ResourceLoader*)loaderClass.Create();
    this->resource->SetLoader(loader);
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSlot::Reset()
{
    // take care ManagedResource doesn't use this resource any longer
    if (this->currentManagedResource.isvalid())
    {
        this->currentManagedResource->SetResource(0);
    }
    this->currentManagedResource = 0;

    // reset Resource::State and ResourceLoader
    this->resource->SetState(Resource::Initial);
    if (this->resource->GetLoader().isvalid())
    {
        this->resource->GetLoader()->Reset();
    }
    else
    {
        // this is usually performed by the ResourceLoader
        this->resource->Unlock();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSlot::SetupFromManagedResource(Ptr<ManagedResource> managedResource)
{
    this->Reset();
    this->currentManagedResource = managedResource;
    if (this->currentManagedResource.isvalid())
    {
        this->resource->SetResourceId(this->currentManagedResource->GetResourceId());
        this->currentManagedResource->SetResourceType(this->resource->GetRtti());
        this->currentManagedResource->SetResource(this->resource);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSlot::Unload()
{
    // tell managedResource it shall use placeholder
    this->Reset();;
    if (this->resource.isvalid())
    {
        this->resource->Unload();
        this->resource->SetLoader(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSlot::PrintDebugInfo() const
{
    n_printf("todo: print slot-info\n");
}
} // namespace Resources
//------------------------------------------------------------------------------