#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UIElementGroup
    
    An element group represents a group of elements. Can be used to apply the same operations to many elements.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "uielement.h"
namespace UI
{
class UiElementGroup : public Core::RefCounted
{
    __DeclareClass(UiElementGroup);
public:
	/// constructor
	UiElementGroup();
	/// destructor
	virtual ~UiElementGroup();

    /// adds element to group
    void AddElement(const Ptr<UiElement>& element);
    /// removes element
    void RemoveElement(const Ptr<UiElement>& element);

    /// hides all elements
    void Hide();
    /// shows all elements
    void Show();
    /// toggles elements visible
    void Toggle();

private:
    Util::Array<Ptr<UiElement>> elements;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
UiElementGroup::AddElement( const Ptr<UiElement>& element )
{
    n_assert(element.isvalid());
    this->elements.Append(element);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
UiElementGroup::RemoveElement( const Ptr<UiElement>& element )
{
    n_assert(element.isvalid());
    IndexT index = this->elements.FindIndex(element);
    n_assert(InvalidIndex != index);
    this->elements.EraseIndex(index);
}
} // namespace UI
//------------------------------------------------------------------------------