//------------------------------------------------------------------------------
//  link.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "node/node.h"
#include "link.h"
#include "variable/variableinstance.h"

namespace Nody
{
__ImplementClass(Nody::Link, 'LINK', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Link::Link() : 
	from(0),
	to(0),
	graphics(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Link::~Link()
{
	this->from = 0;
	this->to = 0;
	this->graphics = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
Link::Discard()
{
	n_assert(this->from.isvalid());
	n_assert(this->to.isvalid());

    // discard simulation
    this->to->GetNode()->InvalidateSimulation();

	this->from->GetNode()->RemoveLink(this);
	this->to->GetNode()->RemoveLink(this);
    this->from->RemoveOutLink(this);
    this->to->UnsetInLink();

	this->from = 0;
	this->to = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
Link::GenerateGraphics()
{
	n_assert(!this->graphics.isvalid());
	this->graphics = LinkGraphics::Create();
	this->graphics->link = this;
	this->graphics->Generate();
}

//------------------------------------------------------------------------------
/**
*/
void 
Link::DestroyGraphics()
{
	n_assert(this->graphics.isvalid());
	this->graphics->Destroy();
	this->graphics = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
Link::LinkVariables(const Ptr<VariableInstance>& from, const Ptr<VariableInstance>& to)
{
	n_assert(from.isvalid());
	n_assert(to.isvalid());

	// if we can connect them, we do so and return true
	if (VariableInstance::CanConnect(from, to))
	{
		// set link pointers
		from->AddOutLink(this);
		to->SetInLink(this);

		// add link to parent nodes
		from->GetNode()->AddLink(this);
		to->GetNode()->AddLink(this);

		// copy pointers to endpoints of link
		this->from = from;
		this->to = to;

        // perform simulation
		from->GetNode()->Simulate();
        to->GetNode()->Simulate();

		return true;
	}

	// otherwise we return false :)
	return false;
}

} // namespace Nody