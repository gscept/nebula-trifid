#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodyScene
    
    A node scene represents a virtual scene of nodes and their connections
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "graphics/nodegraphicsscene.h"
#include "link/link.h"
#include "node/node.h"

namespace Nody
{
class NodeGraphicsScene;
class VariableInstance;
class NodeScene : public Core::RefCounted
{
	__DeclareClass(NodeScene);
public:
	/// constructor
	NodeScene();
	/// destructor
	virtual ~NodeScene();

	/// returns pointer to graphics
	const Ptr<NodeGraphicsScene>& GetNodeSceneGraphics() const;
	/// sets graphics pointer
	void LinkWithGraphics(const Ptr<NodeGraphicsScene>& graphics);

	/// adds a node to the node scene, also adds graphics to graphics scene if available
	void AddNode(const Ptr<Node>& node);
	/// removes a node from the node scene, also removes graphics from graphics scene if available
	void RemoveNode(const Ptr<Node>& node);

	/// creates link between variables instances
	void CreateLink(const Ptr<VariableInstance>& from, const Ptr<VariableInstance>& to);
    /// removes link
    void RemoveLink(const Ptr<Link>& link);

    /// returns super node of scene, or null pointer if none exists (this is a node which is setup using a super variation)
    const Ptr<Node>& GetSuperNode() const;    

	/// clears scene of nodes
	void Clear();

    /// get list of nodes
    const Util::Array<Ptr<Node>>& GetNodes() const;
    /// get list of links
    const Util::Array<Ptr<Link>>& GetLinks() const;
private:
    Ptr<Node> mainNode;
	Util::Array<Ptr<Node> > nodes;
	Util::Array<Ptr<Link> > links;
	Ptr<NodeGraphicsScene> graphicsScene;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Node>& 
NodeScene::GetSuperNode() const
{
    return this->mainNode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Node>>& 
NodeScene::GetNodes() const
{
    return this->nodes;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Link>>& 
NodeScene::GetLinks() const
{
    return this->links;
}
} // namespace Nody
//------------------------------------------------------------------------------