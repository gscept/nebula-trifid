//------------------------------------------------------------------------------
//  node.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "node.h"
#include "variation/variationinstance.h"
#include "graphics/nodegraphics.h"
#include "link/link.h"
#include "scripting/argsblock.h"
#include "scripting/arg.h"
#include "scripting/scriptserver.h"

namespace Nody
{
__ImplementClass(Nody::Node, 'NOIN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Node::Node() :
    flags(NoFlags),
	node(0),
    superVariation(0),
	variation(0),
	graphics(0),
    simulation(0),
	simulatedThisFrame(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Node::~Node()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Setup(const Ptr<Variation>& variation)
{
	n_assert(variation.isvalid());
	n_assert(!this->variation.isvalid());

	// create instance of variation
	this->variation = variation->CreateInstance(this);

    // create simulation object
    this->simulation = Simulation::Create();
    this->simulation->Setup(this);

    // get simulation command
    Util::String cmd = this->variation->GetOriginalVariation()->GetSimulationCommand();
    const Ptr<Scripting::ScriptServer> scriptServer = Scripting::ScriptServer::Instance();
    if (scriptServer->HasCommand(cmd))
    {
        this->simulation->SetCommand(scriptServer->GetCommandByName(cmd));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Setup(const Ptr<SuperVariation>& superVariation)
{
    n_assert(superVariation.isvalid());
    n_assert(!this->superVariation.isvalid());

    // create instance of supervariaton
    this->superVariation = superVariation->CreateInstance(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
Node::Discard()
{
	n_assert(this->variation.isvalid() || this->superVariation.isvalid());
    this->values.Clear();
    this->valueTypes.Clear();

    if (this->simulation.isvalid())
    {
        this->simulation->Discard();
        this->simulation = 0;
    }    

    if (this->variation.isvalid())
    {
	    this->variation->Discard();
	    this->variation = 0;
    }
    else
    {
        this->superVariation->Discard();
        this->superVariation = 0;
    }
}

//------------------------------------------------------------------------------
/**
	Adds a link to node, but makes sure the link actually connects with this node
*/
void 
Node::AddLink( const Ptr<Link>& link )
{
	n_assert(link.isvalid());
	this->links.Append(link);
}

//------------------------------------------------------------------------------
/**
*/
void 
Node::RemoveLink( const Ptr<Link>& link )
{
	n_assert(link.isvalid());
	this->links.EraseIndex(this->links.FindIndex(link));
}

//------------------------------------------------------------------------------
/**
	Override in subclass.
	We probably want graphics to be per-application specific...
*/
void 
Node::GenerateGraphics()
{
    n_assert(this->graphics.isvalid());

    // setup default value
    this->graphics->OnSimulate(this->simulationValue.Key(), this->simulationValue.Value(), this->simulationResult);

    // run simulation once the graphics is setup
    this->Simulate();
}

//------------------------------------------------------------------------------
/**
*/
void 
Node::DestroyGraphics()
{
	n_error("Node::DestroyGraphics() called!\n");
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetValue(const Util::String& key, const Util::Variant& value)
{
	n_assert(this->values.Contains(key));
	this->values[key] = value;

    // set simulation value
    if (key == this->variation->GetOriginalVariation()->GetSimulationValue())
    {
        this->simulationValue = Util::KeyValuePair<Util::Variant, Nody::VarType>(value, this->simulationValue.Value());
		this->Simulate();
    }
}

//------------------------------------------------------------------------------
/**
    Override in subclass, this simulation method doesn't run any simulation
*/
void 
Node::Simulate()
{
	if (this->simulatedThisFrame)
	{
		this->simulatedThisFrame = false;
		return;
	}

    if (this->IsSuperNode())
    {
        // hmm, maybe apply final result to preview object?
    }
    else
    {
        n_assert(this->graphics.isvalid());
        if (this->simulation->IsValid())
        {
            this->simulation->Run(this->simulationResult);
            this->graphics->OnSimulate(this->simulationResult);
        }
        else
        {
            this->graphics->OnSimulate(this->simulationValue.Key(), this->simulationValue.Value(), this->simulationResult);
        }
		this->simulatedThisFrame = true;

        // go to output links and update them
        const Util::Array<Ptr<Link>>& links = this->GetLinks();
        IndexT i;
        for (i = 0; i < links.Size(); i++)
        {
            const Ptr<Link>& link = links[i];
            const Ptr<Node>& node = link->GetToVariable()->GetNode();
            if (node.get() != this)
            {
                node->Simulate();
            }
        }
    } 
       
}

//------------------------------------------------------------------------------
/**
*/
void 
Node::InvalidateSimulation()
{
    this->simulationResult.ResetDefault();
    this->graphics->OnSimulate(this->simulationResult);
}
} // namespace Nody