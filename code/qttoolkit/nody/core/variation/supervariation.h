#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::SuperVariation
    
    A super variation describes the content of an super node in Nody. 
    This node is essentially responsible for tying everything together.

    The super variation may only take inputs, and will on the instance level 
    be responsible for traversing the node network and resolve all connections leading to this node.

    This illustration shows a typical example of how the inputs to a super node may be connected.

           -----------------------------------
           |                 S               |
           |      input 1         input 2    |
           |        |               |        |
           |       / \             / \       | 
           |      N   N           |   N      |
           |                      N          |
           |                      |          |
           |                     / \         |
           |                    N   N        |
           |                                 |
           -----------------------------------
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "supervariationinstance.h"
#include "variable/variable.h"
namespace Nody
{
class SuperVariation : public Core::RefCounted
{
	__DeclareClass(SuperVariation);
public:
	/// constructor
	SuperVariation();
	/// destructor
	virtual ~SuperVariation();

    /// discard variation
    void Discard();

    /// creates an instance of a super variation
    virtual Ptr<SuperVariationInstance> CreateInstance(const Ptr<Node>& node);

    /// adds an input variable to the variation
    void AddInput(const Ptr<Variable>& var);

    /// sets the name of the variation
    void SetName(const Util::String& name);
    /// gets the name of the variation
    const Util::String& GetName() const;

    /// set description
    void SetDescription(const Util::String& description);
    /// get description
    const Util::String& GetDescription() const;


    /// returns list of inputs
    const Util::Array<Ptr<Variable>>& GetInputs() const;

private:
    Util::String name;
    Util::String description;

    Util::Array<Ptr<Variable>> inputs;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
SuperVariation::SetName( const Util::String& name )
{
    n_assert(name.IsValid());
    this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
SuperVariation::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SuperVariation::SetDescription( const Util::String& description )
{
    this->description = description;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
SuperVariation::GetDescription() const
{
    return this->description;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Variable>>& 
SuperVariation::GetInputs() const
{
    return this->inputs;
}

} // namespace Nody
//------------------------------------------------------------------------------