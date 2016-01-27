//------------------------------------------------------------------------------
//  baseitem.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "baseitem.h"
#include <QGridLayout>
#include "core/refcounted.h"

namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
BaseItem::BaseItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseItem::~BaseItem()
{
    // free item handler
    this->itemHandler = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseItem::Discard()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
BaseItem::OnActivated()
{
	// hides frame
	//this->frame->hide();

	// show frame again
	//this->frame->show();

	// raises frame
	//this->frame->raise();
}

//------------------------------------------------------------------------------
/**
*/
void
BaseItem::OnClicked()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseItem::OnRightClicked(const QPoint& pos)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
BaseItem::OnDeactivated()
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
BaseItem* 
BaseItem::Clone()
{
	BaseItem* item = new BaseItem;
	item->SetName(this->GetName());
	item->SetWidget(this->GetWidget());
	item->SetHandler(this->GetHandler());
	return item;
}
} // namespace Widgets