//------------------------------------------------------------------------------
//  supervariationinstance.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "supervariationinstance.h"
#include "variable/variable.h"
#include "node/node.h"

namespace Nody
{
__ImplementClass(Nody::SuperVariationInstance, 'SUVI', Core::RefCounted);
__ImplementSingleton(Nody::SuperVariationInstance);

//------------------------------------------------------------------------------
/**
*/
SuperVariationInstance::SuperVariationInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SuperVariationInstance::~SuperVariationInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
SuperVariationInstance::Setup( const Ptr<SuperVariation>& superVariation, const Ptr<Node>& node )
{
    n_assert(superVariation.isvalid());
    n_assert(node.isvalid());
    this->node = node;
    this->superVariation = superVariation;

    // get inputs, create instances and append them to this instance
    const Util::Array<Ptr<Variable> >& origInputs = this->superVariation->GetInputs();
    IndexT i;
    for (i = 0; i < origInputs.Size(); i++)
    {
        const Ptr<Variable>& origInput = origInputs[i];
        Ptr<VariableInstance> varInst = origInput->CreateInstance();
        varInst->SetNode(node);
        this->inputs.Append(varInst);
        this->inputsByName.Add(origInput->GetName(), varInst);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
SuperVariationInstance::Discard()
{
    IndexT i;
    for (i = 0; i < this->inputs.Size(); i++)
    {
        this->inputs[i]->Discard();
        this->inputs[i]->DestroyGraphics();
    }
    this->inputs.Clear();
    this->inputsByName.Clear();
}
} // namespace Nody