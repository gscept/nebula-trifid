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
    /// set the path in which this directory lies
    void SetPath(const Util::String& path);
	/// set if this directory is system maintained
	void SetSystem(bool b);

	/// setup the item, this will create the graphics + icon together with a description
	void Setup();
	/// discard the item
	void Discard();

	/// handle clicking
	void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
	/// call when a directory is clicked
	void OnSelected(const QString&, const QString&);

private:
	bool system;
	Util::String dir;
    Util::String path;
	
};

//------------------------------------------------------------------------------
/**
*/
inline void
TiledDirectoryItem::SetDirectory(const Util::String& dir)
{
	this->dir = dir;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledDirectoryItem::SetPath(const Util::String& path)
{
    this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TiledDirectoryItem::SetSystem(bool b)
{
	this->system = b;
}

} // namespace ResourceBrowser