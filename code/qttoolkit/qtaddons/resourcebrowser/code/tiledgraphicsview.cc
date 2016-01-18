//------------------------------------------------------------------------------
//  tiledgraphicsview.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tiledgraphicsview.h"
#include "tiledgraphicsitem.h"
#include <QScrollBar>

#define VIEW_PADDING 16
#define ITEM_SPACING 12
namespace ResourceBrowser
{

//------------------------------------------------------------------------------
/**
*/
TiledGraphicsView::TiledGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
	this->scene = new QGraphicsScene;
	this->setScene(this->scene);
	this->itemSize = QSize(104, 140);
}

//------------------------------------------------------------------------------
/**
*/
TiledGraphicsView::~TiledGraphicsView()
{
	this->setScene(NULL);
	delete this->scene;
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsView::AddTiledItem(TiledGraphicsItem* item)
{
	n_assert(!this->items.contains(item));

	// set item size
	item->SetSize(this->itemSize);
	item->Setup();

	// setup item
	this->items.append(item);
	this->scene->addItem(item);
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsView::RemoveTiledItem(TiledGraphicsItem* item)
{
	n_assert(this->items.contains(item));

	item->Discard();
	this->items.removeOne(item);
	this->scene->removeItem(item);

	this->Rearrange();
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsView::Rearrange()
{
	// get visible size of view
	QSize viewSize = this->size();
	SizeT numX = viewSize.width() / (this->itemSize.width() + VIEW_PADDING);

	// make sure numX is not 0
	numX = Math::n_max(numX, 1);

	QRectF itemsRect;
	SizeT x, y;
    IndexT itemIndex = 0;
	IndexT i;
	for (i = 0; i < this->items.size(); i++)
	{
        TiledGraphicsItem* item = this->items[i];
        if (item->isVisible())
        {
            IndexT xIndex = itemIndex % numX;
            IndexT yIndex = itemIndex / numX;
			x = xIndex * this->itemSize.width() + ITEM_SPACING + xIndex * ITEM_SPACING;
			y = yIndex * this->itemSize.height() + ITEM_SPACING + yIndex * ITEM_SPACING;

            item->setPos(x, y);
            itemIndex++;
			itemsRect = itemsRect.united(item->sceneBoundingRect());
        }
	}

	// resize scene to fit all items
	QRectF rect = itemsRect;
	qreal width = rect.width() + VIEW_PADDING * 2;
	qreal height = rect.height() + VIEW_PADDING * 2;
	rect.setTop(0);
	rect.setLeft(0);
	rect.setWidth(width);
	rect.setHeight(height);
	this->setSceneRect(rect);
}

//------------------------------------------------------------------------------
/**
	Basically does RemoveTiledItem, but without the costly Rearrange for every item
*/
void
TiledGraphicsView::Clear()
{
	IndexT i;
	for (i = 0; i < this->items.size(); i++)
	{
		this->items[i]->Discard();
	}
	this->items.clear();
	this->scene->clear();
}

//------------------------------------------------------------------------------
/**
*/
void
TiledGraphicsView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);
	this->scene->setSceneRect(this->rect());
	this->Rearrange();
}

} // namespace ResourceBrowser