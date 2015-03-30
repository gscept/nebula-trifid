#pragma once
#ifndef REMOTE_REMOTEREQUESTREADER_H
#define REMOTE_REMOTEREQUESTREADER_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteRequestReader
    
    A stream reader which cracks a Remote request into its components.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "io/streamreader.h"

// forward declaration
namespace IO
{
    class XmlReader;
}
//------------------------------------------------------------------------------
namespace Remote
{
class RemoteRequestReader : public IO::StreamReader
{
    __DeclareClass(RemoteRequestReader);
public:
    /// constructor
    RemoteRequestReader();
    /// decode the request from the stream (call first before Get methods!)
    bool ReadRequest();
    /// return true if the stream contains a valid Remote request 
    bool IsValidRemoteRequest() const;
    /// get the xml reader for the remote request content
    const Ptr<IO::XmlReader>& GetXmlReader() const;
    /// get the name of the target controller
    const Util::String& GetControllerName() const;

private:
    bool isValidRemoteRequest;
    Ptr<IO::XmlReader> xmlReader;
    Util::String controllerName;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
RemoteRequestReader::IsValidRemoteRequest() const
{
    return this->isValidRemoteRequest;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<IO::XmlReader>& 
RemoteRequestReader::GetXmlReader() const
{
    return this->xmlReader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
RemoteRequestReader::GetControllerName() const
{
    return controllerName;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif
