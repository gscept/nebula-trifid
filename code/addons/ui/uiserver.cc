//------------------------------------------------------------------------------
//  uiserver.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/uiserver.h"


namespace UI
{
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
__ImplementClass(UI::UiServer, 'UIS', Base::UiServerBase);
#elif (__CEUI__&&__WIN32__)
__ImplementClass(UI::UiServer, 'UIS', CEUI::CEUIServer);
#elif (__LIBROCKET__)
__ImplementClass(UI::UiServer, 'UIS', LibRocket::RocketServer);
#elif __WII__
__ImplementClass(UI::UiServer, 'UIS', Wii::WiiUIServer);
#else
#error "UI::UIServer not implemented on this platform"
#endif
__ImplementSingleton(UI::UiServer);

//------------------------------------------------------------------------------
/**
*/
UiServer::UiServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
UiServer::~UiServer()
{
    __DestructSingleton;
}

} // namespace UI