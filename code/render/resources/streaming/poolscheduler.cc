//------------------------------------------------------------------------------
//  poolscheduler.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "poolscheduler.h"
#include "resources/resource.h"
#include "resourcepool.h"
#include "resourceslot.h"
#include "resourcerequestinfo.h"
#include "resources/managedresource.h"

namespace Resources {
__ImplementClass(Resources::PoolScheduler, 'POSC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PoolScheduler::PoolScheduler()
{
}

//------------------------------------------------------------------------------
/**
*/
PoolScheduler::~PoolScheduler(void)
{
}

//------------------------------------------------------------------------------
/**
    If startSearchFront == false free slot will be searched starting from the END of the list.
    Use this for long-time non-autoManaged resources so they don't "pollute" the front of the slot-array.
*/
const Ptr<ResourceSlot>
PoolScheduler::RequestSlot(const Ptr<ResourcePool>& pool, const ResourceRequestInfo* requestInfo, bool startSeachFront)
{
    // the oldest frameId ensures no recently loaded resource is thrown out too early
    IndexT oldestFrameId = pool->frameIdx - pool->minFrameAge;
    IndexT i, slotIdx, freeSlot = -1;
    IndexT iterStart = 0;
    if (false == startSeachFront)
    {
        iterStart = pool->slots.Size() - 1;
    }

    // look up if resource was loaded before and slot' resource is still valid
    Ptr<Resource> res = 0;
    for (i = 0; i < pool->slots.Size(); i++)
    {
        if (startSeachFront)
        {
            slotIdx = i;
        }
        else
        {
            slotIdx = iterStart - i;
        }
        res = pool->slots[slotIdx]->GetResource();
        if (res->GetResourceId() == requestInfo->GetResourceId())
        {
            if (res->IsLoaded() && !res->IsLocked())
            {
                // found it! reuse
                return pool->slots[slotIdx];
            }
            else
            {
                // found it! ... but it's invalid...
                break;
            }
        }
    }

    for (i = 0; i < pool->slots.Size(); i++)
    {
        if (startSeachFront)
        {
            slotIdx = i;
        }
        else
        {
            slotIdx = iterStart - i;
        }
        if (pool->slots[slotIdx]->GetResource()->IsLocked())
        {
            continue;
        }
        Ptr<ManagedResource> slotResource = pool->slots[slotIdx]->GetCurrentManagedResource();
        if (false == slotResource.isvalid())
        {
            // found one that is free to overwrite - use it
            return pool->slots[slotIdx];
        }
        if (true == slotResource->IsAutoManaged() && slotResource->GetLastFrameId() < oldestFrameId
            && false == slotResource->GetResource()->IsLocked())
        {
            oldestFrameId = slotResource->GetLastFrameId();
            freeSlot = slotIdx;
        }
    }

    if (InvalidIndex != freeSlot)
    {
        return pool->slots[freeSlot];
    }
    else
    {
        return 0;
    }
}
} // namespace Resources
//------------------------------------------------------------------------------
