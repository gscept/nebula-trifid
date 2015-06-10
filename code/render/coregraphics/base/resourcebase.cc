//------------------------------------------------------------------------------
//  resourcebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/resourcebase.h"

namespace Base
{
__ImplementClass(Base::ResourceBase, 'RSBS', Resources::Resource);

//------------------------------------------------------------------------------
/**
*/
ResourceBase::ResourceBase() :
    usage(UsageImmutable),
    access(AccessNone),
	syncing(SyncingSimple)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ResourceBase::~ResourceBase()
{
    // empty
}

} // namespace Base