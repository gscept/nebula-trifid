#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::FmodCoreAudioPageHandler
  
    Provide a HTML debug page for the CoreAudio subsystem.
    
    Usage:
    http://host/coreaudio
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
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
class FmodCoreAudioPageHandler : public Http::HttpRequestHandler
{
    __DeclareClass(FmodCoreAudioPageHandler);
public:
    /// constructor
    FmodCoreAudioPageHandler();
    /// handle a http request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);

private:
    /// handle a projectlist request
    Http::HttpStatus::Code HandleProjectsInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// handle a memory request
    Http::HttpStatus::Code HandleMemoryInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// handle an events request
    Http::HttpStatus::Code HandleEventInfoRequest(const Ptr<IO::Stream>& responseStream);
    /// write event-group data to stream recusrively
    void DumpGroup(char *projectName, Util::String groupName, FMOD::EventGroup *group, const Ptr<Http::HtmlPageWriter> &htmlWriter) const;
};

} // namespace Debug

//------------------------------------------------------------------------------
