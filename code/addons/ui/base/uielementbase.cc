//------------------------------------------------------------------------------
//  uielementbase.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/base/uielementbase.h"
#include "ui/uilayout.h"

namespace Base
{
__ImplementClass(Base::UiElementBase, 'SUEL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
UiElementBase::UiElementBase() :
    visible(true),
	enabled(true),
	selected(false),
	focus(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
UiElementBase::~UiElementBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::Setup( const Ptr<UI::UiLayout>& layout )
{
    // override in subclass
	this->layout = layout;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::Discard()
{
	this->layout = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::Show()
{
    this->visible = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::Hide()
{
    this->visible = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::Toggle()
{
    if (this->visible)
    {
        this->Hide();
    }
    else
    {
        this->Show();
    }
}

//------------------------------------------------------------------------------
/**
    Override in subclass, but do not forget to call this first
*/
const bool 
UiElementBase::GetVisible() const
{
    return this->visible;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetText( const Util::String& text )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
UiElementBase::GetText() const
{
    // empty, override in subclass
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::AppendText( const Util::String& text )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::AddListItem( const Util::String& item, void* userData )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void* 
UiElementBase::GetListItem( const Util::String& item )
{
    // empty, override in subclass
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::RemoveListItem( const Util::String& item )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::ClearList()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetInputValue( const Util::String& value )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
UiElementBase::GetInputValue()
{
    // empty, override in subclass
    return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetInputText( const Util::String& text )
{
	// empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
UiElementBase::GetInputText()
{
	// empty, override in subclass
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetChecked( bool b )
{
	// empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool 
UiElementBase::IsChecked()
{
	// empty, override in subclass
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetScrollSizes( const Math::float2& sizes )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetScrollPositions( const Math::float2& positions )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetImage( const Util::String& image )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetProgress( float progress )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
    Override in subclass but do not forget to call this first
*/
void 
UiElementBase::SetEnabled( bool b )
{
    this->enabled = b;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetSelected( bool b )
{
	this->selected = b;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::AddComboBoxItem( const Util::String& item, const Util::String & key )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
UiElementBase::GetComboBoxItem( unsigned index )
{
	// override in subclass
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::RemoveComboBoxItem( unsigned index )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::ClearComboBox()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetFocus( bool b )
{
	this->focus = b;
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::SetProperty( const Util::String& p, const Util::String& value )
{
	// override in subclass
}


//------------------------------------------------------------------------------
/**
*/
Util::String 
UiElementBase::GetProperty( const Util::String& key ) const
{
	// override in subclass
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::AddClass( const Util::String& c )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementBase::RemoveClass( const Util::String& c )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
const Util::String 
UiElementBase::GetClasses() const
{
	// override in subclass
	return "";
}

//------------------------------------------------------------------------------
/**
*/
Ptr<UI::UiElement>
UiElementBase::AddElement(const Util::String & tag, const Util::String & inst, const Util::Array<UI::ElementAttributeEntry> & attributes)
{
	// override in subclass
	return Ptr<UI::UiElement>();
}

//------------------------------------------------------------------------------
/**
*/
void
UiElementBase::RemoveElement(const Ptr<UI::UiElement> & elem)
{
	// override in subclass
}

} // namespace Base