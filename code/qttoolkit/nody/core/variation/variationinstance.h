#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariationInstance
    
    Instantiated variation, holds the actual variable instances which can be per-node unique
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/array.h"
#include "variable/variableinstance.h"

namespace Nody
{
class Node;
class Variation;
class VariableInstance;
class VariationInstance : public Core::RefCounted
{
	__DeclareClass(VariationInstance);
public:
	/// constructor
	VariationInstance();
	/// destructor
	virtual ~VariationInstance();

	/// sets up instance from variation
	void Setup(const Ptr<Variation>& variation, const Ptr<Node>& node);
	/// discards an instance
	void Discard();

	/// returns pointer to original variation
	const Ptr<Variation>& GetOriginalVariation() const;
	
	/// returns reference to list of inputs
	const Util::Array<Ptr<VariableInstance> >& GetInputs() const;
    /// returns reference to input by name
    const Ptr<VariableInstance>& GetInput(const Util::String& name) const;
    /// returns true if output exists
    const bool HasInput(const Util::String& name) const;
	/// returns reference to list of outputs
	const Util::Array<Ptr<VariableInstance> >& GetOutputs() const;
    /// returns reference to output by name
    const Ptr<VariableInstance>& GetOutput(const Util::String& name) const;
    /// returns true if output exists
    const bool HasOutput(const Util::String& name) const;
    /// returns reference to list of hidden
    const Util::Array<Ptr<VariableInstance> >& GetHiddens() const;
    /// returns reference to hidden by name
    const Ptr<VariableInstance>& GetHidden(const Util::String& name) const;
    /// returns true if hidden exists
    const bool HasHidden(const Util::String& name) const;

private:
	Util::Array<Ptr<VariableInstance> > inputs;
	Util::Dictionary<Util::String, Ptr<VariableInstance> > inputsByName;
	Util::Array<Ptr<VariableInstance> > outputs;
	Util::Dictionary<Util::String, Ptr<VariableInstance> > outputsByName;
    Util::Array<Ptr<VariableInstance> > hidden;
    Util::Dictionary<Util::String, Ptr<VariableInstance> > hiddenByName;
	Ptr<Variation> variation;
	Ptr<Node> node;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<VariableInstance> >& 
VariationInstance::GetInputs() const
{
    return this->inputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
VariationInstance::GetInput( const Util::String& name ) const
{
    n_assert(this->inputsByName.Contains(name));
    return this->inputsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
VariationInstance::HasInput( const Util::String& name ) const
{
    return this->inputsByName.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<VariableInstance> >& 
VariationInstance::GetOutputs() const
{
    return this->outputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
VariationInstance::GetOutput( const Util::String& name ) const
{
    n_assert(this->outputsByName.Contains(name));
    return this->outputsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
VariationInstance::HasOutput( const Util::String& name ) const
{
    return this->outputsByName.Contains(name);
}
//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<VariableInstance> >& 
VariationInstance::GetHiddens() const
{
    return this->hidden;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariableInstance>& 
VariationInstance::GetHidden( const Util::String& name ) const
{
    n_assert(this->hiddenByName.Contains(name));
    return this->hiddenByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
VariationInstance::HasHidden( const Util::String& name ) const
{
    return this->hiddenByName.Contains(name);
}
} // namespace Nody
//------------------------------------------------------------------------------