#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledTextureItem
	
	Implements a tiled texture item.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "tiledgraphicsitem.h"
namespace ResourceBrowser
{
class ImageLoaderUnit;
class TiledTextureItem : public TiledGraphicsItem
{
	Q_OBJECT
public:
	/// constructor
	TiledTextureItem();
	/// destructor
	virtual ~TiledTextureItem();

    /// set path to texture category folder
    void SetPath(const Util::String& path);
    /// set texture category
    void SetCategory(const Util::String& cat);
    /// set texture file name
    void SetFilename(const Util::String& file);

	/// setup the item, reduces the texture path to category/file, locates the .dds and loads the preview, updates the label with the image type
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
	void OnSelected(const QString& tex);

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
TiledTextureItem::SetPath(const Util::String& path)
{
    this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledTextureItem::SetCategory(const Util::String& cat)
{
    this->category = cat;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledTextureItem::SetFilename(const Util::String& file)
{
    this->filename = file;
}

} // namespace ResourceBrowser