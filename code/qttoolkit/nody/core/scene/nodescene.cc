//------------------------------------------------------------------------------
//  nodescene.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "nodescene.h"
#include "node/node.h"
#include "link/link.h"
#include "variable/variableinstance.h"

namespace Nody
{
__ImplementClass(Nody::NodeScene, 'NOSC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
NodeScene::NodeScene() : 
    mainNode(0),
	graphicsScene(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
NodeScene::~NodeScene()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<NodeGraphicsScene>& 
NodeScene::GetNodeSceneGraphics() const
{
	return this->graphicsScene;
}

//------------------------------------------------------------------------------
/**
	Links graphics with scene, and scene with graphics
*/
void 
NodeScene::LinkWithGraphics( const Ptr<NodeGraphicsScene>& graphics )
{
	n_assert(graphics.isvalid());
	this->graphicsScene = graphics;
	graphics->SetNodeScene(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeScene::AddNode( const Ptr<Node>& node )
{
	// if we have graphics attached, add node to graphics
	if (this->graphicsScene.isvalid())
	{
		// generate graphics for node
		node->GenerateGraphics();

		// then add it to the scene
		this->graphicsScene->AddNodeGraphics(node->GetGraphics());
	}

	// add node to list
	this->nodes.Append(node);

    // assign supernode if the incoming node is one
    if (node->IsSuperNode())
    {
        n_assert(!this->mainNode.isvalid());
        this->mainNode = node;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeScene::RemoveNode( const Ptr<Node>& node )
{
    n_assert(node != this->mainNode);

	// if we have graphics attached, remove node from graphics
	if (this->graphicsScene.isvalid())
	{
		// remove graphics
		this->graphicsScene->RemoveNodeGraphics(node->GetGraphics());
	}

	// get links from node (copy list since the actual list will change with each removed link!)
	Util::Array<Ptr<Link>> nodeLinks = node->GetLinks();

	// remove links
	IndexT i;
	for (i = 0; i < nodeLinks.Size(); i++)
	{
		this->RemoveLink(nodeLinks[i]);
	}    

	// discard node
	node->Discard();

	// remove node from list
	this->nodes.EraseIndex(this->nodes.FindIndex(node));
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeScene::CreateLink( const Ptr<VariableInstance>& from, const Ptr<VariableInstance>& to )
{
	n_assert(from.isvalid());
	n_assert(to.isvalid());

	// create link
	Ptr<Link> link = Link::Create();

	// link variables together
	if (link->LinkVariables(from, to))
	{
		// generate graphics for link
		link->GenerateGraphics();

		// add graphics to scene
		this->graphicsScene->AddLinkGraphics(link->GetGraphics());

		// set anchors and update graphics
		// ideally I don't want to do this in the logical scene but it's easiest this way...
		const Ptr<LinkGraphics>& linkGraphics = link->GetGraphics();
		linkGraphics->SetAnchorFrom(from->GetGraphics()->GetCenter());
		linkGraphics->SetAnchorTo(to->GetGraphics()->GetCenter());
		linkGraphics->Update();

		// finally lock graphics
		linkGraphics->Lock();

		// add link to list
		this->links.Append(link);
	}
	else
	{
		// just destroy the link!
		link = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeScene::RemoveLink( const Ptr<Link>& link )
{
    n_assert(link.isvalid());
    this->graphicsScene->RemoveLinkGraphics(link->GetGraphics());
    link->Discard();
    link->DestroyGraphics();
    this->links.EraseIndex(this->links.FindIndex(link));
}

//------------------------------------------------------------------------------
/**
*/
void 
NodeScene::Clear()
{
	IndexT i;

    // remove focus in graphics
    this->GetNodeSceneGraphics()->DropFocus();

	// discard links
	for (i = 0; i < links.Size(); i++)
	{
		this->graphicsScene->RemoveLinkGraphics(this->links[i]->GetGraphics());
		this->links[i]->Discard();
		this->links[i]->DestroyGraphics();
	}
	this->links.Clear();

	// discard nodes
	for (i = 0; i < nodes.Size(); i++)
	{
		this->graphicsScene->RemoveNodeGraphics(this->nodes[i]->GetGraphics());
		this->nodes[i]->Discard();
		this->nodes[i]->DestroyGraphics();
	}
	this->nodes.Clear();

    // set main node to 0
    this->mainNode = 0;
}

} // namespace Nody