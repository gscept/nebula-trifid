//------------------------------------------------------------------------------
//  layoutitem.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "layoutitem.h"
#include "contentbrowserapp.h"

using namespace Util;
namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
LayoutItem::LayoutItem()
{
	this->type = LayoutItemType;
}

//------------------------------------------------------------------------------
/**
*/
LayoutItem::~LayoutItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
LayoutItem::OnActivated()
{
	BaseItem::OnActivated();
	// cast base handler to model handler
	Ptr<UIHandler> handler = this->itemHandler.downcast<UIHandler>();

	// set ui of item handler
	handler->SetUI(this->ui);

	handler->SetName(this->GetName().toLatin1().constData());
	// tell handler to load ui
	handler->LoadUIInfo();

	// setup item handler
	handler->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void
LayoutItem::OnClicked()
{
	BaseItem::OnClicked();

}

//------------------------------------------------------------------------------
/**
*/
void 
LayoutItem::OnRightClicked(const QPoint& pos)
{
	// emtpy
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
LayoutItem::Clone()
{
	LayoutItem* item = new LayoutItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}

} // namespace Widgets