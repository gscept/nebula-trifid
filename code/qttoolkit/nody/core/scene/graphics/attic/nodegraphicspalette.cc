//------------------------------------------------------------------------------
//  nodegraphicspalette.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodegraphicspalette.h"
#include "node/graphics/nodegraphics.h"

namespace Nody
{
__ImplementClass(Nody::NodeGraphicsPalette, 'NDGP', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsPalette::NodeGraphicsPalette()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodeGraphicsPalette::~NodeGraphicsPalette()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsPalette::AddNodeGraphics( const Ptr<NodeGraphics>& node )
{
	// first generate graphics for node
	node->GenerateGraphics();

	// adds node to palette
	node->AddToPalette(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeGraphicsPalette::RemoveNodeGraphics( const Ptr<NodeGraphics>& node )
{
	// removes node from palette
	node->RemoveFromPalette(this);
}
} // namespace Nody