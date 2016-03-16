//------------------------------------------------------------------------------
//  simulation.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "simulation.h"
#include "link/link.h"
#include "variable/variableinstance.h"
#include "node/node.h"
#include "scripting/arg.h"
#include <QPixmap>

namespace Nody
{
__ImplementClass(Nody::Simulation, 'SIMU', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Simulation::Simulation() :
    command(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Simulation::~Simulation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Simulation::Setup(const Ptr<Nody::Node>& node)
{
    n_assert(node.isvalid());
    this->node = node;
}

//------------------------------------------------------------------------------
/**
*/
void 
Simulation::Discard()
{
    this->node = 0;
    this->command = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
Simulation::Run(const SimResult& result)
{
    n_assert(this->node.isvalid());

    // setup arguments
    const Util::Array<Ptr<VariableInstance>>& variables = this->node->GetVariation()->GetInputs();

    // get arguments and assert count
    Scripting::ArgsBlock& args = this->command->Arguments();
    n_assert(variables.Size() <= args.GetNumArgs());

    IndexT i;
    for (i = 0; i < variables.Size(); i++)
    {
        const Ptr<VariableInstance>& var = variables[i];
        const Ptr<Link>& link = var->GetInLink();
        if (link.isvalid())
        {
            const Ptr<Node>& otherNode = link->GetFromVariable()->GetNode();
            const SimResult& img = otherNode->GetSimulationResult();
            
            // set argument
            args.ArgValue(i).SetVoidPtr((void*)&img);
        }
        else
        {
            // input is unused, stop simulation
            return;
        }
    }

    // set result argument
    args.ArgValue("output").SetVoidPtr((void*)&result);
    this->command->OnExecute();
}

} // namespace Nody