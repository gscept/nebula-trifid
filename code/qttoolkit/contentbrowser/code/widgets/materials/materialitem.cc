//------------------------------------------------------------------------------
//  materialitem.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "materialitem.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
MaterialItem::MaterialItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MaterialItem::~MaterialItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseItem*
MaterialItem::Clone()
{
	MaterialItem* item = new MaterialItem;
	item->SetName(this->name);
	item->SetWidget(this->frame);
	item->SetHandler(this->itemHandler);
	item->SetUi(this->ui);
	return item;
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialItem::OnActivated()
{

}

//------------------------------------------------------------------------------
/**
*/
void
MaterialItem::OnClicked()
{

}

//------------------------------------------------------------------------------
/**
*/
void
MaterialItem::OnRightClicked(const QPoint& pos)
{

}

} // namespace Widgets