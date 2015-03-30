#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariableInstance
    
    A variable instance describes a variable as an instance of it, 
	meaning you can change the instance without changing the variable boilerplate
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/variant.h"
#include "variable/graphics/variableinstancegraphics.h"
#include "variable/vartype.h"

namespace Nody
{
class Node;
class Link;
class Variable;
class VariableInstance : public Core::RefCounted
{
	__DeclareClass(VariableInstance);
public:
	/// constructor
	VariableInstance();
	/// destructor
	virtual ~VariableInstance();

	/// sets up instance from variable
	void Setup(const Ptr<Variable>& origVar);
	/// discards instance
	void Discard();

	/// generate graphics
	void GenerateGraphics();
	/// discard graphics
	void DestroyGraphics();
	/// get graphics
	const Ptr<VariableInstanceGraphics>& GetGraphics() const;

    /// set the inferred type, this is only valid if the original variable is using the 'Any' or 'AnyVec' type
    void SetInferredType(const VarType& type);
    /// get the inferred type
    const VarType& GetInferredType() const;
    /// sets the inferred type to be this variable type
    void ResetInferredType();

	/// set node
	void SetNode(const Ptr<Node>& node);
	/// get node
	const Ptr<Node>& GetNode() const;

	/// add a link to be associated with this variable
	void AddOutLink(const Ptr<Link>& link);
	/// remove link associated with this variable
	void RemoveOutLink(const Ptr<Link>& link);
    /// returns true if there are any outputs
    const bool HasOutLinks() const;

	/// sets the link through which this variable instance is connected with another
	void SetInLink(const Ptr<Link>& link);
	/// get connected link
	const Ptr<Link>& GetInLink() const;
	/// disables the incoming link
	void UnsetInLink();
	
	/// test if this variable can be connected with another
	static bool CanConnect(const Ptr<VariableInstance>& lhs, const Ptr<VariableInstance>& rhs);

	/// returns pointer to original variable
	const Ptr<Variable>& GetOriginalVariable() const;

private:
    VarType inferredType;
	Ptr<VariableInstanceGraphics> graphics;
	Ptr<Variable> variable;
	Util::Array<Ptr<Link> > outLinks;
	Ptr<Link> inLink;
	Ptr<Node> node;
};

//------------------------------------------------------------------------------
/**
*/
inline const VarType& 
VariableInstance::GetInferredType() const
{
    return this->inferredType;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variable>& 
VariableInstance::GetOriginalVariable() const
{
	return this->variable;
}

} // namespace Nody
//------------------------------------------------------------------------------