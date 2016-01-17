//------------------------------------------------------------------------------
//  effectvarbuffer.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectvarbuffer.h"
#include "internal/internaleffectvarbuffer.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer::EffectVarbuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectVarbuffer::~EffectVarbuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVarbuffer::Discard()
{
    delete this->internalVarbuffer;
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVarbuffer::Commit()
{

}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectVarbuffer::GetName() const
{
    return this->internalVarbuffer->GetName();
}

//------------------------------------------------------------------------------
/**
*/
const bool
EffectVarbuffer::IsActive() const
{
	return this->internalVarbuffer->masterBuffer->active;
}

//------------------------------------------------------------------------------
/**
*/
void*
EffectVarbuffer::GetHandle() const
{
	return *this->internalVarbuffer->bufferHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectVarbuffer::SetBuffer(void* handle)
{
	this->internalVarbuffer->SetBuffer(handle);
}

} // namespace AnyFX