#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::PoolScheduler

    A PoolScheduler manages a pool with a certain strategy.
    Currently it consits of a single method and does not even has any members
    as parsing the ResourcePool as a parameter eases handling.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourcePool;
class ResourceSlot;
class ResourceRequestInfo;

class PoolScheduler : public Core::RefCounted
{
    __DeclareClass(PoolScheduler);
public:
    /// constructor
    PoolScheduler(void);
    /// destructor
    ~PoolScheduler(void);

    /// requests a resource for usage and tells slot's ManagedResource it's unloaded
    const Ptr<ResourceSlot> RequestSlot(const Ptr<ResourcePool>& pool, const ResourceRequestInfo* requestInfo, bool startSearchFront = true);
};
} // namespace Resources
//------------------------------------------------------------------------------
