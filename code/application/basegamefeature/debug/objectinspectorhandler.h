#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::ObjectInspectorHandler
    
    Provide information about all registered game entities to 
    the debug http server.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "http/httprequesthandler.h"

namespace Http
{
    class HtmlPageWriter;
}
//------------------------------------------------------------------------------
namespace Debug
{
class ObjectInspectorHandler : public Http::HttpRequestHandler
{
    __DeclareClass(ObjectInspectorHandler);
public:
    /// constructor
    ObjectInspectorHandler();
    /// handle a http request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<Http::HttpRequest>& request);

private:
    void WriteEntityInfo( const Util::String &category, const Ptr<Http::HtmlPageWriter>& htmlWriter, int id = -1 );
    /// show formular for entity attr changes
    void WriteEntityAttrEditFormular(const Util::String &category, IndexT entityId, const Util::String& attrIdName, const Ptr<Http::HtmlPageWriter>& htmlWriter );
    /// apply attribute value
    void ApplyAttributeValue(IndexT entityId, const Util::String& attrIdName, const Util::String& valueStr);

};

} // namespace Debug
//------------------------------------------------------------------------------

    