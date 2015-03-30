//------------------------------------------------------------------------------
//  resourcerequestinfo.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcerequestinfo.h"

namespace Resources
{

//------------------------------------------------------------------------------
/**
*/
ResourceRequestInfo::ResourceRequestInfo() :
    resourceId("not set"),
    autoManaged(true),
    resType(0)
{}

//------------------------------------------------------------------------------
/**
*/
ResourceRequestInfo::ResourceRequestInfo(const ResourceId& resourceId, bool autoManaged, const Core::Rtti* resType) :
    resourceId(resourceId),
    autoManaged(autoManaged),
    resType(resType)
{}

//------------------------------------------------------------------------------
/**
*/
ResourceRequestInfo::~ResourceRequestInfo(void)
{
}


//------------------------------------------------------------------------------
/**
*/
void
ResourceRequestInfo::DebugPrint() const
{
    n_printf("ResourceId: ");
    n_printf(this->resourceId.AsString().AsCharPtr());
    n_printf("autoManaged: ");
    if (this->autoManaged)
    {
        n_printf("true\n");
    }
    else
    {
        n_printf("false\n");
    }
}
} // namespace Resources
//------------------------------------------------------------------------------