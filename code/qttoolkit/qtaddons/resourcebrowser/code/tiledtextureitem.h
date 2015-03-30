#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledTextureItem
	
	Implements a tiled texture item.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "tiledgraphicsitem.h"
#include <QThread>
namespace ResourceBrowser
{
class TextureLoaderUnit;
class TiledTextureItem : public TiledGraphicsItem
{
	Q_OBJECT
public:
	/// constructor
	TiledTextureItem();
	/// destructor
	virtual ~TiledTextureItem();

	/// set texture path
	void SetPath(const Util::String& path);

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
	TextureLoaderUnit* loader;
	Util::String texture;
};

//------------------------------------------------------------------------------
/**
*/
inline void
TiledTextureItem::SetPath(const Util::String& path)
{
	this->texture = path;
}

} // namespace ResourceBrowser