//------------------------------------------------------------------------------
//  resourceinfo.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourceinfo.h"

namespace Resources
{
//------------------------------------------------------------------------------
/**
*/
ResourceInfo::ResourceInfo() :
    estimatedSize(0)
{
}

//------------------------------------------------------------------------------
/**
*/
ResourceInfo::ResourceInfo(const ResourceInfo* info) :
    estimatedSize(info->estimatedSize)
{
}

//------------------------------------------------------------------------------
/**
*/
ResourceInfo::~ResourceInfo()
{
}
} // namespace Resources
//------------------------------------------------------------------------------