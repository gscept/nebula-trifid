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

	/// setup the item, reduces the texture path to category/file, locates the .dds and loads the preview, updates the label with the image type
	void Setup();
	/// discard the item, deallocates the image
	void Discard();

	/// handle clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// handle right clicking
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:
	/// signalled when a texture is selected
	void OnSelected(const QString& tex);
};

} // namespace ResourceBrowser