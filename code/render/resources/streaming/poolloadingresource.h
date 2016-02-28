#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::PoolLoadingResource

    A specialized LoadingResource for pool using strategies.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/streaming/loadingresource.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceSlot;

class PoolLoadingResource : public Resources::LoadingResource
{
    __DeclareClass(PoolLoadingResource);
public:
    /// constructor
    PoolLoadingResource();
    /// destructor
    ~PoolLoadingResource();

    /// get target slot (slot we want to load)
    const Ptr<ResourceSlot>& GetNewSlot() const;
    /// get the source slot (slot we want to reuse)
    const Ptr<ResourceSlot>& GetOldSlot() const;

    /// set target slot (slot we want to load)
    void SetTargetSlot(const Ptr<ResourceSlot>& targetSlot);
    /// set the source slot (slot we want to reuse)
    void SetSourceSlot(const Ptr<ResourceSlot>& srcSlot);

    /// called as the request was performed successfully
    virtual void OnSuccessRequest();

protected:
    /// the slot containing the resource to load
    Ptr<ResourceSlot> newSlot;
    /// the slot our ManagedResource' Resource is currently contained at (may be 0!)
    Ptr<ResourceSlot> oldSlot;
};

//------------------------------------------------------------------------------
/**
*/
inline
PoolLoadingResource::PoolLoadingResource()
{

}

//------------------------------------------------------------------------------
/**
*/
inline
PoolLoadingResource::~PoolLoadingResource()
{

}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ResourceSlot>&
PoolLoadingResource::GetNewSlot() const
{
    return this->newSlot;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ResourceSlot>&
PoolLoadingResource::GetOldSlot() const
{
    return this->oldSlot;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PoolLoadingResource::SetTargetSlot(const Ptr<ResourceSlot>& targetSlot)
{
    this->newSlot = targetSlot;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PoolLoadingResource::SetSourceSlot(const Ptr<ResourceSlot>& srcSlot)
{
    this->oldSlot = srcSlot;
}
} // namespace Resource
//------------------------------------------------------------------------------