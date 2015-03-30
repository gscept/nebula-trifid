//------------------------------------------------------------------------------
//  resourcepool.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/managedresource.h"
#include "resourcepool.h"
#include "resourceslot.h"
#include "resources/resource.h"
#include "resourcecreator.h"
#include "poolscheduler.h"

namespace Resources {
__ImplementClass(Resources::ResourcePool, 'RESP', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourcePool::ResourcePool(void) :
    initialized(false),
    poolSize(0),
    minFrameAge(5),
    amountSlots(0),
    frameIdx(0)
    ,rejectedRequests(0),
    autoMipMapPoolsNotFound(0),
    autoMipMapSlotsNotFound(0),
    totalRejectedRequests(0),
    totalAutoMipMapPoolsNotFound(0),
    totalAutoMipMapSlotsNotFound(0)
{
}

//------------------------------------------------------------------------------
/**
*/
ResourcePool::~ResourcePool(void)
{
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ResourcePool::GetSlotsUsed() const
{
    uint result = 0;
    IndexT i;
    for (i = 0; i < this->slots.Size(); i++)
    {
        const Ptr<ManagedResource>& curRes = this->slots[i]->GetCurrentManagedResource();
        if (curRes.isvalid())
        {
            result ++;
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
uint
ResourcePool::GetUsedSize() const
{
    return this->GetSlotsUsed() * this->poolInfo->GetSize();
}

//------------------------------------------------------------------------------
/**
*/
void
ResourcePool::SetLoader(const Core::Rtti* loader)
{
    this->defaultLoaderClass = loader;
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourcePool::Initialize(const Ptr<ResourceCreator>& resourceCreator, const Ptr<PoolScheduler>& scheduler, SizeT amountSlots)
{
    n_assert(false == this->IsInitialized());
    n_assert(this->poolInfo != 0);
    n_assert(resourceCreator.isvalid());
    n_assert(scheduler.isvalid());

    this->scheduler = scheduler;

    if (InvalidIndex == amountSlots)
    {
        if(this->amountSlots <= 0)
        {
            n_printf("WARNING: pool '%s' has %i slots!\n", this->poolId.Value(), this->amountSlots);
        }
    }
    else
    {
        this->amountSlots = amountSlots;
    }
    this->poolSize = poolInfo->GetSize() * this->amountSlots;

    SizeT i;
    for (i = 0; i < this->amountSlots; i++)
    {
        Ptr<ResourceSlot> newSlot = ResourceSlot::Create();
        newSlot->SetResource(resourceCreator->CreateResource(poolInfo));
        newSlot->CreateLoader(*this->defaultLoaderClass);
        this->slots.Append(newSlot);
    }

    this->initialized = true;
    return this->initialized;
}

//------------------------------------------------------------------------------
/**
    Create as many slots as we can until given limit is reached. 
    Return estimated number of bytes we haven NOT used.
*/
uint
ResourcePool::InitializeMax(const Ptr<ResourceCreator>& resourceCreator, const Ptr<PoolScheduler>& scheduler, uint maxPoolSize)
{
    n_assert(false == this->initialized);
    n_assert(this->poolInfo != 0);
    n_assert(resourceCreator.isvalid());
    n_assert(scheduler.isvalid());

    this->scheduler = scheduler;

    // ensure we can create at least one slot
    n_assert(this->poolInfo->GetSize() <= maxPoolSize);
    while (this->poolSize <= maxPoolSize + this->poolInfo->GetSize())
    {
        Ptr<ResourceSlot> newSlot = ResourceSlot::Create();
        newSlot->SetResource(resourceCreator->CreateResource(poolInfo));
        newSlot->CreateLoader(*this->defaultLoaderClass);
        this->slots.Append(newSlot);
    }
    this->amountSlots = this->slots.Size();
    this->poolSize = this->poolInfo->GetSize() * this->amountSlots;
    this->initialized = true;
    return maxPoolSize - this->poolSize;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourcePool::Reset()
{
    n_assert(this->initialized);
    IndexT i;
    for(i = 0; i < this->slots.Size(); i++)
    {
        this->slots[i]->SetupFromManagedResource(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourcePool::Unload()
{
    n_assert(this->initialized);
    IndexT i;
    for (i = 0; i < this->slots.Size(); i++)
    {
        this->slots[i]->Unload();
    }
    this->slots.Clear();
    n_assert(this->poolInfo != 0);
    delete this->poolInfo;
    this->scheduler = 0;
    this->initialized = false;
}

//------------------------------------------------------------------------------
/**
    Leave all slot request strategy to the scheduler as this enables dynamic replacement
    at runtime to change the strategy.
*/
const Ptr<ResourceSlot>
ResourcePool::RequestSlot(const ResourceRequestInfo* requestInfo, bool startSearchFront)
{
    n_assert(this->scheduler.isvalid());
    return this->scheduler->RequestSlot(this, requestInfo, startSearchFront);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceSlot>
ResourcePool::GetSlot(const Ptr<ManagedResource>& managedResource) const
{
    IndexT i;
    for (i = 0; i < this->slots.Size(); i++)
    {
        if (this->slots[i]->GetCurrentManagedResource() == managedResource)
        {
            return this->slots[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceSlot>
ResourcePool::GetSlot(const ResourceId& resId) const
{
    IndexT slotIdx = InvalidIndex;
    IndexT i;
    for (i = 0; i < this->slots.Size(); i++)
    {
        if (this->slots[i]->GetResource()->GetResourceId() == resId)
        {
            slotIdx = i;
            break;
        }
    }
    if (InvalidIndex != slotIdx)
    {
        return this->slots[slotIdx];
    }
    else
    {
        return 0;
    }    
}

//------------------------------------------------------------------------------
/**
*/
const ResourceId&
ResourcePool::GetIdForSlot(IndexT slotIdx) const
{
    n_assert(slotIdx >= 0 && slotIdx < this->slots.Size());
    return this->slots[slotIdx]->GetResource()->GetResourceId();
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ResourcePool::GetSlotStatistics(PoolStatisticsRequest conditions) const
{
    if (conditions.none == SET)
    {
        return this->slots.Size();
    }

    IndexT i;
    SizeT counter = 0;

    for (i = 0; i < this->slots.Size(); i++)
    {
        Ptr<ResourceSlot>& slot = this->slots[i];
        if (!slot->GetCurrentManagedResource().isvalid())
        {
            continue;
        }
        Ptr<ManagedResource> resource = slot->GetCurrentManagedResource();
        n_assert(resource->GetResource().isvalid());
        if (conditions.active == SET && resource->GetLastFrameId() > this->frameIdx + this->minFrameAge)
        {
            continue;
        }
        counter++;
        /*
        if (conditions.active == true && !slot->GetCurrentManagedResource()->IsAutoManaged())
        {
            continue;
        }
        if (discarded == true && !slot->GetCurrentManagedResource()->IsDiscarded())
        {
            continue;
        }
            slot->GetCurrentManagedResource()->GetLastFrameId() < this->minFrameAge + this->frameIdx &&
            !slot->GetCurrentManagedResource()->IsDiscarded())
        {
            counter++;
        }*/
    }
    return counter;
}

//------------------------------------------------------------------------------
/**
*/
const Util::StringAtom
ResourcePool::GetStateStringForSlot(const ResourceId& resId) const
{
    IndexT i;
    Util::StringAtom result = "State unavailable";
    for (i = 0; i < this->slots.Size(); i++)
    {
        if (resId == this->slots[i]->GetResource()->GetResourceId())
        {
            switch (this->slots[i]->GetResource()->GetState())
            {
            case Resource::Initial:
                result = "Initial";
                break;
            case Resource::Loaded:
                result = "Loaded";
                break;
            case Resource::Pending:
                result = "Pending";
                break;
            case Resource::Cancelled:
                result = "Cancelled";
                break;
            case Resource::Failed:
                result = "Failed";
                break;
            default:
                break;
            }
            break;
        }
    }
    return result;
}
} // namespace Resources
//------------------------------------------------------------------------------
