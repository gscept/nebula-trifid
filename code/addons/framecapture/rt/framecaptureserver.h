#pragma once
//------------------------------------------------------------------------------
/**
	@class FrameCapture::FrameCaptureServer

    Provide continuous frame capture to file at a fixed frame rate.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/imagefileformat.h"
#include "timing/time.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace FrameCapture
{
class FrameCaptureServer : public Core::RefCounted
{
    __DeclareClass(FrameCaptureServer);
    __DeclareSingleton(FrameCaptureServer);
public:
    /// constructor
    FrameCaptureServer();
    /// destructor
    virtual ~FrameCaptureServer();

    /// set base directory
    void SetBaseDirectory(const Util::String& n);
    /// get base directory
    const Util::String& GetBaseDirectory() const;
    /// set capture frame rate in seconds (def is 1/25)
    void SetFrameTime(Timing::Time t);
    /// get capture frame rate in seconds
    Timing::Time GetFrameTime() const;
    /// set capture file format (default is JPG)
    void SetCaptureFileFormat(CoreGraphics::ImageFileFormat::Code fmt);
    /// get capture file format
    CoreGraphics::ImageFileFormat::Code GetCaptureFileFormat() const;
    /// start a new capture session
    bool Start();
    /// stop the current capture session
    void Stop();
    /// return true if currently capturing
    bool IsCapturing() const;
    /// trigger the capture server, call after rendering is complete
    bool Trigger();
    /// create an automatically named screen shot
    Util::String SaveScreenShot();

private:
    /// find next session index
    void SetToNextSessionIndex();
    /// return full directory name of current session
    Util::String GetSessionDirectoryPath();
    /// capture the current frame
    bool CaptureFrame();
    /// create session directory for new session
    bool CreateSessionDirectory();
    /// Sets the RenderTarget offset variable to the right value 
    void SetCorrectRenderTargetOffset(int numTilesX, int numTilesY, int tileXNum, int tileYNum, bool reset);

    CoreGraphics::ImageFileFormat::Code captureFileFormat;
    bool isCapturing;
    Timing::Time frameTime;
    Util::String baseDirName;
    int sessionIndex;
    int frameIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
FrameCaptureServer::SetBaseDirectory(const Util::String& n)
{
    this->baseDirName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
FrameCaptureServer::GetBaseDirectory() const
{
    return this->baseDirName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FrameCaptureServer::SetFrameTime(Timing::Time t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
FrameCaptureServer::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
FrameCaptureServer::SetCaptureFileFormat(CoreGraphics::ImageFileFormat::Code fmt)
{
    this->captureFileFormat = fmt;
}

//------------------------------------------------------------------------------
/**
*/
inline
CoreGraphics::ImageFileFormat::Code
FrameCaptureServer::GetCaptureFileFormat() const
{
    return this->captureFileFormat;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
FrameCaptureServer::IsCapturing() const
{
    return this->isCapturing;
}
} // namespace FrameCapture