#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceSaver
    
    A ResourceSaver object can write out resource data to a file or another
    destination. Saving resources is strictly synchronous (unlike loading,
    which may happen asynchronously).
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;

class ResourceSaver : public Core::RefCounted
{
    __DeclareClass(ResourceSaver);
public:
    /// constructor
    ResourceSaver();
    /// destructor
    virtual ~ResourceSaver();

    /// called when the resource saver is attached to its resource
    virtual void OnAttachToResource(const Ptr<Resource>& res);
    /// called when the resource saver is removed from its resource
    virtual void OnRemoveFromResource();
    /// return true if attached to resource
    bool IsAttachedToResource() const;
    /// get pointer to resource
    const Ptr<Resource>& GetResource() const;

    /// called by resource when a save is requested
    virtual bool OnSave();

protected:
    Ptr<Resource> resource;
};

} // namespace Resources
//------------------------------------------------------------------------------
