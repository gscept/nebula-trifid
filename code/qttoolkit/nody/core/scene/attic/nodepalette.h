#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodePalette
    
    A node palette contains the set of all available nodes.
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/nodegraphicspalette.h"
#include "node/node.h"
namespace Nody
{
class NodePalette : public Core::RefCounted
{
	__DeclareClass(NodePalette);
public:
	/// constructor
	NodePalette();
	/// destructor
	virtual ~NodePalette();

	/// adds a node to the palette, internally creates a palette instance node for graphical representation
	void AddNode(const Ptr<Node>& node);
	/// removes a node from the palette, internally removes instance node
	void RemoveNode(const Ptr<Node>& node);

	/// gets a node based on index
	const Ptr<Node>& GetNode(IndexT i) const;
	/// gets a node based on name, return 0 if it fails
	const Ptr<Node> FindNode(const Util::String& name) const;

	/// returns pointer to graphics palette
	const Ptr<NodeGraphicsPalette> GetGraphics() const;
	/// sets pointer to graphics palette
	void SetGraphics(const Ptr<NodeGraphicsPalette>& graphics);

private:
	Ptr<NodeGraphicsPalette> graphics;
	Util::Array<Ptr<Node> > palette;
	Util::Array<Ptr<NodeInstance> > nodeInstances;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<NodeGraphicsPalette> 
NodePalette::GetGraphics() const
{
	return this->graphics;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NodePalette::SetGraphics( const Ptr<NodeGraphicsPalette>& graphics )
{
	n_assert(graphics.isvalid());
	this->graphics = graphics;
}
} // namespace Nody
//------------------------------------------------------------------------------