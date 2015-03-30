//------------------------------------------------------------------------------
//  node.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "node.h"
#include "nodeinstance.h"
#include "variation/variation.h"

namespace Nody
{
__ImplementClass(Nody::Node, 'NODE', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Node::Node() :
	variation(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Node::~Node()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<NodeInstance> 
Node::CreateInstance()
{
	Ptr<NodeInstance> instance = NodeInstance::Create();
	Ptr<Node> thisPtr(this);
	instance->Setup(thisPtr);
	return instance;
}

} // namespace Nody