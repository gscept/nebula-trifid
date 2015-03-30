#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FramePostEffect
    
    A frame post-effect implements draws a fullscreen quad through a
    shader which implements the post effect. Additionally it is
    possible to add render batches to a post effect, these will
    be called after the fullscreen quad is rendered, allowing the
    application to render stuff on top of the final frame.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "frame/framepassbase.h"
#include "renderutil/drawfullscreenquad.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FramePostEffect : public FramePassBase
{
    __DeclareClass(FramePostEffect);
public:


    /// constructor
    FramePostEffect();
    /// destructor
    virtual ~FramePostEffect();
    /// setup the post effect
    void Setup();
    /// discard the post effect
    virtual void Discard();
    /// render the post effect
    virtual void Render();

    /// called from frame shader whenever the display resizes
	virtual void OnDisplayResize(SizeT width, SizeT height);

#if NEBULA3_ENABLE_PROFILING
    /// add batch profiler
    void SetFramePostEffectDebugTimer(const Util::String& name);
#endif

private:
    _declare_timer(debugTimer);
    RenderUtil::DrawFullScreenQuad drawFullScreenQuad;
};
} // namespace Frame
//------------------------------------------------------------------------------

     