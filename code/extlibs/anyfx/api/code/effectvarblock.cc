//------------------------------------------------------------------------------
//  effectvarblock.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effectvarblock.h"
#include "internal/internaleffectvarblock.h"

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
const std::string& 
EffectVarblock::GetName() const
{
	return this->internalVarblock->GetName();
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
