#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledGraphicsItem
	
	Implements a special type of item base class to use with the TiledGraphicsView.
	Inherits QGraphicsItem
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QObject>
#include <QGraphicsItemGroup>
namespace ResourceBrowser
{
class TiledGraphicsItem :
	public QObject,
	public QGraphicsItemGroup
{
	Q_OBJECT
public:
	/// constructor
	TiledGraphicsItem();
	/// destructor
	virtual ~TiledGraphicsItem();
	
	/// virtual setup function, implement in subclass
	virtual void Setup();
	/// virtual discard function, implement in subclass
	virtual void Discard();

	/// handle mouse entering item
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	/// handle mouse leaving item
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	/// set size of item
	void SetSize(const QSize& size);
protected:

	/// call rescale whenever the item resizes
	virtual void Rescale();

	QSize size;
	QGraphicsPolygonItem* background;
	QGraphicsPixmapItem* graphics;
	QGraphicsTextItem* label;
};


//------------------------------------------------------------------------------
/**
*/
inline void
TiledGraphicsItem::SetSize(const QSize& size)
{
	this->size = size;
	this->Rescale();
}

} // namespace ResourceBrowser