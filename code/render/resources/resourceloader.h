#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ResourceLoader
    
    A resource loader is responsible to setup a resource object with valid
    data. 
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ResourceLoader : public Core::RefCounted
{
    __DeclareClass(ResourceLoader);
public:
    /// constructor
    ResourceLoader();
    /// destructor
    virtual ~ResourceLoader();
    
    /// called when the resource loader is attached to its resource
    virtual void OnAttachToResource(const Ptr<Resource>& res);
    /// called when the resource loader is removed from its resource
    virtual void OnRemoveFromResource();
    /// return true if attached to resource
    bool IsAttachedToResource() const;
    /// get pointer to resource
    const Ptr<Resource>& GetResource() const;

    /// return true if asynchronous loading is supported
    virtual bool CanLoadAsync() const;
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
    /// called by resource to cancel a pending load
    virtual void OnLoadCancelled();
    /// call frequently while after OnLoadRequested() to put Resource into loaded state
    virtual bool OnPending();
    /// return current state
    Resource::State GetState() const;
    /// resets loader-stats e.g. state
    virtual void Reset();

protected:
    /// set current state
    void SetState(Resource::State s);

    Ptr<Resource> resource;
    Resource::State state;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ResourceLoader::SetState(Resource::State s)
{
    this->state = s;
}

//------------------------------------------------------------------------------
/**
*/
inline Resource::State
ResourceLoader::GetState() const
{
    return this->state;
}

} // namespace Resource
//------------------------------------------------------------------------------
    