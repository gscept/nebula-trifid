#include "clipmodel.h"

namespace Importer
{

//------------------------------------------------------------------------------
/**
*/
ClipModel::ClipModel( void )
{
	QStandardItem* item;
	item = new QStandardItem("Name"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(0, item);
	item = new QStandardItem("Start time"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(1, item);
	item = new QStandardItem("Stop time"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(2, item);
	item = new QStandardItem("Pre infinity"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(3, item);
	item = new QStandardItem("Post infinity"); item->setTextAlignment(Qt::AlignCenter);
	this->setHorizontalHeaderItem(4, item);
}

//------------------------------------------------------------------------------
/**
*/
ClipModel::~ClipModel( void )
{

}
}