#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledSurfaceItem
	
	A surface item is a resource which corresponds to a material surface.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "tiledgraphicsitem.h"
#include "imageloaderthread.h"
namespace ResourceBrowser
{
class ImageLoaderUnit;
class TiledSurfaceItem : public TiledGraphicsItem
{
    Q_OBJECT
public:
	/// constructor
	TiledSurfaceItem();
	/// destructor
	virtual ~TiledSurfaceItem();

    /// set path to surface category folder
    void SetPath(const Util::String& path);
    /// set surface category
    void SetCategory(const Util::String& cat);
    /// set surface file name
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
TiledSurfaceItem::SetPath(const Util::String& path)
{
    this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledSurfaceItem::SetCategory(const Util::String& cat)
{
    this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledSurfaceItem::SetFilename(const Util::String& file)
{
    this->filename = file;
}

} // namespace ResourceBrowser