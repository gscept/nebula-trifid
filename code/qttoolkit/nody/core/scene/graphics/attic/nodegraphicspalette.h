#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeGraphicsPalette
    
    A graphics palette is the graphical representation of the palette of nodes available
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include <QGraphicsScene>

namespace Nody
{
class NodeGraphics;
class NodeGraphicsPalette : 
	public QGraphicsScene,
	public Core::RefCounted
{
	__DeclareClass(NodeGraphicsPalette);
public:
	/// constructor
	NodeGraphicsPalette();
	/// destructor
	virtual ~NodeGraphicsPalette();

	/// adds a graphics node to the palette
	void AddNodeGraphics(const Ptr<NodeGraphics>& node);
	/// removes node graphics
	void RemoveNodeGraphics(const Ptr<NodeGraphics>& node);
}; 
} // namespace Nody
//------------------------------------------------------------------------------