//------------------------------------------------------------------------------
//  generator.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "generator.h"
#include "node/node.h"
#include "link/link.h"

namespace Nody
{
__ImplementClass(Nody::Generator, 'BSGE', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Generator::Generator() :
    error(false),
    visualize(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Generator::~Generator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::GenerateToFile( const Ptr<NodeScene>& scene, const IO::URI& path )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
Generator::GenerateToBuffer(const Ptr<NodeScene>& scene, Util::Blob& output)
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::Validate( const Util::Blob& buffer )
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::VisitNode( const Ptr<Node>& node )
{
    n_assert(node.isvalid());
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::VisitLink( const Ptr<Link>& link )
{
    n_assert(link.isvalid());
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::RevisitNode( const Ptr<Node>& node )
{
    n_assert(node.isvalid());
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::RevisitLink( const Ptr<Link>& link )
{
    n_assert(link.isvalid());
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::BreadthFirst( const Ptr<Node>& start )
{
    n_assert(start.isvalid());
    this->BreadthFirstHelper(start);
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::BreadthFirstHelper( const Ptr<Node>& current )
{
    n_assert(current.isvalid());
    this->VisitNode(current);
    const Util::Array<Ptr<Link>>& links = current->GetLinks();
    Util::Array<Ptr<Link>> linksToVisit;

    // visit links
    IndexT i;
    for (i = 0; i < links.Size(); i++)
    {
        const Ptr<Link>& link = links[i];
        const Ptr<VariableInstance>& var = link->GetFromVariable();
        if (var->GetNode() != current)
        {
            this->VisitLink(link);
            linksToVisit.Append(link);
        }
    }

    // now traverse links and visit their nodes
    for (i = 0; i < linksToVisit.Size(); i++)
    {
        const Ptr<Link>& link = linksToVisit[i];
        this->BreadthFirstHelper(link->GetFromVariable()->GetNode());
    }

    // do the same with the links
    for (i = 0; i < linksToVisit.Size(); i++)
    {
        const Ptr<Link>& link = linksToVisit[i];
        this->RevisitLink(link);
    }

    // when all links have been visited, revisit them
    this->RevisitNode(current);
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::DepthFirst( const Ptr<Node>& start )
{
    n_assert(start.isvalid());
    this->DepthFirstHelper(start);
}

//------------------------------------------------------------------------------
/**
*/
void 
Generator::DepthFirstHelper( const Ptr<Node>& current )
{
    n_assert(current.isvalid());
    this->VisitNode(current);

    // visit links
    const Util::Array<Ptr<Link>>& links = current->GetLinks();
    IndexT i;
    for (i = 0; i < links.Size(); i++)
    {
        const Ptr<Link>& link = links[i];
        const Ptr<VariableInstance>& var = link->GetFromVariable();
        if (var->GetNode() != current)
        {
            this->VisitLink(link);
            this->DepthFirstHelper(var->GetNode());
            this->RevisitLink(link);
        }
    }

    // visit node again
    this->RevisitNode(current);
}
} // namespace Nody