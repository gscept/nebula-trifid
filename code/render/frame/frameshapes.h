#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameShapes
    
    Special pass used to render simple shapes.
    
    (C) 2011 gscept
*/
#include "frame/framepassbase.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameShapes : public FramePassBase
{
	__DeclareClass(FrameShapes);
public:
	/// constructor
	FrameShapes();
	/// destructor
	virtual ~FrameShapes();

	/// discard the frame shapes pass
	virtual void Discard();
	/// render the shapes
	virtual void Render();

#if NEBULA3_ENABLE_PROFILING
	/// add batch profiler
	void SetFrameShapesPassDebugTimer(const Util::String& name);
#endif

protected:   
	_declare_timer(debugTimer);
}; 
} // namespace Frame
//------------------------------------------------------------------------------