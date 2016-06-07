#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::BaseGenerator
    
    Base class for code/script generation. Override and implement for language specific generation and validation.

    There are two traversal methods, depth first and breadth first.

    Breadth first works by the following visitation priority
        Node
            Links
            Recurse with Links destination Node
            Revisit links
        Revisit node
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "node/node.h"
#include "link/link.h"
#include "io/uri.h"
namespace Nody
{
class Node;
class Link;
class NodeScene;
class Generator : public Core::RefCounted
{
	__DeclareClass(Generator);
public:
	/// constructor
	Generator();
	/// destructor
	virtual ~Generator();

    /// set to true if the generator should visualize the generation process, the delay is the time each component is being shown
    void SetVisualize(bool b, uint delay);

    /// generate code to path
    virtual void GenerateToFile(const Ptr<NodeScene>& scene, const IO::URI& path);
    /// generate code to buffer
    virtual void GenerateToBuffer(const Ptr<NodeScene>& scene, Util::Blob& output);

    /// returns generation status
    const bool GetStatus() const;

    /// validate generated buffer
	virtual void Validate(const Ptr<Nody::NodeScene>& scene, const Util::String& language);

protected:
    /// handles a single node in the network
    virtual void VisitNode(const Ptr<Node>& node);
    /// handles a single link in the network
    virtual void VisitLink(const Ptr<Link>& link);
    /// called on the way back on a visited node
    virtual void RevisitNode(const Ptr<Node>& node);
    /// called on the way back on a visited link
    virtual void RevisitLink(const Ptr<Link>& link);

    /// traverses node hierarchy breadth first
    void BreadthFirst(const Ptr<Node>& start);
    /// recursive breadth first function helper
    void BreadthFirstHelper(const Ptr<Node>& current);

    /// traverses node hierarchy depth first
    void DepthFirst(const Ptr<Node>& start);
    /// recursive depth first function helper
    void DepthFirstHelper(const Ptr<Node>& current);

	/// validate if there is any loops
	void ValidateGraph(const Ptr<Node>& start);
	/// recursive helper for graph validation
	void ValidateGraphHelper(const Ptr<Node>& current, Util::Array<Ptr<Node>>& visitedNodes);

    bool error;
	Util::String errorMessage;
    bool visualize;
    uint delay;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Generator::SetVisualize( bool b, uint delay )
{
    this->visualize = b;
    this->delay = delay;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Generator::GetStatus() const
{
    return !this->error;
}

} // namespace Nody
//------------------------------------------------------------------------------