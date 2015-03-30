//------------------------------------------------------------------------------
//  resourceloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourceloader.h"
#include "resources/resource.h"
#include "resources/resourcesaver.h"

namespace Resources
{
__ImplementClass(Resources::ResourceLoader, 'RSLD', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceLoader::ResourceLoader() :
    state(Resource::Initial)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ResourceLoader::~ResourceLoader()
{
    n_assert(!this->IsAttachedToResource());
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceLoader::OnAttachToResource(const Ptr<Resource>& res)
{
    n_assert(!this->IsAttachedToResource());
    this->resource = res;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceLoader::OnRemoveFromResource()
{
    n_assert(this->IsAttachedToResource());
    if (Resource::Pending == this->GetState())
    {
        this->OnLoadCancelled();
    }
    this->resource = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceLoader::IsAttachedToResource() const
{
    return this->resource.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Resource>&
ResourceLoader::GetResource() const
{
    return this->resource;
}

//------------------------------------------------------------------------------
/**
    This method should be overriden in a subclass and indicates whether
    the resource loader supports asynchronous resource loading. If asynchronous
    loading is requested, the OnLoadRequested() method will return immediately
    and the Resource object will be put into Pending state. Afterwards,
    the Resource object needs to poll the ResourceLoader using the OnPending
    method, which will eventually setup the Resource object.
*/
bool
ResourceLoader::CanLoadAsync() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    This method is called by our Resource object to perform a synchronous
    or initiate an asynchronous load. When performing a synchronous load,
    the method should setup the Resource and go into the Done state
    (or Failed state when the load has failed). In asynchronous mode,
    the method should put the resource loader into the Pending state.
*/
bool
ResourceLoader::OnLoadRequested()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This method is called by our Resource object if a pending asynchronous
    load should be cancelled.
*/
void
ResourceLoader::OnLoadCancelled()
{
    this->SetState(Resource::Cancelled);
}

//------------------------------------------------------------------------------
/**
    This method should be called at some time after OnLoadRequested() 
    as long as the ResourceLoader is in the Pending state. This will 
    check whether the asynchronous loader job has finished, and if yes,
    setup the Resource object, bringing it from the Pending into the
    Loaded state. If something goes wrong, the ResourceLoader will
    go into the Failed state. If the outstanding loader job isn't finished
    yet, the ResourceLoader should remain in Pending state, and the
    method should return false. Otherwise the Resource should be
    initialized, and the method should return true.
*/
bool
ResourceLoader::OnPending()
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceLoader::Reset()
{
    if (this->state == Resource::Pending)
    {
        this->OnLoadCancelled();
    }
    this->SetState(Resource::Initial);
}
} // namespace Resources