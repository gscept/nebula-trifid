//------------------------------------------------------------------------------
//  fontitem.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fontitem.h"
#include "contentbrowserapp.h"

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
FontItem::FontItem() 
{
	this->type = FontItemType;
}

//------------------------------------------------------------------------------
/**
*/
FontItem::~FontItem()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
void
FontItem::OnClicked()
{
	BaseItem::OnClicked();

}

//------------------------------------------------------------------------------
/**
*/
void 
FontItem::OnRightClicked(const QPoint& pos)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
FontItem::Clone()
{
	FontItem* item = new FontItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}

} // namespace Widgets