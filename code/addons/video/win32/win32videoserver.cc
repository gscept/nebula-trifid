//------------------------------------------------------------------------------
//  win32inputserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/win32/win32videoserver.h"

namespace Win32
{
__ImplementClass(Win32::Win32VideoServer, 'W3VS', Base::VideoServerBase);
__ImplementSingleton(Win32::Win32VideoServer);

//------------------------------------------------------------------------------
/**
*/
Win32VideoServer::Win32VideoServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Win32VideoServer::~Win32VideoServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**    
*/
void
Win32VideoServer::Open()
{
    n_assert(!this->IsOpen());
    VideoServerBase::Open();
}

//------------------------------------------------------------------------------
/**    
*/
void
Win32VideoServer::Close()
{
    n_assert(this->IsOpen());

    // call parent class
    VideoServerBase::Close();
}

//------------------------------------------------------------------------------
/**    
*/
void
Win32VideoServer::OnFrame(Timing::Time time)
{
    VideoServerBase::OnFrame(time);
}
} // namespace Win32