#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::StreamingTexturePageHandler

    Http page handler, that provides statistics and control of texture streaming.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "http/httprequesthandler.h"
#include "util/stringatom.h"
#include "resources/streaming/poolresourcemapper.h"
//------------------------------------------------------------------------------
namespace Resources
{
    class ResourcePool;
}
namespace Http
{
    class HtmlPageWriter;
}
namespace Debug
{
class StreamingTexturePageHandler : public Http::HttpRequestHandler
{
    __DeclareClass(StreamingTexturePageHandler);
public:
    /// constructor
    StreamingTexturePageHandler();
    /// handle a http request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);

private:
    /// handle HTTP request for single pool information
    void HandlePoolRequest(const Util::String& poolName, const Ptr<Http::HttpRequest>& request);
    /// handle HTTP request for single slot information
    void HandleSlotRequest(const Util::String& poolName, const Util::String& slotName, const Ptr<Http::HttpRequest>& request);
    /// returns hex code version of given values (between 0.0 and 1.0)
    Util::String GetHexColorCode(float colorValue);

    /// handle a "raw" texture image request
    Http::HttpStatus::Code HandleImageRequest(const Util::Dictionary<Util::String,Util::String>& query, const Ptr<IO::Stream>& responseStream);
    /// add SVG
    bool StreamingTexturePageHandler::SVGChart(const Util::Dictionary<Util::String,Util::String>& query, const Ptr<Http::HttpRequest>& request);
    /// add pool info HEADERS like width, height, num mips, ...
    void AddPoolHeaders(const Ptr<Http::HtmlPageWriter>& htmlWriter, bool extended);
    /// add pool info DATA like width, height, num mips, ...
    void AddPoolData(const Ptr<Resources::ResourcePool>& pool, const Ptr<Http::HtmlPageWriter>& htmlWriter, bool extended);

    /// writes modified pool settings to xml
    void WriteCurrentPoolSettingToXML();
    /// load slot-amount of pools from xml
    void LoadPoolXML();
    /// updates slot numbers of pools using given query
    void UpdateSlotSets(const Util::Dictionary<Util::String,Util::String>& query);

    Util::Array<Util::String> dictSort;
    Util::Dictionary<Util::StringAtom, Resources::PoolSetupInfo> poolSetupDict;

    IO::URI defaultPoolName;
    IO::URI modifiedPoolName;

};

} // namespace FrameCapture
//------------------------------------------------------------------------------
