#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameGUI
    
    Special pass to handle rendering the GUI
    
    (C) 2011 gscept
*/
#include "frame/framepassbase.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameGUI : public FramePassBase
{
	__DeclareClass(FrameGUI);
public:
	/// constructor
	FrameGUI();
	/// destructor
	virtual ~FrameGUI();
	/// discard the frame gui pass
	virtual void Discard();
	/// render the gui
	virtual void Render();

#if NEBULA3_ENABLE_PROFILING
	/// add batch profiler
	void SetFrameGUIPassDebugTimer(const Util::String& name);
#endif

protected:   
	_declare_timer(debugTimer);
}; 
} // namespace Frame
//------------------------------------------------------------------------------