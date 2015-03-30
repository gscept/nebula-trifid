#pragma once
//------------------------------------------------------------------------------
/**
	@class ResourceBrowser::TiledDirectoryItem
	
	Implements a TiledGraphicsItem which corresponds to a directory.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "tiledgraphicsitem.h"
namespace ResourceBrowser
{
class TiledDirectoryItem : public TiledGraphicsItem
{
	Q_OBJECT
public:
	/// constructor
	TiledDirectoryItem();
	/// destructor
	virtual ~TiledDirectoryItem();

	/// set the directory
	void SetDirectory(const Util::String& dir);

	/// setup the item, this will create the graphics + icon together with a description
	void Setup();
	/// discard the item
	void Discard();

	/// handle clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
	/// call when a directory is clicked
	void OnSelected(const QString&);

private:
	Util::String dir;
	
};

//------------------------------------------------------------------------------
/**
*/
inline void
TiledDirectoryItem::SetDirectory(const Util::String& dir)
{
	this->dir = dir;
}


} // namespace ResourceBrowser