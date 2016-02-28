#pragma once
#ifndef REMOTE_REMOTERESPONSEWRITER_H
#define REMOTE_REMOTERESPONSEWRITER_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteResponseWriter
  
    Stream writer which writes a correct Remote response to a stream.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "io/streamwriter.h"
#include "remote/remotestatus.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteResponseWriter : public IO::StreamWriter
{
    __DeclareClass(RemoteResponseWriter);
public:
    /// set status code
    void SetStatusCode(RemoteStatus::Code statusCode);
    /// set optional content stream (needs valid media type!)
    void SetContent(const Ptr<IO::Stream>& contentStream);
    /// write Remote response to the stream
    void WriteResponse();

private:
    RemoteStatus::Code statusCode;
    Ptr<IO::Stream> contentStream;
};

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteResponseWriter::SetStatusCode(RemoteStatus::Code c)
{
    this->statusCode = c;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteResponseWriter::SetContent(const Ptr<IO::Stream>& s)
{
    this->contentStream = s;
}

}
//------------------------------------------------------------------------------
#endif
