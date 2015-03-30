#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::LoadingResource

    A LoadingResource contains a Resource we want to load and a ManagedResource which may
    have a pointer towards a Resource we may reuse for loading.
    ResourceCache keeps an sorted Array of LoadingResources
    to determine loading-order of requested resources.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/managedresource.h"

//------------------------------------------------------------------------------
namespace Resources
{

class LoadingResource : public Core::RefCounted
{
    __DeclareClass(LoadingResource);
public:
    /// constructor
    LoadingResource();
    /// destructor
    ~LoadingResource();
    
    /// less-than operator
    friend bool operator < (const LoadingResource& a, const LoadingResource& b);

    /// returns loading-priority-value
    int GetPriority() const;
    /// returns ManagedResource
    const Ptr<ManagedResource>& GetManagedResource() const;
    /// returns Resource we want to load to
    const Ptr<Resource>& GetTargetResource() const;

    /// set ManagedResource
    void SetManagedResource(const Ptr<ManagedResource>& managedResource);
    /// set target resource (resource we want to load)
    void SetTargetResource(const Ptr<Resource>& targetResource);
    /// set the loading-priority
    void SetPriority(int priority);

    /// called as the request is aborted (canceled, aborted, failed, ...)
    virtual void OnCancelRequest();
    /// called as the request was performed successfully
    virtual void OnSuccessRequest();

protected:
    /// the managed resource we place on the resource after successful loading
    Ptr<ManagedResource> managedResource;
    /// the resource we want to load
    Ptr<Resource> targetResource;
    int priority;
};

//------------------------------------------------------------------------------
/**
*/
inline
LoadingResource::LoadingResource() :
    priority(0)
{

}

//------------------------------------------------------------------------------
/**
*/
inline
LoadingResource::~LoadingResource()
{

}

//------------------------------------------------------------------------------
/**
*/
inline bool
operator < (const LoadingResource& a, const LoadingResource& b)
{
    return a.priority < b.priority;
}

//------------------------------------------------------------------------------
/**
*/
inline int
LoadingResource::GetPriority() const
{
    return this->priority;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<ManagedResource>&
LoadingResource::GetManagedResource() const
{
    return this->managedResource;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resource>&
LoadingResource::GetTargetResource() const
{
    return this->targetResource;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LoadingResource::SetPriority(int priority)
{
    this->priority = priority;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LoadingResource::SetManagedResource(const Ptr<ManagedResource>& managedResource)
{
    this->managedResource = managedResource;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LoadingResource::SetTargetResource(const Ptr<Resource>& targetResource)
{
    this->targetResource = targetResource;
}

} // namespace Resource
//------------------------------------------------------------------------------