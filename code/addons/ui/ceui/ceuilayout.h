#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::CEUILayout

    Base class for SUI layouts.

    (C) 2009 Radon Labs GmbH
*/
#include "sui/base/suilayoutbase.h"
#include "cegui/include/CEGUIWindow.h"

//------------------------------------------------------------------------------
namespace CEUI
{
class CEUILayout : public Base::SUILayoutBase
{
    __DeclareClass(CEUILayout);
public:
    /// constructor
    CEUILayout();
    /// destructor
    virtual ~CEUILayout();

    /// discard the layout
    virtual void Discard();
     /// load layout from disc
    virtual void Load();
    /// show the layout 
    virtual void Show();
    /// hide the layout (may fade out)
    virtual void Hide();
	/// update the layout
	virtual void Update();
	/// register callback handlers for the GUI elements
	virtual void RegisterHandlers();
    
    /// invoke an action on the layout
    virtual void InvokeAction(const Ptr<SUI::SUIAction>& action);
    /// process input events, and append to outEvents array, returns number of generated events
    SizeT ProcessInputEvents(const Util::Array<Input::InputEvent>& inputEvents, Util::Array<SUI::SUIEvent>& outEvents);

	CEGUI::Window * GetLayout();
    
	bool HandleEvent(const CEGUI::KeyEventArgs&);
	bool HandleEvent(const CEGUI::MouseEventArgs&);
	bool HandleEvent(const CEGUI::WindowEventArgs&);
	bool HandleMouseEnterEvent(const CEGUI::MouseEventArgs&);
	bool HandleMouseLeaveEvent(const CEGUI::MouseEventArgs&);
protected:
	

    CEGUI::Window* ceguiLayout;
	Util::Array<SUI::SUIEvent>* batchEvents;	
};

inline
CEGUI::Window * 
CEUILayout::GetLayout()
{
	return this->ceguiLayout;
}
} // namespace Base
//------------------------------------------------------------------------------

