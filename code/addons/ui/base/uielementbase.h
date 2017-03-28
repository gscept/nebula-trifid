#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::UiElementBase
    
    An element is a fragment of a layout. 
    An element supports all the functionality for which we apply text, get text, images etc.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "util/variant.h"

namespace UI
{
	class UiLayout;

	class UiElement;
	typedef Util::KeyValuePair<Util::String, Util::Variant> ElementAttributeEntry;
}


namespace Base
{

class UiElementBase : public Core::RefCounted
{
	__DeclareClass(UiElementBase);
public:
	/// constructor
	UiElementBase();
	/// destructor
	virtual ~UiElementBase();

    /// set element id
    void SetElementId(const Util::StringAtom& elementId);
    /// get element id
    const Util::StringAtom& GetElementId() const;

    /// setup element from layout
    virtual void Setup(const Ptr<UI::UiLayout>& layout);
	/// discard element
	virtual void Discard();

    /// get if element is visible
    virtual const bool GetVisible() const;
    /// show element
    virtual void Show();
    /// hide element
    virtual void Hide();
	/// toggle visibility
	void Toggle();
    
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

    /// sets this element to be in focus
    void SetFocus(bool b);

    /// sets a property
    void SetProperty(const Util::String& p, const Util::String& value);
    /// gets property
    Util::String GetProperty(const Util::String& p) const;

	/// sets an attribute
	void SetAttribute(const Util::String& attr, const Util::String& value);
	/// gets an attribute
	Util::String GetAttribute(const Util::String& attr);

    /// adds a class to an element
    void AddClass(const Util::String& c);
    /// removes a class from an element
    void RemoveClass(const Util::String& c);
    /// sets all classes on an element
    void SetClasses(const Util::String& c);
	/// returns all classes
	const Util::String GetClasses() const;

	/// create a new child element
	Ptr<UI::UiElement> AddElement(const Util::String & tag, const Util::String & inst, const Util::Array<UI::ElementAttributeEntry> & attributes);
	/// removed a child element
	void RemoveElement(const Ptr<UI::UiElement> & elem);

protected:
	Ptr<UI::UiLayout> layout;
    Util::StringAtom elementId;
    bool visible;
    bool enabled;
    bool selected;
    bool focus;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
UiElementBase::SetElementId( const Util::StringAtom& elementId )
{
    this->elementId = elementId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom& 
UiElementBase::GetElementId() const
{
    return this->elementId;
}
} // namespace Base
//------------------------------------------------------------------------------