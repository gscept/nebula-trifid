#pragma once

/*
    @class Resources::ResourceRequestInfo

    A ResourceRequestInfo contains several informations for requesting
    a resource from the ResourceCache. Basically containing a ResourceId, it it's possible
    to hold any further informations which can be useful for loading, i.e. a loading-priority.
    Inherit from this class to make up your own RequestInfo and loading-tactics.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file   
*/
#include "resources/resourceid.h"
#include "core/rtti.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceRequestInfo
{
public:
    /// constructor
    ResourceRequestInfo(void);
    /// constructor with resourceId
    ResourceRequestInfo(const ResourceId& resourceId, bool autoManaged = true, const Core::Rtti* resType = 0);
    /// destructor
    ~ResourceRequestInfo(void);

    /// returns resourceId
    const ResourceId& GetResourceId() const;
    /// returns rtti of resource
    const Core::Rtti* GetResType() const;
    /// returns true if resource is autoManaged
    bool IsAutoManaged() const;

    /// sets resourceId
    void SetResourceId(const ResourceId& resourceId);
    /// sets the resource type
    void SetResType(const Core::Rtti* resType);
    /// sets whether requested resource is autoManaged or not
    void SetAutoManaged(bool autoManaged);

    /// prints out debug-informations
    virtual void DebugPrint() const;

protected:
    ResourceId resourceId;
    bool autoManaged;
    const Core::Rtti* resType;
};

//------------------------------------------------------------------------------
/**
*/
inline const ResourceId&
ResourceRequestInfo::GetResourceId() const
{
    return this->resourceId;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceRequestInfo::SetResourceId(const ResourceId& resourceId)
{
    this->resourceId = resourceId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Core::Rtti*
ResourceRequestInfo::GetResType() const
{
    return this->resType;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceRequestInfo::SetResType(const Core::Rtti* resType)
{
    this->resType = resType;
}
//------------------------------------------------------------------------------
inline bool
ResourceRequestInfo::IsAutoManaged() const
{
    return this->autoManaged;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceRequestInfo::SetAutoManaged(bool autoManaged)
{
    this->autoManaged = autoManaged;
}
} // namespace Resources
//------------------------------------------------------------------------------