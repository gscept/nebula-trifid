//------------------------------------------------------------------------------
//  effectsubroutine.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectsubroutine.h"
#include "internal/internaleffectsubroutine.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectSubroutine::EffectSubroutine()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectSubroutine::~EffectSubroutine()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectSubroutine::Discard()
{
    delete this->internalSubroutine;
}

//------------------------------------------------------------------------------
/**
*/
const std::string& 
EffectSubroutine::GetName() const
{
    return this->internalSubroutine->GetName();
}

//------------------------------------------------------------------------------
/**
*/
const EffectSubroutine::SubroutineType& 
EffectSubroutine::GetType() const
{
    return this->internalSubroutine->GetType();
}
} // namespace AnyFX