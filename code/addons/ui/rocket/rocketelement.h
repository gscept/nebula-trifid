#pragma once
//------------------------------------------------------------------------------
/**
    @class LibRocket::RocketElement
    
    Implements a rocket GUI element.
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "ui/base/uielementbase.h"
#include "util/variant.h"
#include "util/dictionary.h"
#include "Rocket/Core/Variant.h"

namespace Rocket
{
namespace Core
{
class Element;
}
}

namespace UI
{
	class UiElement;
	typedef Util::KeyValuePair<Util::String, Util::Variant> ElementAttributeEntry;
}

namespace LibRocket
{
class RocketElement : public Base::UiElementBase
{
	__DeclareClass(RocketElement);
public:
	/// constructor
	RocketElement();
	/// destructor
	virtual ~RocketElement();

    /// setup
    void Setup(const Ptr<UI::UiLayout>& layout);

    /// show element
    void Show();
    /// hide element
    void Hide();

	/// set text of element (assumes element is a text element)
	void SetText(const Util::String& text);
	/// get text of element
	Util::String GetText() const;
	/// appends text to current text
	void AppendText(const Util::String& text);

	/// adds an item to item list
	void AddListItem(const Util::String& item, void* userData);
	/// get list item
	void* GetListItem(const Util::String& item);
	/// removes list item
	void RemoveListItem(const Util::String& item);
	/// clears list
	void ClearList();

    /// set input value
    void SetInputValue(const Util::String& value);
    /// get input value
    Util::String GetInputValue();
	/// sets input text
	void SetInputText(const Util::String& text);
	/// gets input text
	Util::String GetInputText();
	/// sets checked status
	void SetChecked(bool b);
	/// gets checked status
	bool IsChecked();

	/// sets the scroll sizes for this element
	void SetScrollSizes(const Math::float2& sizes);
	/// sets the scroll positions for this element
	void SetScrollPositions(const Math::float2& positions);

	/// set image
	void SetImage(const Util::String& image);

	/// set progress
	void SetProgress(float progress);

	/// set if the element should be enabled
	void SetEnabled(bool b);

	/// set element to be selected
	void SetSelected(bool b);

	/// add item to combo box
	void AddComboBoxItem(const Util::String& item, const Util::String & key);
	/// return combo box item
	Util::String GetComboBoxItem(unsigned index);
	/// remove combo box item
	void RemoveComboBoxItem(unsigned index);
	/// clear combo box
	void ClearComboBox();
	/// get selected combo box item
	const unsigned GetSelectedComboBoxItem() const;
	/// set current selection in combo box
	void SetCurrentComboBoxItem(int index) const;
	/// get number of options in combo box
	const unsigned GetNumOptionsInComboBox() const;
	/// sets this element to be in focus

	void SetFocus(bool b);

	/// sets a property
	void SetProperty(const Util::String& p, const Util::String& value);
	/// gets property
	Util::String GetProperty(const Util::String& p) const;

	/// sets an attribute
	void SetAttribute(const Util::String& attr, const Util::String& value);
	/// removes an attribute
	void RemoveAttribute(const Util::String& attr);
	/// gets an attribute
	Util::String GetAttribute(const Util::String& attr);

	/// adds a class to an element
	void AddClass(const Util::String& c);
	/// removes a class from an element
	void RemoveClass(const Util::String& c);
    /// sets all classes on an element
    void SetClasses(const Util::String& c);
	/// get classes
	const Util::String GetClasses() const;

	/// convert between rocket and nebula variant
	static Rocket::Core::Variant N2RVariant(Util::Variant & var);
	
	/// create a new child element
	Ptr<UI::UiElement> AddElement(const Util::String & tag, const Util::String & inst, const Util::Array<UI::ElementAttributeEntry> & attributes);
	/// removed a child element
	void RemoveElement(const Ptr<UI::UiElement> & elem);

private:
    Rocket::Core::Element* element;
}; 
} // namespace LibRocket
//------------------------------------------------------------------------------