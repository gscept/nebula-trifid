#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::MutableSurfaceInstance
	
	Implements a surface instance which is mutable, so it can be discarded and setup again.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "materials/surfaceinstance.h"
namespace Materials
{
class MutableSurfaceInstance : public SurfaceInstance
{
	__DeclareClass(MutableSurfaceInstance);
public:
	/// constructor
	MutableSurfaceInstance();
	/// destructor
	virtual ~MutableSurfaceInstance();

    /// cleanup instance
    void Cleanup();
    /// setup instance again
    void Setup(const Ptr<Surface>& surface);
};
} // namespace Materials