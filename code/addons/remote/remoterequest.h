#pragma once
#ifndef REMOTE_REMOTEREQUEST_H
#define REMOTE_REMOTEREQUEST_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteRequest
    
    Encapsulates a complete Remote request into a message.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "messaging/message.h"
#include "remote/remotestatus.h"
#include "io/xmlreader.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteRequest : public Messaging::Message
{
    __DeclareClass(RemoteRequest);
    __DeclareMsgId;
public:
    /// constructor
    RemoteRequest();
    /// destructor
    virtual ~RemoteRequest();

    /// set the xml reader
    void SetXmlReader(const Ptr<IO::XmlReader>& xmlReader);
    /// get the xml reader
    const Ptr<IO::XmlReader>& GetXmlReader() const;
    /// set the response content stream
    void SetResponseContentStream(const Ptr<IO::Stream>& responseContentStream);
    /// get the response content stream
    const Ptr<IO::Stream>& GetResponseContentStream() const;
    /// set the remote status (set by RemoteRequestHandler)
    void SetStatus(RemoteStatus::Code status);
    /// get the remote status 
    RemoteStatus::Code GetStatus() const;

private:
    Ptr<IO::Stream> responseContentStream;
    Ptr<IO::XmlReader> xmlReader;
    RemoteStatus::Code status;
};

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequest::SetXmlReader(const Ptr<IO::XmlReader>& xmlReader)
{
    this->xmlReader = xmlReader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<IO::XmlReader>&
RemoteRequest::GetXmlReader() const
{
    return this->xmlReader;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequest::SetResponseContentStream(const Ptr<IO::Stream>& s)
{
    this->responseContentStream = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<IO::Stream>&
RemoteRequest::GetResponseContentStream() const
{
    return this->responseContentStream;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequest::SetStatus(RemoteStatus::Code s)
{
    this->status = s;
}

//------------------------------------------------------------------------------
/**
*/
inline RemoteStatus::Code
RemoteRequest::GetStatus() const
{
    return this->status;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif
    