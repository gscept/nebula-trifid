#pragma once
#ifndef REMOTE_REMOTESTATUS_H
#define REMOTE_REMOTESTATUS_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteStatus
  
    HTTP status code enumeration (e.g. 404 Not Found).
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteStatus
{
public:
    /// status codes
    //FIXME: makes no sense for TCP/IP. Do we need a RemoteStatus class at all?
    enum Code
    {
        Continue                        = 100,
        OK                              = 200,
        BadRequest                      = 400,
        Forbidden                       = 403,
        NotFound                        = 404,
        MethodNotAllowed                = 405,
        NotAcceptable                   = 406,
        InternalServerError             = 500,
        NotImplemented                  = 501,
        ServiceUnavailable              = 503,

        InvalidRemoteStatus = 0xffff
    };

    /// convert from string
    static Code FromString(const Util::String& str);
    /// convert to string
    static Util::String ToString(Code c);
    /// convert code to human readable string
    static Util::String ToHumanReadableString(Code c);
};

} // namespace Remote
//------------------------------------------------------------------------------
#endif
