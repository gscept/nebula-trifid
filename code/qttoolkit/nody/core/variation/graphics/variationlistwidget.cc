//------------------------------------------------------------------------------
//  variationlistwidget.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variationlistwidget.h"
#include <QPainter>
#include <QMenu>
#include <QAction>

namespace Nody
{

//------------------------------------------------------------------------------
/**
*/
VariationListWidget::VariationListWidget(QWidget* parent) :
	QTreeWidget(parent),
	isDragging(false),
	dragItem(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VariationListWidget::~VariationListWidget()
{
	// empty
}

#if __VARIATIONS_DRAGNDROP__
//------------------------------------------------------------------------------
/**
*/
void 
VariationListWidget::mousePressEvent( QMouseEvent *event )
{
	VariationListItem* item = (VariationListItem*)this->itemAt(event->pos());
	if (item && item->parent())
	{
		this->startDragPos = event->pos();
		this->isDragging = true;
		this->dragItem = item;
	}
	QTreeWidget::mousePressEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
VariationListWidget::mouseMoveEvent( QMouseEvent *event )
{
	if (this->isDragging)
	{
		// create a drag object and mime data
		QDrag* drag = new QDrag(this);
		QMimeData* mimeData = new QMimeData;

		// create pixmap
		QFont font = this->font();
		font.setPointSize(9);
		font.setFamily("Segoe UI");
		font.setKerning(true);
		QRect rect = this->visualItemRect(this->dragItem);
		QSize size = rect.size();
		QPoint pos = rect.topLeft();
		rect.setX(0);
		rect.setY(0);
		rect.setSize(size);
		
		// create pixmap and painter
		QPixmap pixmap(size);
		QPainter painter(&pixmap);
		
		// first render background, then text
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(rect, this->palette().highlight());
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.setFont(font);
		painter.setPen(QPen(Qt::white));
		painter.drawText(rect, this->dragItem->text(0));
		painter.end();
		
		// we don't actually need to use any mime data so we store the variation in the user pointer instead
		mimeData->setText(this->dragItem->text(0));
		mimeData->setUserData(0, (QObjectUserData*)this->dragItem->GetVariation().get());
		drag->setHotSpot(this->startDragPos - pos);
		drag->setMimeData(mimeData);
		drag->setPixmap(pixmap);
		drag->exec(Qt::MoveAction);
	}
	else
	{
		QTreeWidget::mouseMoveEvent(event);
	}

}

//------------------------------------------------------------------------------
/**
*/
void 
VariationListWidget::mouseReleaseEvent( QMouseEvent *event )
{
	QTreeWidget::mouseReleaseEvent(event);
}

#endif

//------------------------------------------------------------------------------
/**
*/
void 
VariationListWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
	// get item at position
	VariationListItem* item = (VariationListItem*)this->itemAt(event->pos());
	if (item && item->parent())
	{
		// create variation!
		emit CreateNode(item->GetVariation());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
VariationListWidget::contextMenuEvent( QContextMenuEvent* event )
{
	// get item at position
	VariationListItem* item = (VariationListItem*)this->itemAt(event->pos());
	if (item && item->parent())
	{
		// create context menu
		QMenu menu(this);
		QAction* action = new QAction("Info", this);
		menu.addAction(action);
		menu.exec(event->globalPos());
	}

}

} // namespace Nody