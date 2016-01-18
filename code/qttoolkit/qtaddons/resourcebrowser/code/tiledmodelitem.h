#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledModelItem
	
	This class represents a visual representation of 
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "tiledgraphicsitem.h"
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

};

} // namespace ResourceBrowser