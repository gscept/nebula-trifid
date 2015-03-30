//------------------------------------------------------------------------------
//  trackmodel.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "trackmodel.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
TrackModel::TrackModel()
{
	QStandardItem* item;
	item = new QStandardItem("Clip"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(0, item);
	item = new QStandardItem("Weight"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(1, item);
}

//------------------------------------------------------------------------------
/**
*/
TrackModel::~TrackModel()
{
	// empty
}

} // namespace Widgets