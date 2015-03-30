#pragma once
//------------------------------------------------------------------------------
/**
    @class Win32::Win32VideoServer
    
    Win32-specific VideoServer.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "video/base/videoserverbase.h"

//------------------------------------------------------------------------------
namespace Win32
{
class Win32VideoServer : public Base::VideoServerBase
{
    __DeclareClass(Win32VideoServer);
    __DeclareSingleton(Win32VideoServer);
public:
    /// constructor
    Win32VideoServer();
    /// destructor
    virtual ~Win32VideoServer();

    /// open the video server
    virtual void Open();
    /// close the video server
    virtual void Close();
    /// call on frame
    virtual void OnFrame(Timing::Time time);

protected:

};

} // namespace Win32
//------------------------------------------------------------------------------