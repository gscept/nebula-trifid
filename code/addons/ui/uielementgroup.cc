//------------------------------------------------------------------------------
//  uielementgroup.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uielementgroup.h"

namespace UI
{
__ImplementClass(UI::UiElementGroup, 'UELG', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
UiElementGroup::UiElementGroup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
UiElementGroup::~UiElementGroup()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementGroup::Hide()
{
    IndexT i;
    for (i = 0; i < this->elements.Size(); i++)
    {
        this->elements[i]->Hide();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementGroup::Show()
{
    IndexT i;
    for (i = 0; i < this->elements.Size(); i++)
    {
        this->elements[i]->Show();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
UiElementGroup::Toggle()
{
    IndexT i;
    for (i = 0; i < this->elements.Size(); i++)
    {
        this->elements[i]->Toggle();
    }
}
} // namespace UI