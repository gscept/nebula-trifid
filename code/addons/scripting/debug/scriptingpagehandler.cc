//------------------------------------------------------------------------------
//  scriptingpagehandler.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scripting/debug/scriptingpagehandler.h"
#include "http/html/htmlpagewriter.h"
#include "scripting/scriptserver.h"
#include "scripting/command.h"

namespace Debug
{
__ImplementClass(Debug::ScriptingPageHandler, 'DSPH', Http::HttpRequestHandler);

using namespace Scripting;
using namespace Http;
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ScriptingPageHandler::ScriptingPageHandler()
{
    this->SetName("Scripting");
    this->SetDesc("show debug information about Scripting subsystem");
    this->SetRootLocation("scripting");
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingPageHandler::HandleRequest(const Ptr<HttpRequest>& request) 
{
    n_assert(HttpMethod::Get == request->GetMethod());

    // configure a HTML page writer
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("NebulaT Scripting Subsystem Info");
    if (htmlWriter->Open())
    {
        ScriptServer* scriptServer = ScriptServer::Instance();
        String str;
        htmlWriter->Element(HtmlElement::Heading1, "Scripting Subsystem");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
        htmlWriter->Text("Script Server class is: ");
        htmlWriter->Element(HtmlElement::Bold, scriptServer->GetClassName());

        htmlWriter->Element(HtmlElement::Heading3, "Registered Commands");
        SizeT numCommands = scriptServer->GetNumCommands();
        IndexT cmdIndex;
        for (cmdIndex = 0; cmdIndex < numCommands; cmdIndex++)
        {
            const Ptr<Command>& cmd = scriptServer->GetCommandByIndex(cmdIndex);
            htmlWriter->Element(HtmlElement::Heading4, cmd->GetName());
            htmlWriter->Raw(cmd->GetHelp());
            htmlWriter->LineBreak();
            htmlWriter->Text(cmd->GetSyntax());
            htmlWriter->LineBreak();
        }
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {   
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

} // namespace Debug
