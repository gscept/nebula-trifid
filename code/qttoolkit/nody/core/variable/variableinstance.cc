//------------------------------------------------------------------------------
//  variableinstance.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variableinstance.h"
#include "variable/variable.h"
#include "node/node.h"
#include "link/link.h"
#include "project/project.h"

namespace Nody
{
__ImplementClass(Nody::VariableInstance, 'VRIN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
VariableInstance::VariableInstance() :
	variable(0),
	inLink(0),
    graphics(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VariableInstance::~VariableInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::Setup(const Ptr<Variable>& origVar)
{
	n_assert(origVar.isvalid());
	n_assert(!this->variable.isvalid());
	this->variable = origVar;
    this->inferredType = origVar->GetType();
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstance::Discard()
{
	n_assert(this->variable.isvalid());
	n_assert(this->node.isvalid());
	Nody::Project::DeregisterParameterName(this);
	this->variable = 0;
	this->node = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstance::GenerateGraphics()
{
	n_assert(!this->graphics.isvalid());
	this->graphics = VariableInstanceGraphics::Create();
	this->graphics->SetVariableInstance(this);
	this->graphics->Generate();
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstance::DestroyGraphics()
{
	n_assert(this->graphics.isvalid());
	this->graphics->Destroy();
	this->graphics = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::SetInferredType(const VarType& type)
{
    n_assert(this->GetOriginalVariable()->IsAllowingInference());

    // only set inferred type if we are using 'fuzzy' type 
    const VarType& varType = this->GetOriginalVariable()->GetType();
    if (varType.IsA(VarType::Any) || varType.IsA(VarType::AnyVec))
    {
        this->inferredType = type;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstance::ResetInferredType()
{
    this->inferredType = this->variable->GetType();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<VariableInstanceGraphics>& 
VariableInstance::GetGraphics() const
{
	return this->graphics;
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::SetNode(const Ptr<Node>& node)
{
	n_assert(node.isvalid());
	this->node = node;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Node>& 
VariableInstance::GetNode() const
{
	return this->node;
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::AddOutLink(const Ptr<Link>& link)
{
	n_assert(link.isvalid());
	if (this->outLinks.IsEmpty()) this->graphics->Lock();
	this->outLinks.Append(link);
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::RemoveOutLink(const Ptr<Link>& link)
{
	n_assert(link.isvalid());
	this->outLinks.EraseIndex(this->outLinks.FindIndex(link));
	if (this->outLinks.IsEmpty()) this->graphics->Unlock();
}

//------------------------------------------------------------------------------
/**
*/
const bool 
VariableInstance::HasOutLinks() const
{
    return this->outLinks.Size() > 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VariableInstance::SetInLink(const Ptr<Link>& link)
{
	n_assert(link.isvalid());
	this->inLink = link;
	this->graphics->Lock();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Link>& 
VariableInstance::GetInLink() const
{
	return this->inLink;
}

//------------------------------------------------------------------------------
/**
*/
void 
VariableInstance::UnsetInLink()
{
	n_assert(this->inLink.isvalid());
	this->inLink = 0;
	this->graphics->Unlock();
}

//------------------------------------------------------------------------------
/**
*/
bool
VariableInstance::CanConnect(const Ptr<VariableInstance>& lhs, const Ptr<VariableInstance>& rhs)
{
	n_assert(rhs.isvalid());
	const Ptr<Variable>& lhsOrig = lhs->GetOriginalVariable();
	const Ptr<Variable>& rhsOrig = rhs->GetOriginalVariable();

	// the variables are only linkable iff none of the variables are linked, 
	// the from-variable (lhs) is an output/constant/global and the to-variable (rhs) is an input, 
	// and lastly that both variables resides in different nodes
	if (!rhs->inLink.isvalid() &&
		(lhs->GetOriginalVariable()->GetIOFlag() == Variable::Output) &&
		(rhs->GetOriginalVariable()->GetIOFlag() == Variable::Input) &&
		(lhs->GetNode() != rhs->GetNode()))
	{
		// test if types can be mapped and return true if possible
		return lhsOrig->GetType().CanMap(rhsOrig->GetType());
	}
	return false;	
}

} // namespace Nody