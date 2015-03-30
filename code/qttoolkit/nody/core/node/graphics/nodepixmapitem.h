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

	/// set pointer parent graphics node
	void SetNodeGraphics(const Ptr<NodeGraphics>& node);
	/// returns pointer to graphics node which is the owner of this item
	const Ptr<NodeGraphics>& GetNodeGraphics();
private:
	Ptr<NodeGraphics> node;
}; 

} // namespace Nody
//------------------------------------------------------------------------------