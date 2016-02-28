#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UiServer

    The central server object of the UI system, running in the render-thread.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
#include "ui/base/uiserverbase.h"
namespace UI
{
class UiServer : public Base::UiServerBase
{
    __DeclareClass(UiServer);
    __DeclareSingleton(UiServer);
public:
    /// constructor
    UiServer();
    /// destructor
    virtual ~UiServer();    
};
}
#elif(__CEUI__&&__WIN32__)
#include "ui/ceui/ceuiserver.h"
namespace UI
{
class UiServer : public CEUI::CEUIServer
{
    __DeclareClass(UiServer);
    __DeclareSingleton(UiServer);
public:
    /// constructor
    UiServer();
    /// destructor
    virtual ~UiServer();    
};
}
#elif(__LIBROCKET__)
#include "ui/rocket/rocketserver.h"
namespace UI
{
class UiServer : public LibRocket::RocketServer
{
	__DeclareClass(UiServer);
	__DeclareSingleton(UiServer);
public:
	/// constructor
	UiServer();
	/// destructor
	virtual ~UiServer();    
};
}
#elif __WII__
#include "ui/wii/wiiuiserver.h"
namespace UI
{
class UiServer : public Wii::WiiUIServer
{
    __DeclareClass(UiServer);
    __DeclareSingleton(UiServer);
public:
    /// constructor
    UiServer();
    /// destructor
    virtual ~UiServer();    
};
}
#else
#error "UI::UIServer not implemented on this platform"
#endif