//------------------------------------------------------------------------------
//  rocketelement.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "rocketelement.h"
#include "rocketlayout.h"
#include "ui/uilayout.h"
#include <Rocket/Core.h>
#include <Rocket/Controls/ElementFormControl.h>
#include <Rocket/Controls/ElementFormControlSelect.h>


namespace LibRocket
{
__ImplementClass(LibRocket::RocketElement, 'ROEL', Base::UiElementBase);

//------------------------------------------------------------------------------
/**
*/
RocketElement::RocketElement() :
    element(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RocketElement::~RocketElement()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
RocketElement::Setup(const Ptr<UI::UiLayout>& layout)
{
	n_assert(0 == this->element);
	Base::UiElementBase::Setup(layout);
    Ptr<RocketLayout> rocketLayout = layout.downcast<RocketLayout>();
    this->element = rocketLayout->GetLayout()->GetElementById(this->elementId.AsString().AsCharPtr());	

	// set the visibility, gets the visibility from rocket
	this->visible = this->element->GetProperty("visibility")->ToString() == "hidden" ? false : true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::Show()
{
    UiElementBase::Show();
    this->element->SetProperty("visibility", "visible");	    
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::Hide()
{
    UiElementBase::Hide();
    this->element->SetProperty("visibility", "hidden");	    
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetText( const Util::String& text )
{
	n_assert(0 != this->element);
	element->SetInnerRML(text.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
Util::String
RocketElement::GetText() const
{
	n_assert(0 != this->element);

	// cast to text element
	Util::String text = element->GetInnerRML().CString();
	return text;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::AppendText( const Util::String& text )
{
	n_assert(0 != this->element);

	// get current text
	Util::String current = element->GetInnerRML().CString();
	element->SetInnerRML((current + text).AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::AddListItem( const Util::String& item, void* userData )
{
	n_assert(0 != this->element);
}

//------------------------------------------------------------------------------
/**
*/
void* 
RocketElement::GetListItem( const Util::String& item )
{
	n_assert(0 != this->element);
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::RemoveListItem( const Util::String& item )
{
	n_assert(0 != this->element);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::ClearList()
{
	n_assert(0 != this->element);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetInputValue( const Util::String& value )
{
    n_assert(0 != this->element);
    if (this->element->GetTagName() == "input")
    {
        Rocket::Controls::ElementFormControl* el = (Rocket::Controls::ElementFormControl*)this->element;
        el->SetValue(value.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::String
RocketElement::GetInputValue()
{
    n_assert(0 != this->element);
    if (this->element->GetTagName() == "input")
    {
        Rocket::Controls::ElementFormControl* el = (Rocket::Controls::ElementFormControl*)this->element;
        return el->GetValue().CString();
    }
    else
    {
        return "";
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetInputText( const Util::String& text )
{
    n_assert(0 != this->element);
    if (element->GetTagName() == "input")
    {
        Rocket::Controls::ElementFormControl* el = dynamic_cast<Rocket::Controls::ElementFormControl*>(this->element);
        n_assert(0 != el);
        el->SetValue(text.AsCharPtr());
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
RocketElement::GetInputText()
{
	n_assert(0 != this->element);
	if (element->GetTagName() == "input")
	{
		Rocket::Controls::ElementFormControl* el = dynamic_cast<Rocket::Controls::ElementFormControl*>(this->element);
		n_assert(0 != el);
		Util::String ret = el->GetValue().CString();
		return ret;
	}
	else
	{
		return "";
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetChecked( bool b )
{
	n_assert(0 != this->element);
	Rocket::Core::String type = this->element->GetAttribute<Rocket::Core::String>("type", "");
	if (type == "checkbox" || type == "radio")
	{
		if (b)
		{			
			this->element->SetAttribute<bool>("checked", b);
		}
		else
		{
			this->element->RemoveAttribute("checked");
		}
		
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
RocketElement::IsChecked()
{
	n_assert(0 != this->element);
	Rocket::Core::String type = this->element->GetAttribute<Rocket::Core::String>("type", "");
	if (type == "checkbox" || type == "radio")
	{
		if (this->element->HasAttribute("checked"))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetScrollSizes( const Math::float2& sizes )
{
	n_assert(0 != this->element);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetScrollPositions( const Math::float2& positions )
{
	n_assert(0 != this->element);
	this->element->SetScrollLeft(positions.x());
	this->element->SetScrollTop(positions.y());
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetImage( const Util::String& image )
{
	n_assert(0 != this->element);
	element->SetInnerRML(image.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetProgress( float progress )
{
	n_assert(0 != this->element);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetEnabled( bool b )
{
	n_assert(0 != this->element);
	if (b)
	{
		if (this->element->HasAttribute("disabled"))
		{
			this->element->RemoveAttribute("disabled");
		}
	}
	else
	{
		this->element->SetAttribute("disabled", true);
	}    
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetSelected( bool b )
{
	n_assert(0 != this->element);
    this->element->SetAttribute("selected", b);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::AddComboBoxItem( const Util::String& item, const Util::String & key )
{
	n_assert(0 != this->element);
    Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
    n_assert(0 != select);
	select->Add(item.AsCharPtr(), key.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
RocketElement::GetComboBoxItem( unsigned index )
{
	n_assert(0 != this->element);
    Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
    n_assert(0 != select);
    return select->GetOption(index)->GetValue().CString();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::RemoveComboBoxItem( unsigned index )
{
	n_assert(0 != this->element);
    Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
    n_assert(0 != select);
    select->Remove(index);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::ClearComboBox()
{
	n_assert(0 != this->element);
    Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
    n_assert(0 != select);
    select->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
const unsigned
RocketElement::GetSelectedComboBoxItem()const
{
	n_assert(0 != this->element);
	Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
	n_assert(0 != select);
	return select->GetSelection();
}

//------------------------------------------------------------------------------
/**
*/
void
RocketElement::SetCurrentComboBoxItem(int index)const
{
	n_assert(0 != this->element);
	Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
	n_assert(0 != select);
	return select->SetSelection(index);
}

//------------------------------------------------------------------------------
/**
*/
const unsigned
RocketElement::GetNumOptionsInComboBox()const
{
	n_assert(0 != this->element);
	Rocket::Controls::ElementFormControlSelect* select = dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(this->element);
	n_assert(0 != select);
	return select->GetNumOptions();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetFocus( bool b )
{
	n_assert(0 != this->element);

	if (b)	this->element->Focus();
	else	this->element->Blur();
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetProperty( const Util::String& p, const Util::String& value )
{
	n_assert(0 != this->element);
    this->element->SetProperty(p.AsCharPtr(), value.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
RocketElement::GetProperty( const Util::String& p ) const
{
	n_assert(0 != this->element);
	Util::String ret = this->element->GetProperty<Rocket::Core::String>(p.AsCharPtr()).CString();
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetAttribute( const Util::String& attr, const Util::String& value )
{
	n_assert(0 != this->element);
	this->element->SetAttribute(attr.AsCharPtr(), value.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
	RocketElement::RemoveAttribute( const Util::String& attr)
{
	n_assert(0 != this->element);
	this->element->RemoveAttribute(attr.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
RocketElement::GetAttribute( const Util::String& attr )
{
	n_assert(0 != this->element);
	Util::String ret = this->element->GetAttribute<Rocket::Core::String>(attr.AsCharPtr(), "").CString();
	return ret;
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::AddClass( const Util::String& c )
{
	n_assert(0 != this->element);
	this->element->SetClass(c.AsCharPtr(), true);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::RemoveClass( const Util::String& c )
{
	n_assert(0 != this->element);
	this->element->SetClass(c.AsCharPtr(), false);
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketElement::SetClasses( const Util::String& c )
{
    n_assert(0 != this->element);
    this->element->SetClassNames(c.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
const Util::String 
RocketElement::GetClasses() const
{
	n_assert(0 != this->element);
	return this->element->GetClassNames().CString();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<UI::UiElement> 
RocketElement::AddElement(const Util::String & tag, const Util::String & inst, const Util::Array<UI::ElementAttributeEntry> & attributes)
{
	Ptr<UI::UiElement> elem = UI::UiElement::Create();

	Rocket::Core::XMLAttributes attrs;
	
	for (int i = 0; i < attributes.Size(); i++)
	{
		Util::Variant val = attributes[i].Value();
		attrs.Set(attributes[i].Key().AsCharPtr(), RocketElement::N2RVariant(val));
	}

	Rocket::Core::Element* newelem = Rocket::Core::Factory::InstanceElement(this->element, tag.AsCharPtr(), inst.AsCharPtr(), attrs);
	n_assert(newelem);
	elem->element = newelem;
	this->element->AppendChild(newelem);
	return elem;
}

//------------------------------------------------------------------------------
/**
*/
void
RocketElement::RemoveElement(const Ptr<UI::UiElement> & elem)
{
	Ptr<LibRocket::RocketElement> relem = elem.cast<LibRocket::RocketElement>();
	this->element->RemoveChild(relem->element);
	relem->element->RemoveReference();
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::Variant
RocketElement::N2RVariant(Util::Variant & var)
{
	Rocket::Core::Variant rvar;
	switch (var.GetType())
	{
	case Util::Variant::Int:
		rvar.Set(var.GetInt());
		break;
	case Util::Variant::Float:
		rvar.Set(var.GetFloat());
		break;
	case Util::Variant::String:
		rvar.Set(var.GetString().AsCharPtr());
		break;
	case Util::Variant::Float2:
	{
		Math::float2 f = var.GetFloat2();
		Rocket::Core::Vector2f f2(f.x(), f.y());
		rvar.Set(f2);
	}
		break;
	case Util::Variant::Float4:
	{
		Math::float4 f = var.GetFloat4();
		Rocket::Core::Colourf c(f.x(), f.y(),f.z(),f.w());
		rvar.Set(c);
	}
	break;
	default:
		n_error("unhandled variant type");
	}
	return rvar;
}
} // namespace LibRocket