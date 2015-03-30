#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameLights
    
    Special pass used to handle the rendering of lights, instead of using batchtypes
    
    (C) 2011 gscept
*/
#include "frame/framepassbase.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameLights : public FramePassBase
{
	__DeclareClass(FrameLights);
public:
	/// constructor
	FrameLights();
	/// destructor
	virtual ~FrameLights();
	/// discard the frame light pass
	virtual void Discard();
	/// render the lights
	virtual void Render();

#if NEBULA3_ENABLE_PROFILING
	/// add batch profiler
	void SetFrameLightPassDebugTimer(const Util::String& name);
#endif

protected:   
	_declare_timer(debugTimer);
}; 
} // namespace Frame
//------------------------------------------------------------------------------