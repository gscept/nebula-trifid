//------------------------------------------------------------------------------
//  resourcesaver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourcesaver.h"
#include "resources/resource.h"
#include "resources/resourceloader.h"

namespace Resources
{
__ImplementClass(Resources::ResourceSaver, 'RSSR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceSaver::ResourceSaver()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ResourceSaver::~ResourceSaver()
{
    n_assert(!this->IsAttachedToResource());
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSaver::OnAttachToResource(const Ptr<Resource>& res)
{
    n_assert(!this->IsAttachedToResource());
    this->resource = res;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceSaver::OnRemoveFromResource()
{
    n_assert(this->IsAttachedToResource());
    this->resource = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceSaver::IsAttachedToResource() const
{
    return this->resource.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Resource>&
ResourceSaver::GetResource() const
{
    return this->resource;
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceSaver::OnSave()
{
    n_assert(this->IsAttachedToResource());
    return true;
}

} // namespace Resources