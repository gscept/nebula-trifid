//------------------------------------------------------------------------------
//  uielement.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/uielement.h"

namespace UI
{
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
__ImplementClass(UI::UiElement, 'UIE', Base::UiElementBase);
#elif(__CEUI__&&__WIN32__)
__ImplementClass(UI::UiElement, 'UIE', CEUI::CEUIElement);
#elif(__LIBROCKET__)
__ImplementClass(UI::UiElement, 'UIE', LibRocket::RocketElement);
#elif __WII__
__ImplementClass(UI::UiElement, 'UIE', Wii::WiiUIElement);
#else
#error "UI::UILayout not implemented on this platform"
#endif
}