#pragma once
//------------------------------------------------------------------------------
/**
    @class FrameCapture::FrameCapturePageHandler

    Http page handler, that provides control over the frame capture render module.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "http/httprequesthandler.h"

//------------------------------------------------------------------------------
namespace FrameCapture
{
    class FrameCapturePageHandler : public Http::HttpRequestHandler
    {
        __DeclareClass(FrameCapturePageHandler);
    public:
        /// constructor
        FrameCapturePageHandler();
        /// handle a http request, the handler is expected to fill the content stream with response data
        virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);        
    };

} // namespace FrameCapture
//------------------------------------------------------------------------------