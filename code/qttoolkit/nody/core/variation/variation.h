#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::Variation
    
    A variation represents the state of a node.
	It can also contain other sections such as a preamble (functions or definitions which should be before the actual code).

    It contains inputs, outputs and hidden variables.
    Inputs represents indata to the node state.
    Outputs represents outdata from the node state.
    Hidden represents stuff which are none of the above, for example globally declared variables.

    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "variationinstance.h"
#include "variable/variable.h"
#include "util/array.h"
namespace Nody
{

class Variation : public Core::RefCounted
{
	__DeclareClass(Variation);
public:
	/// constructor
	Variation();
	/// destructor
	virtual ~Variation();

	/// discard variation
	void Discard();

	/// creates variation instance with a pointer to the node in which the variation will belong
	virtual Ptr<VariationInstance> CreateInstance(const Ptr<Node>& node);

	/// adds an input variable to the variation
	void AddInput(const Ptr<Variable>& var);
	/// adds an output variable to the variation
	void AddOutput(const Ptr<Variable>& var);
    /// adds hidden variable to the variation
    void AddHidden(const Ptr<Variable>& var);

	/// sets the name of the variation
	void SetName(const Util::String& name);
	/// gets the name of the variation
	const Util::String& GetName() const;
	
	/// set description
	void SetDescription(const Util::String& description);
	/// get description
	const Util::String& GetDescription() const;

    /// set the variable which should be used for the simulation
    void SetSimulationValue(const Util::String& string);
    /// get the variable to be used for the simulation
    const Util::String& GetSimulationValue() const;
    /// set the command which should be used for simulation
    void SetSimulationCommand(const Util::String& string);
    /// get the command which should be used for simulation
    const Util::String& GetSimulationCommand() const;
    /// set the variation to simulate interactively
    void SetSimulationInteractive(bool b);
    /// returns if the simulation is interactive
    const bool IsSimulationInteractive() const;

	/// returns inputs of variation
	const Util::Array<Ptr<Variable>>& GetInputs() const;
    /// returns input by name
    const Ptr<Variable>& GetInput(const Util::String& key);
	/// returns outputs of variation
	const Util::Array<Ptr<Variable>>& GetOutputs() const;
    /// returns output by name
    const Ptr<Variable>& GetOutput(const Util::String& key);
    /// returns hidden variables
    const Util::Array<Ptr<Variable>>& GetHiddens() const;
    /// returns specific hidden variable by name
    const Ptr<Variable>& GetHidden(const Util::String& key);
private:

	Util::String name;
	Util::String description;

    Util::String simulationValue;
    Util::String simulationCommand;
    bool simulationInteractive;

	Util::Array<Ptr<Variable>> inputs;
    Util::Dictionary<Util::String, Ptr<Variable>> inputsByName;
	Util::Array<Ptr<Variable>> outputs;
    Util::Dictionary<Util::String, Ptr<Variable>> outputsByName;
    Util::Array<Ptr<Variable>> hidden;
    Util::Dictionary<Util::String, Ptr<Variable>> hiddenByName;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Variation::SetName( const Util::String& name )
{
	n_assert(name.IsValid());
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Variation::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Variation::SetDescription( const Util::String& description )
{
	this->description = description;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Variation::GetDescription() const
{
	return this->description;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Variation::SetSimulationValue(const Util::String& string)
{
    this->simulationValue = string;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Variation::GetSimulationValue() const
{
    return this->simulationValue;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Variation::SetSimulationCommand(const Util::String& string)
{
    this->simulationCommand = string;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
Variation::GetSimulationCommand() const
{
    return this->simulationCommand;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Variation::SetSimulationInteractive(bool b)
{
    this->simulationInteractive = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Variation::IsSimulationInteractive() const
{
    return this->simulationInteractive;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Variable> >& 
Variation::GetInputs() const
{
	return this->inputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variable>& 
Variation::GetInput( const Util::String& key )
{
    n_assert(this->inputsByName.Contains(key));
    return this->inputsByName[key];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Variable> >& 
Variation::GetOutputs() const
{
	return this->outputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variable>& 
Variation::GetOutput( const Util::String& key )
{
    n_assert(this->outputsByName.Contains(key));
    return this->outputsByName[key];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Variable>>& 
Variation::GetHiddens() const
{
    return this->hidden;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Variable>& 
Variation::GetHidden( const Util::String& key )
{
    n_assert(this->hiddenByName.Contains(key));
    return this->hiddenByName[key];
}

} // namespace Nody
//------------------------------------------------------------------------------