//------------------------------------------------------------------------------
//  mutablesurfaceinstance.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurfaceinstance.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurfaceInstance, 'MUSI', Core::RefCounted);

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