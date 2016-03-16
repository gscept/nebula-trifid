#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::UiLayoutBase

    Base class for UI layouts.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "ui/uievent.h"
#include "resources/resourceid.h"
#include "resources/resource.h"
#include "input/inputevent.h"
#include "timing/time.h"
#include "frame/framebatch.h"
#include "threading/objectref.h"
#include "ui/uielement.h"

//------------------------------------------------------------------------------
namespace Base
{
class UiLayoutBase : public Core::RefCounted
{
    __DeclareClass(UiLayoutBase);
public:
    /// constructor
    UiLayoutBase();
    /// destructor
    virtual ~UiLayoutBase();

    /// setup the layout
    void Setup(const Util::StringAtom& layoutId, const Resources::ResourceId& resId);
    /// discard the layout
    void Discard();
    /// return true if the layout has been setup
    bool IsValid() const;

    /// return string
    const Util::String& GetScript() const;

    /// get the layout's id
    const Util::StringAtom& GetLayoutId() const;
    /// get the layout's resource id
    const Resources::ResourceId& GetResourceId() const;

    /// get element by id
    Ptr<UI::UiElement> GetElement(const Util::StringAtom& elementId);

    /// load layout from disc
    void Load();
    /// return true if load is still pending (call Load() again if not)
    bool IsPending() const;
    /// return true if loading has failed
    bool LoadFailed() const;
	/// reloads layout
	void Reload();

    /// show the layout 
    virtual void Show();
    /// hide the layout (may fade out)
    virtual void Hide();
    /// toggles the layout visible/hidden
    void Toggle();
    /// return true if currently shown
    bool IsShown() const;	
    /// return true if the layout is ready to be displayed
    bool IsShowable() const;
    
    /// process input events, and append to outEvents array, returns number of generated events
    SizeT ProcessInputEvents(const Util::Array<Input::InputEvent>& inputEvents);
    /// handle input event
    void HandleInput(const Input::InputEvent& event);
    /// update the layout (animation, etc...)
    void Update();
    /// render the layout
    void Render(const Ptr<Frame::FrameBatch>& frameBatch);

protected:
	Util::Dictionary<Util::StringAtom, Ptr<UI::UiElement>> elements;
    Resources::Resource::State resState;
    Util::String script;
    Util::StringAtom layoutId;
    Resources::ResourceId resourceId;
    Timing::Time showTimeStamp;
    Timing::Time hideTimeStamp;
    bool shown;
    bool isValid;	
};

//------------------------------------------------------------------------------
/**
*/
inline bool
UiLayoutBase::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
UiLayoutBase::GetScript() const
{
    return this->script;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
UiLayoutBase::GetLayoutId() const
{
    return this->layoutId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
UiLayoutBase::GetResourceId() const
{
    return this->resourceId;
}

} // namespace Base
//------------------------------------------------------------------------------

