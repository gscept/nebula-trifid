#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UIPlugin
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace UI
{
class UiPlugin : public Core::RefCounted
{
    __DeclareClass(UiPlugin);
public:
    /// constructor
    UiPlugin();
    /// destructor
    virtual ~UiPlugin();

    /// called when plugin is registered
    virtual void OnRegister();
    /// called when plugin is unregistered
    virtual void OnUnregister();
};

} // namespace UI