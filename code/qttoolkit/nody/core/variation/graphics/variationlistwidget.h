#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariationListWidget
    
    Overrides QTreeWidget with special drag'n'drop features.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variationlistitem.h"
#include <QtGui/QTreeWidget>
#include <QtGui/QMouseEvent>

namespace Nody
{
class VariationListWidget : 
	public QTreeWidget
{
	Q_OBJECT	
public:
	/// constructor
	VariationListWidget(QWidget* parent);
	/// destructor
	virtual ~VariationListWidget();

	
#if __VARIATIONS_DRAGNDROP__
	/// handle mouse move press events
	void mousePressEvent(QMouseEvent *event);
	/// handle mouse move
	void mouseMoveEvent(QMouseEvent *event);
	/// handle mouse release
	void mouseReleaseEvent(QMouseEvent *event);
#endif

	/// handle double click
	void mouseDoubleClickEvent(QMouseEvent* event);
	/// handle right click
	void contextMenuEvent(QContextMenuEvent* event);

signals:
	/// emitted whenever we request to create a new node
	void CreateNode(const Ptr<Nody::Variation>&);

private:
	bool isDragging;
	QPoint startDragPos;
	VariationListItem* dragItem;
}; 
} // namespace Nody
//------------------------------------------------------------------------------