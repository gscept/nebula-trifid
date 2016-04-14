#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::ResourceSlot

    One or more ResourceSlots are hold in an Array of a ResourcePool.
    Each slot contains a Resource, i.e. a Texture or a Mesh, and a Ptr
    to the ManagedResource, currently using this Resource

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ManagedResource;
class ResourceLoader;

class ResourceSlot : public Core::RefCounted
{
    __DeclareClass(ResourceSlot);
public:
    /// constructor
    ResourceSlot();
    /// destructor
    ~ResourceSlot();

    /// returns current ManagedResource
    Ptr<ManagedResource> GetCurrentManagedResource() const;
    /// sets current ManagedResource
    void SetCurrentManagedResource(const Ptr<ManagedResource>& managedResource);
    /// returns a Ptr to the Resource, hold by this Slot
    const Ptr<Resource>& GetResource() const;

    /// sets the resource
    void SetResource(const Ptr<Resource>& resource);

    /// sets the resourceLoader and creates a new one for this slot/resource
    virtual void CreateLoader(const Core::Rtti& loaderClass);
    /// tells ManagedResource not to use this Resource any longer, resets Resource and its ResourceLoader
    virtual void Reset();
    /// sets the current ManagedResource and does some Initialization 
    virtual void SetupFromManagedResource(Ptr<ManagedResource> managedResource);
    /// calls Reset plus unloading the resource
    virtual void Unload();

    /// prints out some debug-informations. extend this for addition debug-infos
    virtual void PrintDebugInfo() const;


protected:
    /// the resource itself
    Ptr<Resource> resource;
    /// the managedResource currently using the resource
    Ptr<ManagedResource> currentManagedResource;
};

//------------------------------------------------------------------------------
/**
*/
inline Ptr<ManagedResource>
ResourceSlot::GetCurrentManagedResource() const
{
    return this->currentManagedResource;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceSlot::SetCurrentManagedResource(const Ptr<ManagedResource>& managedResource)
{
    this->currentManagedResource = managedResource;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resource>&
ResourceSlot::GetResource() const
{
    return this->resource;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceSlot::SetResource(const Ptr<Resource>& resource)
{
    this->resource = resource;
}
} // namespace Resource
//------------------------------------------------------------------------------