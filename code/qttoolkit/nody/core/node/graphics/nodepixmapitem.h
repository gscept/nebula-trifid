#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodePixmapItem
    
    Implements a pixmap-item specific for Nody
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QGraphicsPixmapItem>

namespace Nody
{
class NodeGraphics;
class NodePixmapItem : 
	public Core::RefCounted,
	public QGraphicsPixmapItem
{
	__DeclareClass(NodePixmapItem);
public:
	/// constructor
	NodePixmapItem();
	/// destructor
	virtual ~NodePixmapItem();

private:
	friend class NodeGraphics;
	Ptr<NodeGraphics> node;
}; 

} // namespace Nody
//------------------------------------------------------------------------------