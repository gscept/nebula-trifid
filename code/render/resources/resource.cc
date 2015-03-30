//------------------------------------------------------------------------------
//  resource.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resource.h"
#include "resources/resourceloader.h"
#include "resources/resourcesaver.h"
#include "io/ioserver.h"

namespace Resources
{
__ImplementClass(Resources::Resource, 'RSRC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Resource::Resource() :
    state(Initial),
    useCount(0),
    asyncEnabled(false),
    locked(false),
	reload(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Resource::~Resource()    
{
    n_assert(!this->IsLoaded());
    n_assert(!this->IsPending());
    n_assert(!this->loader.isvalid());
    n_assert(!this->saver.isvalid());
}

//------------------------------------------------------------------------------
/**
    This loads the resource through the attached resource loader. Depending
    on the resource loader, the resource may happen synchronously or
    asynchronously. If the resource is loaded asynchronously, the IsPending()
    method will return true as long as the load is in progress, and IsLoaded()
    will become true when the loading process has finished. If the load
    has failed, IsPending() will switch to false and IsLoaded() will not
    be true.
*/
Resource::State
Resource::Load()
{
    n_assert(this->loader.isvalid());
    n_assert(!this->IsLoaded());
    if (this->IsPending())
    {
        // if currently pending, check the resource loader
        // if ready to set us up (the bomb)
        this->loader->OnPending();
    }
    else
    {
        // not pending, request loading
        this->loader->OnLoadRequested();
    }
    this->SetState(this->loader->GetState());
    
    // check for error
    if (Failed == this->state)
    {
        n_printf("Failed to load resource '%s'!\n", this->resourceId.Value());
    }
    return this->state;
}

//------------------------------------------------------------------------------
/**
    This will unload the resource. Only call the method when IsLoaded()
    return true. To cancel a pending asynchronous loading process, call
    the CancelPendingLoad() method.
*/
void
Resource::Unload()
{
    if (this->IsPending())
    {
        this->loader->OnLoadCancelled();
    }
    this->SetState(Initial);
}


//------------------------------------------------------------------------------
/**
*/
void
Resource::Reload()
{
	// first unload the resource
	this->Unload();

	// reset loader
	this->loader->Reset();

	// get async flag
	bool asyncEnabled = this->asyncEnabled;

	// disable asynconicy here
	this->SetAsyncEnabled(false);

	// then simply load it again
	this->Load();

	// reset to previous state
	this->SetAsyncEnabled(asyncEnabled);
}


//------------------------------------------------------------------------------
/**
    This will save the resource. A resource saver must be attached to the
    resource and the resource must be loaded for the method to succeed. 
    Saving will always be performed synchronously.
*/
bool
Resource::Save()
{
    n_assert(this->IsLoaded());
    n_assert(this->saver.isvalid());
    bool success = this->saver->OnSave();
    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
Resource::SetLoader(const Ptr<ResourceLoader>& l)
{
    if (this->loader.isvalid())
    {
        this->loader->OnRemoveFromResource();
        this->loader = 0;
    }
    if (l.isvalid())
    {
        this->loader = l;
        this->loader->OnAttachToResource(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceLoader>&
Resource::GetLoader() const
{
    return this->loader;
}

//------------------------------------------------------------------------------
/**
*/
void
Resource::SetSaver(const Ptr<ResourceSaver>& s)
{
    if (this->saver.isvalid())
    {
        this->saver->OnRemoveFromResource();
        this->saver = 0;
    }
    if (s.isvalid())
    {
        this->saver = s;
        this->saver->OnAttachToResource(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ResourceSaver>&
Resource::GetSaver() const
{
    return this->saver;
}


} // namespace Resources