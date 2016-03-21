#pragma once
#ifndef DEBUG_SCRIPTINGPAGEHANDLER_H
#define DEBUG_SCRIPTINGPAGEHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Debug::ScriptingPageHandler
  
    Provide information about Scripting subsystem to debug http server.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "http/httprequesthandler.h"

//------------------------------------------------------------------------------
namespace Debug
{
class ScriptingPageHandler : public Http::HttpRequestHandler
{
    __DeclareClass(ScriptingPageHandler);
public:
    /// constructor
    ScriptingPageHandler();
    /// handle a http request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);
};

}
//------------------------------------------------------------------------------
#endif
