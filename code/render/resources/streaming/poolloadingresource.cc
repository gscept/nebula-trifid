//------------------------------------------------------------------------------
//  poolloadingresource.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "poolloadingresource.h"
#include "resourceslot.h"

namespace Resources
{
    __ImplementClass(Resources::PoolLoadingResource, 'PLRS', Resources::LoadingResource);

//------------------------------------------------------------------------------
/**
*/
void
PoolLoadingResource::OnSuccessRequest()
{
    // perform some checks
    n_assert(this->newSlot.isvalid());
    n_assert(this->newSlot->GetResource().isvalid());
    n_assert(this->newSlot->GetResource()->GetLoader().isvalid());
    n_assert(this->newSlot->GetResource()->GetState() == Resource::Loaded);
    
    this->newSlot->SetCurrentManagedResource(this->managedResource);
    if(this->oldSlot.isvalid())
    {
        this->oldSlot->SetCurrentManagedResource(0);
        this->oldSlot->GetResource()->Unlock();
    }
    LoadingResource::OnSuccessRequest();
}

} // namespace Resources
//------------------------------------------------------------------------------