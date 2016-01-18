#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledSurfaceItem
	
	A surface item is a resource which corresponds to a material surface.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "tiledgraphicsitem.h"
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
  
	/// setup the item, reduces the texture path to category/file, locates the thumbnail and loads it as a preview, updates the label with the image type
	void Setup();
	/// discard the item, deallocates the image
	void Discard();

	/// handle clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// handle right clicking
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:
	/// signalled when a texture is selected
	void OnSelected(const QString& sur);

private:

};

} // namespace ResourceBrowser