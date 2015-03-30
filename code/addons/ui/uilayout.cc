//------------------------------------------------------------------------------
//  uilayout.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/uilayout.h"

namespace UI
{
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
__ImplementClass(UI::UiLayout, 'UIL', Base::UiLayoutBase);
#elif(__CEUI__&&__WIN32__)
__ImplementClass(UI::UiLayout, 'UIL', CEUI::CEUILayout);
#elif(__LIBROCKET__)
__ImplementClass(UI::UiLayout, 'UIL', LibRocket::RocketLayout);
#elif __WII__
__ImplementClass(UI::UiLayout, 'UIL', Wii::WiiUILayout);
#else
#error "UI::UILayout not implemented on this platform"
#endif
}