#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::SuperVariationInstance
    
    Denotes an instance of a super variation. 
    This is what should be used to instantiate the super variation template class.
    This is then put in a node which causes the node to become a supernode, at which all outputs ultimately ends up.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
namespace Nody
{
class Node;
class SuperVariation;
class VariableInstance;
class SuperVariationInstance : public Core::RefCounted
{
	__DeclareClass(SuperVariationInstance);
    __DeclareSingleton(SuperVariationInstance);
public:
	/// constructor
	SuperVariationInstance();
	/// destructor
	virtual ~SuperVariationInstance();

    /// sets up instance from variation
    void Setup(const Ptr<SuperVariation>& superVariation, const Ptr<Node>& node);
    /// discards an instance
    void Discard();

    /// return list of variable instances
    const Util::Array<Ptr<VariableInstance>>& GetInputs() const;
    /// get input by name
    const Ptr<VariableInstance>& GetInput(const Util::String& name) const;

    /// returns original supervariation
    const Ptr<SuperVariation>& GetOriginalSuperVariation() const;

private:
    Ptr<SuperVariation> superVariation;
    Ptr<Node> node;
    Util::Array<Ptr<VariableInstance>> inputs;
    Util::Dictionary<Util::String, Ptr<VariableInstance> > inputsByName;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<VariableInstance>>& 
SuperVariationInstance::GetInputs() const
{
    return this->inputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
SuperVariationInstance::GetInput( const Util::String& name ) const
{
    n_assert(this->inputsByName.Contains(name));
    return this->inputsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SuperVariation>& 
SuperVariationInstance::GetOriginalSuperVariation() const
{
    return this->superVariation;
}

} // namespace Nody
//------------------------------------------------------------------------------