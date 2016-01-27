//------------------------------------------------------------------------------
//  managedresource.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/managedresource.h"

namespace Resources
{
__ImplementClass(Resources::ManagedResource, 'MGRS', Core::RefCounted);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
ManagedResource::ManagedResource() :
    resType(0),
    clientCount(0),
    renderCount(0),
    priority(NormalPriority),
    autoManaged(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ManagedResource::~ManagedResource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method resets the current render stats and is usually called
    during the Prepare() method of the ResourceManager (before
    rendering is started for the current frame).
*/
void
ManagedResource::ClearRenderStats()
{    
    this->renderCount = 0;
    this->resourceStreamingLevelOfDetail = 0.0f;
    this->maxScreenSpaceSize.set(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
/**
    This method is called by the resource client during rendering to
    write back render statistics. If the resource isn't rendered,
    the method MUST NOT be called. If the resource is rendered, the client
    must provide a screen space size guesstimate which will be used
    by the ResourceMapper to bump or drop the lod of the resource.
*/
void
ManagedResource::UpdateRenderStats(float lod)
{
    this->renderCount++;
    if (this->resourceStreamingLevelOfDetail < lod)
    {
        this->resourceStreamingLevelOfDetail = lod;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by the resource client during rendering to
    write back render statistics. If the resource isn't rendered,
    the method MUST NOT be called. If the resource is rendered, the client
    must provide a screen space size guesstimate which will be used
    by the ResourceMapper to bump or drop the lod of the resource.
*/
void
ManagedResource::UpdateRenderStats(const float2& screenSpaceSize)
{
    this->renderCount++;
    this->maxScreenSpaceSize = float2::maximize(this->maxScreenSpaceSize, screenSpaceSize);
}
} // namespace Resources