#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::FmodStudioPageHandler
  
    Provide a HTML debug page for the FmodStudio subsystem.
    
    Usage:
    http://host/fmodstudio
        
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/    
#include "http/httprequesthandler.h"
#include "resources/resourceid.h"
#include "http/html/htmlpagewriter.h"
#include "http/httpstatus.h"

namespace FMOD { class EventGroup; }
namespace Http { class HtmlPageWriter; }

//------------------------------------------------------------------------------
namespace Debug
{
class FmodStudioPageHandler : public Http::HttpRequestHandler
{
    __DeclareClass(FmodStudioPageHandler);
public:
    /// constructor
    FmodStudioPageHandler();
    /// handle a http request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);

private:
    /// handle a projectlist request
    Http::HttpStatus::Code HandleBankInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// handle a memory request
    Http::HttpStatus::Code HandleMemoryInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// handle an events request
    Http::HttpStatus::Code HandleEventInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// write event-group data to stream recusrively
    void DumpGroup(char *projectName, Util::String groupName, FMOD::EventGroup *group, const Ptr<Http::HtmlPageWriter> &htmlWriter) const;
};

} // namespace Debug

//------------------------------------------------------------------------------
