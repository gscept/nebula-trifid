#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceMapper
    
    Subclasses of ResourceMapper implement specific resource management 
    strategies for one resource type (texture, mesh, etc...). Applications
    may implement their own specialized ResourceMappers if the provided 
    standard mappers don't fit their needs. ResourceMappers
    are attached to the ResourceManager (one per resource type) and are 
    called back by the resource server to perform resource creation
    and management. Resource clients never talk directly to ResourceMappers,
    instead they call the ResourceManager which in turn talks to the
    ResourceMappers.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/resourceid.h"
#include "resources/resourceloader.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ManagedResource;

class ResourceMapper : public Core::RefCounted
{
    __DeclareClass(ResourceMapper);
public:
    /// constructor
    ResourceMapper();
    /// destructor
    virtual ~ResourceMapper();

    /// set a placeholder resource id
    void SetPlaceholderResourceId(const ResourceId& resId);
    /// get placeholder resource id
    const ResourceId& GetPlaceholderResourceId() const;
    /// set asynchronous behaviour (default is asynchronous)
    void SetAsyncEnabled(bool b);
    /// return asynchronous loading state
    bool IsAsyncEnabled() const;

    /// get resource type handled by this resource mapper
    virtual const Core::Rtti& GetResourceType() const;
    /// called from resource manager when mapper is attached
    virtual void OnAttachToResourceManager();
    /// called from resource manager when mapper is removed
    virtual void OnRemoveFromResourceManager();
    /// return true if currently attached to server
    bool IsAttachedToResourceManager() const;
    /// called when a managed resource should be created
    virtual Ptr<ManagedResource> OnCreateManagedResource(const Core::Rtti& resType, const ResourceId& resourceId, const Ptr<ResourceLoader>& optResourceLoader, bool forceSync );
    /// called when a managed resource should be discarded
    virtual void OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource);
    /// called before gathering render stats
    virtual void OnPrepare(bool waiting);
    /// called after gathering render stats to perform resource management
    virtual void OnUpdate(IndexT frameIndex);
    /// return the number of currently pending resources
    virtual SizeT GetNumPendingResources() const;

protected:
    ResourceId placeholderResourceId;
    bool asyncEnabled;
    bool isAttached;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceMapper::IsAttachedToResourceManager() const
{
    return this->isAttached;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceMapper::SetPlaceholderResourceId(const ResourceId& resId)
{
    n_assert(!this->IsAttachedToResourceManager());
    this->placeholderResourceId = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const ResourceId&
ResourceMapper::GetPlaceholderResourceId() const
{
    return this->placeholderResourceId;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceMapper::SetAsyncEnabled(bool b)
{
    this->asyncEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceMapper::IsAsyncEnabled() const
{
    return this->asyncEnabled;
}

} // namespace Resources
//------------------------------------------------------------------------------


    
    