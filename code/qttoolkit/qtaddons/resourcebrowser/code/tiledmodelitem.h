#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledModelItem
	
	This class represents a visual representation of 
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "tiledgraphicsitem.h"
#include "imageloaderthread.h"
namespace ResourceBrowser
{
class ImageLoaderUnit;
class TiledModelItem : public TiledGraphicsItem
{
    Q_OBJECT
public:
	/// constructor
	TiledModelItem();
	/// destructor
	virtual ~TiledModelItem();

    /// set path to model category folder
    void SetPath(const Util::String& path);
    /// set model category
    void SetCategory(const Util::String& cat);
    /// set model file name
    void SetFilename(const Util::String& file);

	/// setup the item, reduces the texture path to category/file, locates the thumbnail and loads it as a preview, updates the label with the image type
	void Setup();
	/// discard the item, deallocates the image
	void Discard();

	/// handle clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// handle right clicking
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private slots:
	/// called when the preview image is loaded
	void OnPreviewLoaded();

signals:
	/// signalled when a texture is selected
	void OnSelected(const QString& sur);

private:
	ImageLoaderUnit* loader;
    Util::String path;
    Util::String category;
    Util::String filename;
};

//------------------------------------------------------------------------------
/**
*/
inline void
TiledModelItem::SetPath(const Util::String& path)
{
    this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledModelItem::SetCategory(const Util::String& cat)
{
    this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledModelItem::SetFilename(const Util::String& file)
{
    this->filename = file;
}

} // namespace ResourceBrowser