#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledGraphicsView
	
	Inherits QGraphicsView to insert Pixmap items that tile.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QGraphicsView>
namespace ResourceBrowser
{

class TiledGraphicsItem;
class TiledGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:

	enum SortingMode
	{
		SortByName,		// file name
		SortByDate		// sort by date
	};

	/// constructor
	TiledGraphicsView(QWidget* parent);
	/// destructor
	virtual ~TiledGraphicsView();

	/// set the size of an item, this will be used to calculate tile size
	void SetItemSize(QSize size);

	/// special function which adds a pixmap item and automatically tiles it
	void AddTiledItem(TiledGraphicsItem* item);
	/// removes an item and rearranges the view
	void RemoveTiledItem(TiledGraphicsItem* item);
	/// get list of items
	const QList<TiledGraphicsItem*>& GetItems() const;

	/// sort items
	void Sort(const SortingMode& mode);

	/// clear view of items, also deletes items so you don't have to!
	void Clear();

	/// rearranges icons when view gets resized or when done adding items
	void Rearrange();

	/// handle resize event
	void resizeEvent(QResizeEvent *event);
private:

	SortingMode sort;
	QSize itemSize;
	QList<TiledGraphicsItem*> items;
	QGraphicsScene* scene;
};

//------------------------------------------------------------------------------
/**
*/
inline const QList<TiledGraphicsItem*>&
TiledGraphicsView::GetItems() const
{
	return this->items;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledGraphicsView::SetItemSize(QSize size)
{
	this->itemSize = size;
	this->Rearrange();
}

} // namespace ResourceBrowser