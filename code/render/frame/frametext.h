#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameText
    
    Special class to handle text rendering passes
    
    (C) 2011 gscept
*/
#include "frame/framepassbase.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameText : public FramePassBase
{
	__DeclareClass(FrameText);
public:
	/// constructor
	FrameText();
	/// destructor
	virtual ~FrameText();

	/// discard the frame text pass
	virtual void Discard();
	/// render the text
	virtual void Render();

#if NEBULA3_ENABLE_PROFILING
	/// add batch profiler
	void SetFrameTextPassDebugTimer(const Util::String& name);
#endif

protected:   
	_declare_timer(debugTimer);
}; 
} // namespace Frame
//------------------------------------------------------------------------------