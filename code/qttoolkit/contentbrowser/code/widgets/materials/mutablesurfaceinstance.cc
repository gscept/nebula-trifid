//------------------------------------------------------------------------------
//  mutablesurfaceinstance.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurfaceinstance.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurfaceInstance, 'MUSI', Materials::SurfaceInstance);

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceInstance::MutableSurfaceInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceInstance::~MutableSurfaceInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurfaceInstance::Cleanup()
{
    SurfaceInstance::Cleanup();
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurfaceInstance::Setup(const Ptr<Surface>& surface)
{
    SurfaceInstance::Setup(surface);
}

} // namespace Materials