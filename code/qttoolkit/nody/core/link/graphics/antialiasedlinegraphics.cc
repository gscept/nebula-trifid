//------------------------------------------------------------------------------
//  antialiasedlinegraphics.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "antialiasedlinegraphics.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Nody
{

//------------------------------------------------------------------------------
/**
*/
AntialiasedLineGraphics::AntialiasedLineGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AntialiasedLineGraphics::~AntialiasedLineGraphics()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AntialiasedLineGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setRenderHint(QPainter::Antialiasing);
	QGraphicsPathItem::paint(painter, option, widget);
	painter->setRenderHint(QPainter::Antialiasing, false);
}

} // namespace Nody