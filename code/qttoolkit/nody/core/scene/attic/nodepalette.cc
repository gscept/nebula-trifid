//------------------------------------------------------------------------------
//  nodepalette.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodepalette.h"
#include "node/nodeinstance.h"

namespace Nody
{
__ImplementClass(Nody::NodePalette, 'NOPL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodePalette::NodePalette() :
	graphics(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NodePalette::~NodePalette()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
NodePalette::AddNode( const Ptr<Node>& node )
{
	// add to palette
	this->palette.Append(node);

	// create instance
	Ptr<NodeInstance> instance = node->CreateInstance();

	// add to instance list
	this->nodeInstances.Append(instance);

	// add instance to graphics if graphics exist
	if (this->graphics.isvalid())
	{
		// generate graphics for instance
		instance->GenerateGraphics();

		// add graphics to scene
		this->graphics->AddNodeGraphics(instance->GetGraphics());
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NodePalette::RemoveNode( const Ptr<Node>& node )
{
	// find index of node
	IndexT index = this->palette.FindIndex(node);

	// remove at index
	this->palette.EraseIndex(index);

	// get node instance
	Ptr<NodeInstance> instance = this->nodeInstances[index];

	// remove from graphics
	if (this->graphics.isvalid())
	{
		this->graphics->RemoveNodeGraphics(instance->GetGraphics());
	}
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Node>& 
NodePalette::GetNode( IndexT i ) const
{
	n_assert(i < this->palette.Size());
	return this->palette[i];
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Node> 
NodePalette::FindNode( const Util::String& name ) const
{
	IndexT i;
	for (i = 0; i < this->palette.Size(); i++)
	{
		const Ptr<Node> node = this->palette[i];
		if (node->GetName() == name)
		{
			return node;
		}
	}

	// if location fails, return NULL
	return NULL;
}


} // namespace Nody