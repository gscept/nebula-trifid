//------------------------------------------------------------------------------
//  variationinstance.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variation.h"
#include "variable/variable.h"
#include "variationinstance.h"
#include "node/node.h"


namespace Nody
{
__ImplementClass(Nody::VariationInstance, 'VAIN', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
VariationInstance::VariationInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VariationInstance::~VariationInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
VariationInstance::Setup( const Ptr<Variation>& variation, const Ptr<Node>& node )
{
	n_assert(variation.isvalid());
	n_assert(node.isvalid());
	this->node = node;
	this->variation = variation;

	// get inputs, create instances and append them to this instance
	const Util::Array<Ptr<Variable> >& origInputs = this->variation->GetInputs();
	IndexT i;
	for (i = 0; i < origInputs.Size(); i++)
	{
		const Ptr<Variable>& origInput = origInputs[i];
		Ptr<VariableInstance> varInst = origInput->CreateInstance();
		varInst->SetNode(node);
		this->inputs.Append(varInst);
		this->inputsByName.Add(origInput->GetName(), varInst);
	}

	// do the same for outputs
	const Util::Array<Ptr<Variable> >& origOutputs = this->variation->GetOutputs();
	for (i = 0; i < origOutputs.Size(); i++)
	{
		const Ptr<Variable>& origOutput = origOutputs[i];
		Ptr<VariableInstance> varInst = origOutput->CreateInstance();
		varInst->SetNode(node);
		this->outputs.Append(varInst);
		this->outputsByName.Add(origOutput->GetName(), varInst);
	}

    // also do for hidden variables
    const Util::Array<Ptr<Variable> >& origHiddens = this->variation->GetHiddens();
    for (i = 0; i < origHiddens.Size(); i++)
    {
        const Ptr<Variable>& origHidden = origHiddens[i];
        Ptr<VariableInstance> varInst = origHidden->CreateInstance();
        varInst->SetNode(node);
        this->hidden.Append(varInst);
        this->hiddenByName.Add(origHidden->GetName(), varInst);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
VariationInstance::Discard()
{
	IndexT i;

	// discard input instances
	for (i = 0; i < this->inputs.Size(); i++)
	{
		this->inputs[i]->Discard();
		this->inputs[i]->DestroyGraphics();
	}
	this->inputs.Clear();
    this->inputsByName.Clear();

	// discard output instances
	for (i = 0; i < this->outputs.Size(); i++)
	{
		this->outputs[i]->Discard();
		this->outputs[i]->DestroyGraphics();
	}
	this->outputs.Clear();
    this->outputsByName.Clear();

    // discard hidden instances
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
const Ptr<Variation>& 
VariationInstance::GetOriginalVariation() const
{
	return this->variation;
}


} // namespace Nody