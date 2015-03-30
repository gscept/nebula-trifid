//------------------------------------------------------------------------------
//  instanceserverbase.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "instanceserverbase.h"

using namespace Models;
namespace Instancing
{
__ImplementSingleton(Instancing::InstanceServerBase);
__ImplementClass(Instancing::InstanceServerBase, 'INSB', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
InstanceServerBase::InstanceServerBase() :
	renderer(0),
	modelNode(0), 
	isBeginInstancing(false),
	isOpen(false),
	multiplier(1)	
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
InstanceServerBase::~InstanceServerBase()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
InstanceServerBase::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
InstanceServerBase::Close()
{
	n_assert(this->IsOpen());
	this->modelNode = 0;
	this->instancesByCode.Clear();
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
	Prepare to render model.
*/
void 
InstanceServerBase::BeginInstancing(const Ptr<ModelNode>& modelNode, const SizeT multiplier)
{
	n_assert(this->IsOpen());
	n_assert(multiplier > 0);
	n_assert(!this->isBeginInstancing);
	this->multiplier = multiplier;
	this->modelNode = modelNode;
	this->isBeginInstancing = true;
}

//------------------------------------------------------------------------------
/**
	Add model instances to list.
*/
void 
InstanceServerBase::AddInstance( const IndexT& instanceCode, const Ptr<ModelNodeInstance>& nodeInstance )
{
	n_assert(this->IsOpen());
	n_assert(this->modelNode.isvalid());
	n_assert(nodeInstance->GetModelNode() == this->modelNode);
	n_assert(this->isBeginInstancing);

	if (!this->instancesByCode.Contains(instanceCode))
	{
		this->instancesByCode.Add(instanceCode, Util::Array<Ptr<ModelNodeInstance> >());
		this->instancesByCode[instanceCode].Append(nodeInstance);
	}
	else
	{
		this->instancesByCode[instanceCode].Append(nodeInstance);
	}
}

//------------------------------------------------------------------------------
/**
	Performs actual instanced rendering, override this in a subclass.
*/
void 
InstanceServerBase::Render()
{
	n_error("InstanceServerBase::Render() called!");
}

//------------------------------------------------------------------------------
/**
	End the instancing.
*/
void 
InstanceServerBase::EndInstancing()
{
	n_assert(this->IsOpen());
	n_assert(this->isBeginInstancing);
	this->instancesByCode.Clear();
	this->modelNode = 0;
	this->isBeginInstancing = false;
}
} // namespace Instancing
