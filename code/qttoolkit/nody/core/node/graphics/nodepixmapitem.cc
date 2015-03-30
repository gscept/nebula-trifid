//------------------------------------------------------------------------------
//  nodepixmapitem.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodegraphics.h"
#include "nodepixmapitem.h"


namespace Nody
{
__ImplementClass(Nody::NodePixmapItem, 'NDPI', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodePixmapItem::NodePixmapItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodePixmapItem::~NodePixmapItem()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodePixmapItem::SetNodeGraphics( const Ptr<NodeGraphics>& node )
{
	n_assert(node.isvalid());
	this->node = node;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<NodeGraphics>& 
NodePixmapItem::GetNodeGraphics()
{
	return this->node;
}
} // namespace Nody