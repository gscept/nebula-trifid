//------------------------------------------------------------------------------
//  loadingresource.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loadingresource.h"
#include "resources/resourceloader.h"

namespace Resources
{
    __ImplementClass(Resources::LoadingResource, 'LDRS', Core::RefCounted);
    
//------------------------------------------------------------------------------
/**
    Do everything needed in here which has to be performed as a loading request
    is going to be terminated because it was canceled, aborted, failed and so on.
*/
void
LoadingResource::OnCancelRequest()
{
    n_assert(this->targetResource.isvalid());
    n_assert(this->targetResource->GetLoader().isvalid());
    
    this->targetResource->Unlock();
}

//------------------------------------------------------------------------------
/**
    Do everything needed in here which has to be performed as a loading request
    is going to be terminated because it was successful and is no longer needed.
*/
void
LoadingResource::OnSuccessRequest()
{
    // perform some checks
    n_assert(this->targetResource.isvalid());
    n_assert(this->targetResource->GetState() == Resource::Loaded);
    n_assert(this->targetResource->GetLoader().isvalid());
    n_assert(this->managedResource.isvalid());
    n_assert(this->managedResource->GetResourceId() == this->targetResource->GetResourceId());

    this->managedResource->SetResource(this->targetResource);
    this->targetResource->Unlock();
}

} // namespace Resources
//------------------------------------------------------------------------------