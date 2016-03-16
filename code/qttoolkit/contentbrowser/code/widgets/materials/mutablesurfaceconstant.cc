//------------------------------------------------------------------------------
//  mutablesurfaceconstant.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurfaceconstant.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurfaceConstant, 'MUSC', Materials::SurfaceConstant);

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