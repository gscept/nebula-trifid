//------------------------------------------------------------------------------
// framecapturepagehandler.cc
// (C) 2010 Radon Labs GmbH
// (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framecapture/rt/framecapturepagehandler.h"
#include "framecapture/rt/framecaptureserver.h"
#include "framecapture/framecaptureprotocol.h"
#include "http/html/htmlpagewriter.h"
#include "graphics/graphicsinterface.h"
#include "io/assignregistry.h"

namespace FrameCapture
{

__ImplementClass(FrameCapture::FrameCapturePageHandler, 'FCPH', Http::HttpRequestHandler);

using namespace Http;
using namespace Graphics;
using namespace CoreGraphics;
//------------------------------------------------------------------------------
/**
*/
FrameCapturePageHandler::FrameCapturePageHandler()
{
    this->SetName("Capture Frames");
    this->SetDesc("Capture rendered content to files.");
    this->SetRootLocation("framecapture");
}

//------------------------------------------------------------------------------
/**
*/
void
FrameCapturePageHandler::HandleRequest(const Ptr<HttpRequest>& request) 
{
    n_assert(HttpMethod::Get == request->GetMethod());
    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Capture Frames");
    if (htmlWriter->Open())
    {      
        Ptr<FrameCaptureServer> captureServer = FrameCaptureServer::Instance();
        
        // get capture commands from request uri
        Util::String screenshotName;
        Util::Dictionary<Util::String,Util::String> queryArgs = request->GetURI().ParseQuery();
        if (queryArgs.Contains("command"))
        {
            if ("startcapture" == queryArgs["command"])
            {
                if (!captureServer->IsCapturing())
                {
                    captureServer->Start();
                }
            }
            else if ("stopcapture" == queryArgs["command"])
            {
                if (captureServer->IsCapturing())
                {
                    captureServer->Stop();
                }
            }
            else if ("screenshot" == queryArgs["command"])
            {
                screenshotName = captureServer->SaveScreenShot();
            }
        }
        
        // head of page
        htmlWriter->Element(HtmlElement::Heading1, "Capture Frames");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        
        // screenshot path, if screenshot was taken
        if (screenshotName.IsValid())
        {
            Util::String dirname =
                IO::AssignRegistry::Instance()->ResolveAssignsInString(
                captureServer->GetBaseDirectory());
            dirname.TrimRight("/");
            Util::String path;
            path.Format("%s/%s",dirname.AsCharPtr(),screenshotName.AsCharPtr());
            htmlWriter->AddAttr("style","color: #008800;");
            htmlWriter->Element(HtmlElement::Span,"Screenshot saved at:");
            htmlWriter->LineBreak();
            htmlWriter->Element(HtmlElement::Teletyper,path);
            htmlWriter->LineBreak();
        }

        // capture status if currently capturing
        if (captureServer->IsCapturing())
        {
            htmlWriter->Begin(HtmlElement::Paragraph);
                htmlWriter->AddAttr("style","color: red; font-weight: bold;");
                htmlWriter->Element(HtmlElement::Span,"Currently capturing...");
            htmlWriter->End(HtmlElement::Paragraph);
        }
        
        // settings table
        htmlWriter->Begin(HtmlElement::Paragraph);
            htmlWriter->Element(HtmlElement::Heading3,"Current capture settings:");
            htmlWriter->Begin(HtmlElement::Table);
                htmlWriter->TableRow2("Capture path: ",captureServer->GetBaseDirectory());    
                htmlWriter->TableRow2("Frames Per Second: ",
                                          Util::String::FromInt((int)(1.0 / captureServer->GetFrameTime())));
                htmlWriter->TableRow2("Capture Format: ",
                                          ImageFileFormat::ToString(captureServer->GetCaptureFileFormat()));
            htmlWriter->End(HtmlElement::Table);
        htmlWriter->End(HtmlElement::Paragraph);
        
        // capture control
        if (captureServer->IsCapturing())
        {
            htmlWriter->AddAttr("href","framecapture?command=stopcapture");
            htmlWriter->Element(HtmlElement::Anchor, "Stop capturing");
        }
        else
        {
            htmlWriter->AddAttr("href","framecapture?command=startcapture");
            htmlWriter->Element(HtmlElement::Anchor, "Start capturing");
        }
        
        // screenshot control
        htmlWriter->AddAttr("href","framecapture?command=screenshot");
        htmlWriter->Element(HtmlElement::Anchor, "Take screenshot");
        
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

} // namespace FrameCapture