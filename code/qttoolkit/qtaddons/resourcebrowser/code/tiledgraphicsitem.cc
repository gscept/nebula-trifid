//------------------------------------------------------------------------------
//  tiledgraphicsitem.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledgraphicsitem.h"
#include <QBrush>
#include <QPen>
#include <QFont>

namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledGraphicsItem::TiledGraphicsItem() :
	background(NULL),
	graphics(NULL),
	label(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TiledGraphicsItem::~TiledGraphicsItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsItem::Setup()
{
	// setup background
	this->background = new QGraphicsPolygonItem;
	this->background->setZValue(-1);
	this->background->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	this->background->setBrush(QBrush(qRgb(75, 75, 75)));

	// rescale background
	this->Rescale();

	// setup graphics
	this->graphics = new QGraphicsPixmapItem;
	this->graphics->setZValue(0);
	this->label = new QGraphicsTextItem;
	this->label->setFont(QFont("Segoe UI", 8));
	this->label->setDefaultTextColor(Qt::white);
	this->label->setZValue(0);

	// disable hover events
	this->setAcceptHoverEvents(true);
	this->background->setAcceptHoverEvents(false);
	this->graphics->setAcceptHoverEvents(false);
	this->label->setAcceptHoverEvents(false);

	// add items to group
	this->addToGroup(this->background);
	this->addToGroup(this->graphics);
	this->addToGroup(this->label);	
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsItem::Discard()
{
	this->removeFromGroup(this->graphics);
	this->removeFromGroup(this->label);
	this->removeFromGroup(this->background);
	delete this->graphics;
	delete this->label;
	delete this->background;
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	this->background->setPen(QPen(QBrush(qRgb(240, 160, 0)), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	this->background->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsItem::Rescale()
{
	if (this->background) this->background->setPolygon(QPolygon(QRect(0, 0, this->size.width(), this->size.height())));
}

} // namespace ResourceBrowser