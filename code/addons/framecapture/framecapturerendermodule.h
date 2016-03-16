#pragma once
//------------------------------------------------------------------------------
/**
    @class FrameCapture::FrameCaptureRenderModule

    The client-side object for the frame capture subsystem

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "rendermodules/rendermodule.h"

namespace FrameCapture
{
class FrameCaptureRenderModule : public RenderModules::RenderModule
{
__DeclareClass(FrameCapture::FrameCaptureRenderModule);
__DeclareSingleton(FrameCapture::FrameCaptureRenderModule);
public:
    /// Constructor
    FrameCaptureRenderModule();
    /// Destructor
    virtual ~FrameCaptureRenderModule();
    /// Setup the render module
    virtual void Setup();
    /// Discard the render module
    virtual void Discard();
};
} // namespace FrameCapture