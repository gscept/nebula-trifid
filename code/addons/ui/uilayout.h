#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UILayout

    Wrapper class for platform specific UI layout classes.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if (!__CEUI__&&!__LIBROCKET__&&(__WIN32__||__XBOX360__))
#include "ui/base/uilayoutbase.h"
namespace UI
{
class UiLayout : public Base::UiLayoutBase
{
    __DeclareClass(UiLayout);
};
}
#elif(__CEUI__&&__WIN32__)
#include "ui/ceui/ceuilayout.h"
namespace UI
{
class UiLayout : public CEUI::CEUILayout
{
    __DeclareClass(UiLayout);
};
}
#elif(__LIBROCKET__)
#include "ui/rocket/rocketlayout.h"
namespace UI
{
class UiLayout : public LibRocket::RocketLayout
{
	__DeclareClass(UiLayout);
};
}
#elif __WII__
#include "ui/wii/wiiuilayout.h"
namespace UI
{
class UiLayout : public Wii::WiiUILayout
{
    __DeclareClass(UiLayout);
};
}
#else
#error "UI::UILayout not implemented on this platform"
#endif