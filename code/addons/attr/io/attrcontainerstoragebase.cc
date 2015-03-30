//------------------------------------------------------------------------------
//  attrcontainerstoragebase.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attr/io/attrcontainerstoragebase.h"

namespace Attr
{
__ImplementAbstractClass(AttrContainerStorageBase, 'ACSB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
AttrContainerStorageBase::AttrContainerStorageBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttrContainerStorageBase::~AttrContainerStorageBase()
{
    // empty
}

}