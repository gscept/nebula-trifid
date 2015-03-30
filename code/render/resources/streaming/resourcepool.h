#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::ResourcePool

    A ResourcePool stores a fixed size of ResourceSlots sharing identical
    Resource-Formats (e.g. resourceType, format, size, ...)

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resourcerequestinfo.h"
#include "resourceinfo.h"
#include "resourceslot.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;
class ResourceCreator;
class PoolScheduler;

enum PoolStatisticsCondition
{
    NOT_SET,
    SET,
    REGARDLESS
};

struct PoolStatisticsRequest
{
    PoolStatisticsCondition none;
    PoolStatisticsCondition active;

    PoolStatisticsRequest()
    {      
        this->Reset();
    }

    void Reset()
    {
        none = NOT_SET;
        active = NOT_SET;
    }
};

class ResourcePool : public Core::RefCounted
{
    friend class PoolScheduler;

    __DeclareClass(ResourcePool);
public:
    /// constructor
    ResourcePool(void);
    /// destructor
    virtual ~ResourcePool(void);

    /// returns true if pool is initialized
    bool IsInitialized() const;
    /// returns total slot count
    SizeT GetNumSlots() const;
    /// return pool id
    const Util::StringAtom& GetId() const;
    /// returns pool-info
    const ResourceInfo* GetInfo() const;
    /// returns current frame Index
    IndexT GetFrameIndex() const;
    /// returns pool size
    uint GetPoolSize() const;
    /// returns slot holding given managedResource or 0 if nothing found
    const Ptr<ResourceSlot> GetSlot(const Ptr<ManagedResource>& managedResource) const;
    /// returns slot holding given managedResource or 0 if nothing found
    const Ptr<ResourceSlot> GetSlot(const ResourceId& resId) const;
    /// set the scheduler for this pool
    void SetScheduler(const Ptr<PoolScheduler>& scheduler);
    /// set pool id
    void SetId(const Util::StringAtom& id);

    /// sets total slot count
    void SetNumSlots(SizeT amountSlots);
    /// sets the pool info
    void SetInfo(ResourceInfo* info);
    /// sets the current frame index
    void SetFrameIndex(IndexT frameIdx);

    /// returns number of currently used slots (Managed and not discarded)
    virtual SizeT GetSlotsUsed() const;
    /// returns size of all slots currently having a valid and not discarded ManagedResource
    uint GetUsedSize() const;

    /// sets the default resource-loader
    virtual void SetLoader(const Core::Rtti* loader);

    /// initializes slots - returns true if everything went fine
    virtual bool Initialize(const Ptr<ResourceCreator>& resourceCreator, const Ptr<PoolScheduler>& scheduler, SizeT slotAmount = -1);
    /// initializes max amount of slot using given maxSize in bytes
    virtual uint InitializeMax(const Ptr<ResourceCreator>& resourceCreator, const Ptr<PoolScheduler>& scheduler, uint maxPoolSize);

    /// calls reset of all slots so connections to their appropriate ManagedResources are cut
    /// but they keep their allocated space
    virtual void Reset();
    /// unloads all slots (shut down)
    virtual void Unload();

    /// requests a resource for usage and tells slot's ManagedResource it's unloaded
    const Ptr<ResourceSlot> RequestSlot(const ResourceRequestInfo* requestInfo, bool startSearchFront = true);

    // --- Debug related methods ---
    /// returns count of currently active slots (frameId < minFrameAge)
    SizeT GetSlotStatistics(PoolStatisticsRequest conditions) const;
    /// returns resource id of given slot index
    const ResourceId& GetIdForSlot(IndexT slotIdx) const;
    /// return human readable string of slot' resource state
    const Util::StringAtom GetStateStringForSlot(const ResourceId& resId) const;


    /// reset all debug stats on frame or totally if totalReset == true
    void ResetDebugStats(bool total = false);
    /// return rejectedRequests or totalRejectedRequests if total == true
    IndexT GetRejectedRequests(bool total = false) const;
    /// return autoMipMapPoolsNotFound or totalAutoMipMapPoolsNotFound if total == true
    IndexT GetAutoMipPoolsNotFound(bool total = false) const;
    /// return autoMipMapSlotsNotFound or totalAutoMipMapSlotsNotFound if total == true
    IndexT GetAutoMipSlotsNotFound(bool total = false) const;

    /// increase counter of rejectedRequests
    void IncreaseRejectedRequests(const ResourceId& resId);
    /// increase counter of autoMipMapPoolsNotFound
    void IncreaseAutoMipPoolsNotFound();
    /// increase counter of autoMipMapSlotsNotFound
    void IncreaseAutoMipSlotsNotFound();

protected:
    SizeT amountSlots;
    /// this is the Info of the RESOURCES held by this pool
    /// (poolInfo->estimatedSize is the size of a single Resource!)
    ResourceInfo* poolInfo;
    /// the total size of the pool in bytes
    uint poolSize;
    const Core::Rtti* defaultLoaderClass;
    Util::Array<Ptr<ResourceSlot>> slots;
    Ptr<PoolScheduler> scheduler;

    /// determines the minimum "age" of a resource before it can be overwritten on autoManage
    /// (so recently loaded resources are not thrown away too quickly)
    /// @todo: check if it's useful as global, per cache, per pool, per slot or per resource
    IndexT minFrameAge;
    bool initialized;
    IndexT frameIdx;

    /// counter for requests no free slot was found for
    IndexT rejectedRequests;
    /// counter for pools which were unable to find during auto mip mapping
    IndexT autoMipMapPoolsNotFound;
    /// counter for free slots which were unable to find during auto mip mapping
    IndexT autoMipMapSlotsNotFound;

    IndexT totalRejectedRequests;
    IndexT totalAutoMipMapPoolsNotFound;
    IndexT totalAutoMipMapSlotsNotFound;

    Util::StringAtom poolId;
    Util::Array<ResourceId> rejectedRequestResources;

};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourcePool::IsInitialized() const
{
    return this->initialized;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourcePool::GetNumSlots() const
{
    return this->slots.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::SetId(const Util::StringAtom& id)
{
    this->poolId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const ResourceInfo*
ResourcePool::GetInfo() const
{
    return this->poolInfo;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
ResourcePool::GetFrameIndex() const
{
    return this->frameIdx;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
ResourcePool::GetPoolSize() const
{
    return this->poolSize;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::SetScheduler(const Ptr<PoolScheduler>& scheduler)
{
    this->scheduler = scheduler;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::SetNumSlots(SizeT amountSlots)
{
    this->amountSlots = amountSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::SetInfo(ResourceInfo* info)
{
    this->poolInfo = info;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::SetFrameIndex(IndexT frameIndex)
{
    this->frameIdx = frameIndex;
}

#if !PUBLIC_BUILD
//------------------------------------------------------------------------------
/**
*/
inline IndexT
ResourcePool::GetRejectedRequests(bool total) const
{
    if (total)
    {
        return this->totalRejectedRequests;
    }
    else
    {
        return this->rejectedRequests;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
ResourcePool::GetAutoMipPoolsNotFound(bool total) const
{
    if (total)
    {
        return this->totalAutoMipMapPoolsNotFound;
    }
    else
    {
        return this->autoMipMapPoolsNotFound;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
ResourcePool::GetAutoMipSlotsNotFound(bool total) const
{
    if (total)
    {
        return this->totalAutoMipMapSlotsNotFound;
    }
    else
    {
        return this->autoMipMapSlotsNotFound;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::IncreaseRejectedRequests(const ResourceId& resId)
{
    if (InvalidIndex == this->rejectedRequestResources.FindIndex(resId))
    {
        this->rejectedRequests++;
        this->totalRejectedRequests++;
        this->rejectedRequestResources.Append(resId);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::IncreaseAutoMipPoolsNotFound()
{
    this->autoMipMapPoolsNotFound++;
    this->totalAutoMipMapPoolsNotFound++;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::IncreaseAutoMipSlotsNotFound()
{
    this->autoMipMapSlotsNotFound++;
    this->totalAutoMipMapSlotsNotFound++;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourcePool::ResetDebugStats(bool total)
{
    this->rejectedRequests = 0;
    this->autoMipMapPoolsNotFound = 0;
    this->autoMipMapSlotsNotFound = 0;
    this->rejectedRequestResources.Clear();

    if (total)
    {
        this->totalRejectedRequests = 0;
        this->totalAutoMipMapPoolsNotFound = 0;
        this->totalAutoMipMapSlotsNotFound = 0;
    }
}
#endif
//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
ResourcePool::GetId() const
{
    return this->poolId;
}

} // namespace Resources
//------------------------------------------------------------------------------
