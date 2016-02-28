#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::SimpleResourceMapper
    
    Generic, most simple resource mapper which can be used for all
    types of resources. Asynchronously loads managed resources on demand.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "resources/resourcemapper.h"
#include "resources/managedresource.h"
#include "resources/resourceloader.h"
    
//------------------------------------------------------------------------------
namespace Resources
{
class SimpleResourceMapper : public ResourceMapper
{
    __DeclareClass(SimpleResourceMapper);
public:
    /// constructor
    SimpleResourceMapper();
    /// destructor
    virtual ~SimpleResourceMapper();

    /// config: set resource type handled by resource mapper (e.g. Mesh::RTTI)
    void SetResourceClass(const Core::Rtti& resClass);
    /// config: set resource loader class (e.g. 
    void SetResourceLoaderClass(const Core::Rtti& resLoaderClass);
    /// config: set managed resource type handled by resource (e.g. ManagedMesh::RTTI)
    void SetManagedResourceClass(const Core::Rtti& managedResClass);

    /// get resource type handled by this resource mapper
    virtual const Core::Rtti& GetResourceType() const;
    /// called from resource manager when mapper is attached
    virtual void OnAttachToResourceManager();
    /// called from resource manager when mapper is removed
    virtual void OnRemoveFromResourceManager();
    /// called when a managed resource should be created
    virtual Ptr<ManagedResource> OnCreateManagedResource(const Core::Rtti& resType, const ResourceId& resId, const Ptr<ResourceLoader>& optResourceLoader=0, bool forceSync = false);
    /// called when a managed resource should be discarded
    virtual void OnDiscardManagedResource(const Ptr<ManagedResource>& managedResource);
    /// called before gathering render stats
    virtual void OnPrepare(bool waiting);
    /// called after gathering render stats to perform resource management
    virtual void OnUpdate(IndexT frameIndex);
    /// return the number of currently pending resources
    virtual SizeT GetNumPendingResources() const;

protected:
    const Core::Rtti* resourceClass;
    const Core::Rtti* resLoaderClass;
    const Core::Rtti* managedResourceClass;
    Ptr<Resource> placeholderResource;
    Util::Dictionary<ResourceId,Ptr<ManagedResource> > managedResources;
    Util::Dictionary<ResourceId,Ptr<Resource> > pendingResources;
};

//------------------------------------------------------------------------------
/**
*/
inline void
SimpleResourceMapper::SetResourceClass(const Core::Rtti& resClass)
{
    n_assert(resClass.IsDerivedFrom(Resource::RTTI));
    this->resourceClass = &resClass;
}

//------------------------------------------------------------------------------
/**
*/
inline void
SimpleResourceMapper::SetResourceLoaderClass(const Core::Rtti& loaderClass)
{
    n_assert(loaderClass.IsDerivedFrom(ResourceLoader::RTTI));
    this->resLoaderClass = &loaderClass;
}

//------------------------------------------------------------------------------
/**
*/
inline void
SimpleResourceMapper::SetManagedResourceClass(const Core::Rtti& managedResClass)
{
    n_assert(managedResClass.IsDerivedFrom(ManagedResource::RTTI));
    this->managedResourceClass = &managedResClass;
}

} // namespace Resources
//------------------------------------------------------------------------------
    