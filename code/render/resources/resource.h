#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::Resource

    Base class for shareable resources. Resources contain some sort
    of data which can be loaded and saved by specialized ResourceLoaders and 
    ResourceSavers.

    @FIXME: before destroying a Resource object, the Loader and Saver objects
    must be manually set to null to resolve a cyclic pointer dependency. The
    ResourceManager will take care of this automatically, but when
    creating resources directly, this must be taken care of!

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/resourceid.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceLoader;
class ResourceSaver;
    
class Resource : public Core::RefCounted
{
    __DeclareClass(Resource);
public:
    /// resource states (DO NOT CHANGE ORDER!)
    enum State
    {
        Initial,        // initial state
        Loaded,         // resource is loaded and valid
        Pending,        // asynchronous load pending, call Load() frequently
        Failed,         // resource loading has failed
        Cancelled       // asynchronous resource loading has been canceled
    };

    /// constructor
    Resource();
    /// destructor
    virtual ~Resource();

    /// request synchronous/asynchronous resource loading
    void SetAsyncEnabled(bool b);
    /// return true if asynchronous resource loading is enabled
    bool IsAsyncEnabled() const;
    /// set locked to true
    void Lock();
    /// set locked to false
    void Unlock();
    /// returns true if resource will be used as source for copy process soon
    bool IsLocked() const;
    /// set the resource identifier
    void SetResourceId(const ResourceId& id);
    /// get the resource identifier
    const ResourceId& GetResourceId() const;
    /// set optional resource loader
    void SetLoader(const Ptr<ResourceLoader>& loader);
    /// get optional resource loader
    const Ptr<ResourceLoader>& GetLoader() const;
    /// set optional resource saver
    void SetSaver(const Ptr<ResourceSaver>& saver);
    /// get optional resource saver
    const Ptr<ResourceSaver>& GetSaver() const;
    /// get current use count
    SizeT GetUseCount() const;

    /// load the resource
    virtual State Load();
    /// unload the resource, or cancel the pending load
    virtual void Unload();
	/// reload the resource, basically unload and load
	virtual void Reload();
    /// set current state (usually only called during Load()!)
    void SetState(State s);
    /// get current state
    State GetState() const;
    /// return true if current state is Loaded
    bool IsLoaded() const;
    /// return true if current state is Pending
    bool IsPending() const;
    /// return true if current state is Failed
    bool LoadFailed() const;
    /// save the resource
    virtual bool Save();

protected:
    friend class ResourceManager;

    /// increment use count
    void IncrUseCount();
    /// decrement use count
    void DecrUseCount();

	ResourceId resourceId;
    Ptr<ResourceLoader> loader;
    Ptr<ResourceSaver> saver;
    State state;
    SizeT useCount;
    bool asyncEnabled;
    bool locked;
	bool reload;
};

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::SetAsyncEnabled(bool b)
{
    this->asyncEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Resource::IsAsyncEnabled() const
{
    return this->asyncEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::Lock()
{
    this->locked = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::Unlock()
{
    this->locked = false;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Resource::IsLocked() const
{
    return this->locked;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::IncrUseCount()
{
    this->useCount++;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::DecrUseCount()
{
    n_assert(this->useCount > 0);
    this->useCount--;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
Resource::GetUseCount() const
{
    return this->useCount;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::SetResourceId(const ResourceId& id)
{
    this->resourceId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const ResourceId&
Resource::GetResourceId() const
{
    return this->resourceId;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Resource::SetState(State s)
{
    this->state = s;
}

//------------------------------------------------------------------------------
/**
*/
inline Resource::State
Resource::GetState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Resource::IsLoaded() const
{
    return (Loaded == this->state);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Resource::IsPending() const
{
    return (Pending == this->state);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Resource::LoadFailed() const
{
    return (Failed == this->state);
}

} // namespace Resources
//------------------------------------------------------------------------------
