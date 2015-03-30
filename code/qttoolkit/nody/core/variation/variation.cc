//------------------------------------------------------------------------------
//  variation.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variation.h"

namespace Nody
{
__ImplementClass(Nody::Variation, 'VART', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Variation::Variation() :
    simulationInteractive(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Variation::~Variation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Variation::Discard()
{
    // discard all types of variables
    IndexT i;
    for (i = 0; i < this->inputs.Size(); i++)
    {
        this->inputs[i]->Discard();
    }
    this->inputs.Clear();
    this->inputsByName.Clear();

    for (i = 0; i < this->outputs.Size(); i++)
    {
        this->outputs[i]->Discard();
    }
    this->outputs.Clear();
    this->outputsByName.Clear();

    for (i = 0; i < this->hidden.Size(); i++)
    {
        this->hidden[i]->Discard();
    }
    this->hidden.Clear();
    this->hiddenByName.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<VariationInstance> 
Variation::CreateInstance(const Ptr<Node>& node)
{
	Ptr<VariationInstance> instance = VariationInstance::Create();
	Ptr<Variation> thisPtr(this);
	instance->Setup(thisPtr, node);
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
void 
Variation::AddInput( const Ptr<Variable>& var )
{
	n_assert(this->inputs.FindIndex(var) == InvalidIndex);
	n_assert(var->GetIOFlag() == Variable::Input);
	this->inputs.Append(var);
    this->inputsByName.Add(var->GetName(), var);
}

//------------------------------------------------------------------------------
/**
*/
void 
Variation::AddOutput( const Ptr<Variable>& var )
{
	n_assert(this->outputs.FindIndex(var) == InvalidIndex);
	n_assert(var->GetIOFlag() == Variable::Output);
	this->outputs.Append(var);
    this->outputsByName.Add(var->GetName(), var);
}

//------------------------------------------------------------------------------
/**
*/
void 
Variation::AddHidden( const Ptr<Variable>& var )
{
    n_assert(this->hidden.FindIndex(var) == InvalidIndex);
    n_assert(var->GetIOFlag() == Variable::Hidden);
    this->hidden.Append(var);
    this->hiddenByName.Add(var->GetName(), var);
}

} // namespace Nody