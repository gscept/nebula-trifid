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
#include "imageloaderthread.h"

#include <QObject>
#include <QGraphicsItemGroup>
#include <QDateTime>
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

	/// set path to category folder
	void SetPath(const Util::String& path);
	/// get path to category folder
	const Util::String& GetPath() const;
	/// set category
	void SetCategory(const Util::String& cat);
	/// get category
	const Util::String& GetCategory() const;
	/// set file name
	void SetFilename(const Util::String& file);
	/// get file name
	const Util::String& GetFilename() const;
	/// get last changed date
	const QDateTime& GetLastChanged() const;

	/// handle mouse entering item
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	/// handle mouse leaving item
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	/// set size of item
	void SetSize(const QSize& size);

signals:
	/// emit when item gets right clicked
	void ItemRightClicked(QGraphicsSceneContextMenuEvent* event);

protected slots:
	/// called when the preview image is loaded
	void OnPreviewLoaded();
protected:

	/// call rescale whenever the item resizes
	virtual void Rescale();

	QSize size;
	QGraphicsPolygonItem* background;
	QGraphicsPixmapItem* graphics;
	QGraphicsTextItem* label;
	QDateTime lastChanged;

	Util::String path;
	Util::String category;
	Util::String filename;

	// loader for the thumbnail
	ImageLoaderUnit* loader;
};

//------------------------------------------------------------------------------
/**
*/
inline void
TiledGraphicsItem::SetPath(const Util::String& path)
{
	this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
TiledGraphicsItem::GetPath() const
{
	return this->path;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledGraphicsItem::SetCategory(const Util::String& cat)
{
	this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
TiledGraphicsItem::GetCategory() const
{
	return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledGraphicsItem::SetFilename(const Util::String& file)
{
	this->filename = file;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
TiledGraphicsItem::GetFilename() const
{
	return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline const QDateTime&
TiledGraphicsItem::GetLastChanged() const
{
	return this->lastChanged;
}


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