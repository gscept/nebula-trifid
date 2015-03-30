#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UiElement

    Wrapper class for platform specific UI layout classes.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
#include "ui/base/uielementbase.h"
namespace UI
{
class UiElement : public Base::UiElementBase
{
    __DeclareClass(UiElement);
};
}
#elif(__CEUI__&&__WIN32__)
#include "ui/ceui/ceuilayout.h"
namespace UI
{
class UiElement : public CEUI::CEUIElement
{
    __DeclareClass(UiElement);
};
}
#elif(__LIBROCKET__)
#include "ui/rocket/rocketelement.h"
namespace UI
{
class UiElement : public LibRocket::RocketElement
{
	__DeclareClass(UiElement);
};
}
#elif __WII__
#include "ui/wii/wiiuilayout.h"
namespace UI
{
class UiElement : public Wii::WiiUIElement
{
    __DeclareClass(UiElement);
};
}
#else
#error "UI::UIElement not implemented on this platform"
#endif