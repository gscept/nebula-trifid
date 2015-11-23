#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ResourceBase
    
    Base class for all CoreGraphics resource classes.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace Base
{
class ResourceBase : public Resources::Resource
{
    __DeclareClass(ResourceBase);
public:
    /// resource usage flags
    enum Usage
    {
        UsageImmutable,      //> can only be read by GPU, not written, cannot be accessed by CPU
        UsageDynamic,        //> can only be read by GPU, can only be written by CPU
        UsageCpu,            //> a resource which is only accessible by the CPU and can't be used for rendering
    };

    // cpu access flags
    enum Access
    {
        AccessNone,         // CPU does not require access to the resource (best)
        AccessWrite,        // CPU has write access
        AccessRead,         // CPU has read access
        AccessReadWrite,    // CPU has read/write access
    };

    // mapping types
    enum MapType
    {
        MapRead,                // gain read access, must be UsageDynamic and AccessRead
        MapWrite,               // gain write access, must be UsageDynamic and AccessWrite
        MapReadWrite,           // gain read/write access, must be UsageDynamic and AccessReadWrite
        MapWriteDiscard,        // gain write access, discard previous content, must be UsageDynamic and AccessWrite
        MapWriteNoOverwrite,    // gain write access, must be UsageDynamic and AccessWrite, see D3D10 docs for details
    };

	// streaming methods
	enum Syncing
	{
		SyncingSimple,					// CPU and GPU buffers are detached, requires Map and Unmap to transport data
		SyncingPersistent,				// buffer is persistently mapped, no need to unmap
		SyncingCoherentPersistent		// buffer is persistently mapped and also coherent on GPU
	};

    /// constructor
    ResourceBase();
    /// destructor
    virtual ~ResourceBase();

    /// set resource usage type
    void SetUsage(Usage usage);
    /// get resource usage type
    Usage GetUsage() const;
    /// set resource cpu access type
    void SetAccess(Access access);
    /// get cpu access type
    Access GetAccess() const;

	/// set streaming method
	void SetSyncing(Syncing method);
	/// get streaming method
	Syncing GetSyncing() const;

protected:
    Usage usage;
    Access access;
	Syncing syncing;

};

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceBase::SetUsage(Usage u)
{
    this->usage = u;
}

//------------------------------------------------------------------------------
/**
*/
inline ResourceBase::Usage
ResourceBase::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceBase::SetAccess(Access a)
{
    this->access = a;
}

//------------------------------------------------------------------------------
/**
*/
inline ResourceBase::Access
ResourceBase::GetAccess() const
{
    return this->access;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceBase::SetSyncing(Syncing s)
{
	this->syncing = s;
}

//------------------------------------------------------------------------------
/**
*/
inline ResourceBase::Syncing
ResourceBase::GetSyncing() const
{
	return this->syncing;
}

} // namespace Base
//------------------------------------------------------------------------------
