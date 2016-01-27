//------------------------------------------------------------------------------
// framecaptureserver.cc
// (C) 2010 Radon Labs GmbH
// (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framecaptureserver.h"
#include "io/ioserver.h"
#include "coregraphics/renderdevice.h"

using namespace Util;
using namespace IO;
using namespace CoreGraphics;
//------------------------------------------------------------------------------
namespace FrameCapture
{
__ImplementClass(FrameCapture::FrameCaptureServer,'FCSV',Core::RefCounted);
__ImplementSingleton(FrameCapture::FrameCaptureServer);
//------------------------------------------------------------------------------
/**
    Constructor
*/
FrameCaptureServer::FrameCaptureServer() :
    captureFileFormat(CoreGraphics::ImageFileFormat::BMP),
    isCapturing(false),
    frameTime(0.04),
    baseDirName("temp:"),
    sessionIndex(0),
    frameIndex(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
FrameCaptureServer::~FrameCaptureServer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Start capturing
*/
bool
FrameCaptureServer::Start()
{
    n_assert(!this->IsCapturing());

    n_printf("FrameCaptureServer::Start(): Starting capture session! \n");
    // find next session index 
    this->SetToNextSessionIndex();

    // create the session directory
    if (this->CreateSessionDirectory())
    {
        // lock frame rate
        // FIXME!!!
        //FrameSync::FrameSyncHandlerThread::Instance()->StartFixedFrameTime(this->frameTime);
        this->isCapturing = true;
        this->frameIndex = 0;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Stop capturing
*/
void
FrameCaptureServer::Stop()
{
    n_printf("FrameCaptureServer::Stop(): Stop capture session! \n");
    n_assert(this->IsCapturing());
    // FIXME!!!
    //FrameSync::FrameSyncHandlerThread::Instance()->StopFixedFrameTime();
    this->isCapturing = false;
}

//------------------------------------------------------------------------------
/**
    Returns the name of the current session.
*/
String
FrameCaptureServer::GetSessionDirectoryPath()
{
    String dirName;
    dirName.Format("%s/%04d", this->baseDirName.AsCharPtr(), this->sessionIndex);
    return dirName;
}

//------------------------------------------------------------------------------
/**
    Create a directory for the current session.
*/
bool
FrameCaptureServer::CreateSessionDirectory()
{
    String dirName = this->GetSessionDirectoryPath();
    return IoServer::Instance()->CreateDirectory(dirName);
}

//------------------------------------------------------------------------------
/**
    This finds the next free session index by iterating through session numbers
    and checking whether a session directory of that name already exists.
*/
void
FrameCaptureServer::SetToNextSessionIndex()
{
    String dirName;
    do
    {
        this->sessionIndex++;
        dirName = this->GetSessionDirectoryPath();
    }
    while (IoServer::Instance()->DirectoryExists(dirName));
}

//------------------------------------------------------------------------------
/**
    Capture the current frame to a file.
*/
bool
FrameCaptureServer::CaptureFrame()
{
    n_assert(this->IsCapturing());

    // create file name
    String filename;
    filename.Format("%s/%05d.%s",
                    this->GetSessionDirectoryPath().AsCharPtr(),
                    ++this->frameIndex,
                    CoreGraphics::ImageFileFormat::ToString(this->captureFileFormat).AsCharPtr());
    // create file stream
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(filename);
    // write screen shot
    RenderDevice::Instance()->SaveScreenshot(this->captureFileFormat, stream);
    return true;
}

//------------------------------------------------------------------------------
/**
    Trigger the frame grabber. This will just call CaptureFrame() if a capture
    session is currently open.
*/
bool
FrameCaptureServer::Trigger()
{
    if (this->IsCapturing())
    {
        return this->CaptureFrame();
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Save a single screenshot to a file. Returns filename of saved screenshot.
*/
Util::String
FrameCaptureServer::SaveScreenShot()
{
    bool dirCreated = IoServer::Instance()->CreateDirectory(this->baseDirName);
    if (!dirCreated) 
    {
        String ret;
        ret.Format("Cannot open file %s", this->baseDirName.AsCharPtr());
        return ret;
    }

    const int MAX_SCREENSHOT = 100000;
    int screenShotNum = 0;
    String filename;
    bool hasValidNum = false;
    while (screenShotNum < MAX_SCREENSHOT && !hasValidNum)
    {
        filename.Format("%s/screenshot_%s.%s",
                        this->baseDirName.AsCharPtr(),
                        String::FromInt(screenShotNum).AsCharPtr(),
                        CoreGraphics::ImageFileFormat::ToString(this->captureFileFormat).AsCharPtr()
                        );
        if (IoServer::Instance()->FileExists(filename))
        {
            screenShotNum ++;
        }
        else
        {
            hasValidNum = true;
        }
    }
    
    // create file stream
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(filename);
    
    // write screen shot
    RenderDevice::Instance()->SaveScreenshot(this->captureFileFormat, stream);
    return filename.ExtractFileName();
}
} // namespace FrameCapture