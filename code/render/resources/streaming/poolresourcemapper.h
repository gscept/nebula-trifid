#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::PoolResourceMapper

    Base-interface for all ResourceCaches using a pool-construct.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "streamingresourcemapper.h"
#include "resourcecreator.h"
#include "poolscheduler.h"
#include "util/array.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourcePool;
class ResourceInfo;

/// helper class for pool generation
struct PoolSetupInfo 
{
    ResourceInfo* info;
    IndexT numSlots;
};

class PoolResourceMapper : public StreamingResourceMapper
{
    __DeclareClass(PoolResourceMapper);

public:
    friend class ResourceScheduler;
    friend class TexturePoolMapperScheduler;
    /// constructor
    PoolResourceMapper(void);
    /// destructor
    ~PoolResourceMapper(void);

    /// read given file name and fill given dictionary with data
    static void ReadTexturePoolFromXML(const IO::URI& fileName, Util::Dictionary<Util::StringAtom, PoolSetupInfo>& setupData);
    /// write given pool data to given file name
    static void WriteTexturePoolToXML(const IO::URI& fileName, const Util::Dictionary<Util::StringAtom, Resources::PoolSetupInfo>& setupData);

    /// sets the default loader class
    void SetResourceCreatorClass(const Core::Rtti& creatorType);
    /// set default scheduler for pools
    void SetDefaultPoolScheduler(const Ptr<PoolScheduler>& scheduler);

    /// creates and initializes pools using given information
    virtual void SetupPools(const Util::Dictionary<Util::StringAtom, PoolSetupInfo>& poolSetupData);
    /// called from resource manager when mapper is attached
    virtual void OnAttachToResourceManager(); 
    /// called from resource manager when mapper is removed
    virtual void OnRemoveFromResourceManager();

    /// called before gathering render stats - updates frameIndex of all pools
    virtual void OnPrepare(bool waiting);
    /// resets all resource-connections and restores initial-state (keeps pools initialized)
    virtual void Reset();

    /// returns allocated resource space of all pools in bytes
    virtual uint GetAllocatedMemory();
    /// returns used memory of all pools in bytes
    virtual uint GetUsedMemory();

    // --- Debug section ---
    /// writes info of all pools to given XML file
    virtual void WritePoolsToXML(const IO::URI& fileName);

    /// returns pool count
    SizeT GetNumPools() const;
    /// returns Ptr to pool for given index (may exclusively be called by debug pagehandler!)
    const Ptr<ResourcePool>& GetPoolForDebug(const Util::StringAtom& poolId) const;

protected:
    /// returns index of pool which is of given info-type or -1 if no pool found
    IndexT GetFittingPoolIndex(const ResourceInfo* resInfo) const;
    /// this is called if a resource is requested but no free slot is found for
    void NoSlotFound(const Ptr<ManagedResource>& resource, IndexT frameIdx);

    Util::Array<Ptr<ResourcePool>> pools;
    const Core::Rtti* creatorType;
    Ptr<PoolScheduler> defaultPoolScheduler;
};

//------------------------------------------------------------------------------
/**
*/
inline void
PoolResourceMapper::SetResourceCreatorClass(const Core::Rtti& creatorType)
{
    this->creatorType = &creatorType;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
PoolResourceMapper::GetNumPools() const
{
    return this->pools.Size();
}
} // namespace Resources
//------------------------------------------------------------------------------
