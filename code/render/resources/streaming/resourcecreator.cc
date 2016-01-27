//------------------------------------------------------------------------------
//  resourcecreator.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcecreator.h"
#include "resources/resource.h"

namespace Resources
{
__ImplementClass(Resources::ResourceCreator, 'RSCR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceCreator::ResourceCreator()
{}

//------------------------------------------------------------------------------
/**
*/
ResourceCreator::~ResourceCreator()
{}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
ResourceCreator::CreateResource(const ResourceInfo* resourceInfo)
{
    n_error("ResourceCreator::CreateResource called. Overload this method in subclasses.");
    return 0;
}
} // namespace Resources
//------------------------------------------------------------------------------