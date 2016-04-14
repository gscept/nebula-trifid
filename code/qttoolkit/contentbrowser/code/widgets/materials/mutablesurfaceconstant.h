#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::MutableSurfaceConstant
	
	Similar to MutableSurfaceMaterial in that it exposes functions in 
	surface constants which should not be accessible during runtime.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "materials/surfaceconstant.h"
namespace Materials
{
class MutableSurfaceConstant : public SurfaceConstant
{
	__DeclareClass(MutableSurfaceConstant);
public:
	/// constructor
	MutableSurfaceConstant();
	/// destructor
	virtual ~MutableSurfaceConstant();

protected:
	friend class MutableSurface;
};
} // namespace Materials