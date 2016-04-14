#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::Simulation
    
    The simulation object encapsulates a simulation function which is used to 
    visualize a node tree.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "scripting/command.h"
#include "variable/vartype.h"
#include "simresult.h"
#include <QImage>

namespace Nody
{
class Node;
class Simulation : public Core::RefCounted
{
	__DeclareClass(Simulation);
public:
	/// constructor
	Simulation();
	/// destructor
	virtual ~Simulation();

    /// setup simulation on node
    void Setup(const Ptr<Nody::Node>& node);
    /// discard simulation
    void Discard();
    /// run simulation, returns produced result
    void Run(const SimResult& result);

    /// returns if simulation is valid, this basically tells if we can perform Run
    const bool IsValid() const;

    /// set type
    void SetType(const Nody::VarType& type);
    /// get type
    const Nody::VarType GetType() const;

    /// set command
    void SetCommand(const Ptr<Scripting::Command>& cmd);
    /// get command
    const Ptr<Scripting::Command>& GetCommand() const;

private:
    Ptr<Nody::Node> node;
    Nody::VarType type;
    Ptr<Scripting::Command> command;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Simulation::IsValid() const
{
    return this->command.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Simulation::SetType(const Nody::VarType& type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Nody::VarType 
Simulation::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Simulation::SetCommand(const Ptr<Scripting::Command>& cmd)
{
    n_assert(cmd.isvalid());
    this->command = cmd;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Scripting::Command>& 
Simulation::GetCommand() const
{
    return this->command;
}

} // namespace Nody
//------------------------------------------------------------------------------