#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FramePass
    
    A frame pass encapsulates all 3d rendering to a render target, organized
    into FrameBatches.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "frame/framepassbase.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FramePass : public FramePassBase
{
    __DeclareClass(FramePass);
public:
    /// constructor
    FramePass();
    /// destructor
    virtual ~FramePass();
    /// discard the frame pass
    virtual void Discard();
    /// render the pass
    virtual void Render();

#if NEBULA3_ENABLE_PROFILING
    /// add batch profiler
    void SetFramePassDebugTimer(const Util::String& name);
#endif

protected:   
    _declare_timer(debugTimer);
};
} // namespace Frame
//------------------------------------------------------------------------------

    