#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UIPlugin
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "timing/time.h"
#include "frame/framebatch.h"

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
	///
	virtual void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);
};

} // namespace UI