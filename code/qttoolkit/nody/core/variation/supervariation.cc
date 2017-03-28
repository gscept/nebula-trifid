//------------------------------------------------------------------------------
//  supervariation.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "supervariation.h"

namespace Nody
{
__ImplementClass(Nody::SuperVariation, 'SUVA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SuperVariation::SuperVariation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SuperVariation::~SuperVariation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
SuperVariation::Discard()
{
    // discard all types of variables
    IndexT i;
    for (i = 0; i < this->inputs.Size(); i++)
    {
        this->inputs[i]->Discard();
    }
    this->inputs.Clear();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<SuperVariationInstance> 
SuperVariation::CreateInstance( const Ptr<Node>& node )
{
    Ptr<SuperVariationInstance> instance = SuperVariationInstance::Create();
    Ptr<SuperVariation> thisPtr(this);
    instance->Setup(thisPtr, node);
    return instance;
}

//------------------------------------------------------------------------------
/**
*/
void 
SuperVariation::AddInput( const Ptr<Variable>& var )
{
    n_assert(this->inputs.FindIndex(var) == InvalidIndex);
    n_assert(var->GetIOFlag() == Variable::Input);
    this->inputs.Append(var);
}

} // namespace Nody