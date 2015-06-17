//------------------------------------------------------------------------------
//  mutablesurfaceconstant.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurfaceconstant.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurfaceConstant, 'MUSC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceConstant::MutableSurfaceConstant()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceConstant::~MutableSurfaceConstant()
{
	// empty
}

} // namespace Materials