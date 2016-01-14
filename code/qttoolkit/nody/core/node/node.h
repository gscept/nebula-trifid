#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::NodeInstance
    
    Implements a generic node which can be connected to other nodes
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "node/graphics/nodegraphics.h"
#include "variation/variation.h"
#include "variation/supervariation.h"
#include "scripting/command.h"
#include "simulation/simulation.h"

namespace Nody
{
class Link;
class Node : public Core::RefCounted
{
	__DeclareClass(Node);
public:

    enum Flags
    {
        NoFlags = 0,
        NoMove = 1 << 0,
        NoDelete = 1 << 1,

        AllNodeFlags = (1 << 2) - 1
    };

	/// constructor
	Node();
	/// destructor
	virtual ~Node();

	/// sets up the node from a variation
	virtual void Setup(const Ptr<Variation>& variation);
    /// sets up a node from a super variation
    virtual void Setup(const Ptr<SuperVariation>& superVariation);
	/// discards the node 
	virtual void Discard();

    /// set node flags
    void SetFlags(const uint flags);
    /// get node flags
    const uint GetFlags() const;

	/// adds a link to a node
	void AddLink(const Ptr<Link>& link);
	/// unlink this node from another
	void RemoveLink(const Ptr<Link>& link);

    /// set value, this will also set the simulation value if the key matches the value
    virtual void SetValue(const Util::String& key, const Util::Variant& value);
    /// get value by name
    const Util::Variant& GetValue(const Util::String& key) const;
    /// get the value type of a value
    const VarType& GetValueType(const Util::String& key);
    /// returns true if node has a value for this key
    const bool HasValue(const Util::String& key) const;
    /// get dictionary of keys and values
    const Util::Dictionary<Util::String, Util::Variant>& GetValueDictionary() const;

    /// return simulation key-value pair for writing and reading
    const Util::KeyValuePair<Util::Variant, VarType>& GetSimulationKeyValue() const;

	/// gets the nodes variation instance
	const Ptr<VariationInstance>& GetVariation() const;
    /// gets the nodes super variation instance
    const Ptr<SuperVariationInstance>& GetSuperVariation() const;

    /// returns true if this node is the main node
    const bool IsSuperNode() const;

	/// gets list of links
	const Util::Array<Ptr<Link> >& GetLinks() const;

	/// returns pointer to graphics
	const Ptr<NodeGraphics>& GetGraphics() const;
	/// generate graphics
	virtual void GenerateGraphics();
	/// destroy graphics
	virtual void DestroyGraphics();

    /// return simulation object
    const Ptr<Simulation>& GetSimulation() const;
    /// return simulation result
    const SimResult& GetSimulationResult() const;
    /// performs simulation, call this function to visualize the current node value
    virtual void Simulate();
    /// reset simulation, this may happen if we disconnect a node
    virtual void InvalidateSimulation();

protected:
    uint flags;
	Ptr<Node> node;
    Ptr<NodeGraphics> graphics;
    Util::Array<Ptr<Link> > links;
    Ptr<SuperVariationInstance> superVariation;
	Ptr<VariationInstance> variation;
    Util::Dictionary<Util::String, Util::Variant> values;
    Util::Dictionary<Util::String, VarType> valueTypes;
	Util::Dictionary<Util::String, uint> valueFlags;

    Ptr<Simulation> simulation;
    Util::KeyValuePair<Util::Variant, VarType> simulationValue;
    SimResult simulationResult;
	bool simulatedThisFrame;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<VariationInstance>& 
Node::GetVariation() const
{
	return this->variation;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SuperVariationInstance>& 
Node::GetSuperVariation() const
{  
    return this->superVariation;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Node::IsSuperNode() const
{
    return this->superVariation.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<NodeGraphics>& 
Node::GetGraphics() const
{
	return this->graphics;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Node::SetFlags( const uint flags )
{
    this->flags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint 
Node::GetFlags() const
{
    return this->flags;
}


//------------------------------------------------------------------------------
/**
*/
inline const Util::Variant& 
Node::GetValue( const Util::String& key ) const
{
    n_assert(this->values.Contains(key));
    return this->values[key];
}

//------------------------------------------------------------------------------
/**
*/
inline const VarType& 
Node::GetValueType(const Util::String& key)
{
    n_assert(this->valueTypes.Contains(key));
    return this->valueTypes[key];
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Node::HasValue( const Util::String& key ) const
{
    return this->values.Contains(key);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, Util::Variant>& 
Node::GetValueDictionary() const
{
    return this->values;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::KeyValuePair<Util::Variant, VarType>& 
Node::GetSimulationKeyValue() const
{
    return this->simulationValue;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Link> >& 
Node::GetLinks() const
{
	return this->links;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Simulation>& 
Node::GetSimulation() const
{
    return this->simulation;
}

//------------------------------------------------------------------------------
/**
*/
inline const SimResult& 
Node::GetSimulationResult() const
{
    return this->simulationResult;
}


} // namespace Nody
//------------------------------------------------------------------------------