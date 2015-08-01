//------------------------------------------------------------------------------
//  effectvarblock.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effectvarblock.h"
#include "internal/internaleffectvarblock.h"
#include "internal/internaleffectvariable.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectVarblock::EffectVarblock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectVarblock::~EffectVarblock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectVarblock::GetName() const
{
	return this->internalVarblock->GetName();
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectVarblock::GetSignature() const
{
    return this->internalVarblock->GetSignature();
}

//------------------------------------------------------------------------------
/**
*/
const size_t
EffectVarblock::GetSize() const
{
    return this->internalVarblock->size;
}

//------------------------------------------------------------------------------
/**
*/
const bool
EffectVarblock::IsActive() const
{
    return this->internalVarblock->masterBlock->active;
}

//------------------------------------------------------------------------------
/**
*/
eastl::vector<VarblockVariableBinding>
EffectVarblock::GetVariables() const
{
    eastl::vector<VarblockVariableBinding> vars;
    vars.resize(this->internalVarblock->variables.size());

    for (unsigned i = 0; i < this->internalVarblock->variables.size(); i++)
    {
        VarblockVariableBinding& binding = vars[i];
        InternalEffectVariable* var = this->internalVarblock->variables[i];
        binding.name = var->GetName();
        binding.size = var->byteSize;
        binding.arraySize = var->arraySize;
        binding.offset = *var->sharedByteOffset;
        if (binding.size * binding.arraySize > 0)
        {
            binding.value = new char[binding.size];
            memcpy(binding.value, var->currentValue, binding.size);
        }        
    }

    return vars;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectVarblock::SetBuffer(void* handle)
{
    this->internalVarblock->SetBuffer(handle);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVarblock::Commit()
{
	this->internalVarblock->Commit();
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVarblock::Discard()
{
	// discard internal varblock
	this->internalVarblock->Release();
	this->internalVarblock = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectVarblock::SetFlushManually(bool b)
{
	this->internalVarblock->SetFlushManually(b);
}

//------------------------------------------------------------------------------
/**
*/
void
EffectVarblock::FlushBuffer()
{
	this->internalVarblock->FlushBuffer();
}

} // namespace AnyFX
