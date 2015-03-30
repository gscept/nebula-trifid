//------------------------------------------------------------------------------
//  internaleffectvarbuffer.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectvarbuffer.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarbuffer::InternalEffectVarbuffer() :
	currentBufferHandle(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarbuffer::~InternalEffectVarbuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarbuffer::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarbuffer::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master)
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Apply()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Commit()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Activate( InternalEffectProgram* program )
{
	// override in subclass
}

} // namespace AnyFX