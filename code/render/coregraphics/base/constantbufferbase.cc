//------------------------------------------------------------------------------
//  constantbufferbase.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "constantbufferbase.h"
#include "coregraphics/shader.h"
#include "coregraphics/shadervariable.h"

namespace Base
{
__ImplementClass(Base::ConstantBufferBase, 'COBB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ConstantBufferBase::ConstantBufferBase() :
    isSetup(false),
    bufferIndex(0),
    sync(false),
    inUpdateSync(false),
    isDirty(false),
	numBuffers(DefaultNumBackingBuffers),
	size(0),
	stride(0),
    buffer(0),
	grow(8),
	baseOffset(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ConstantBufferBase::~ConstantBufferBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::Setup(const SizeT numBackingBuffers)
{
    n_assert(this->size > 0);
	n_assert(numBackingBuffers > 0);
	this->numBuffers = numBackingBuffers;

	// setup with X amount of free indices we can pick from
	IndexT i;
	for (i = 0; i < numBackingBuffers; i++)
	{
		this->freeIndices.Append(i);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::SetupFromBlockInShader(const Ptr<CoreGraphics::ShaderState>& shader, const Util::String& blockName, const SizeT numBackingBuffers)
{
	// implement in subclass, must call Setup at some point in implementation!
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ConstantBufferBase::AllocateInstance(SizeT numInstances /*= 1*/)
{
	n_assert(numInstances > 0);
	return 0;
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::FreeInstance(SizeT offset)
{
	n_assert(offset >= 0);
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::Discard()
{
    IndexT i;
    for (i = 0; i < this->variables.Size(); i++)
    {
        this->variables[i]->Cleanup();
    }
    this->variables.Clear();
    this->variablesByName.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::CycleBuffers()
{
	this->bufferIndex = (this->bufferIndex + 1) % this->numBuffers;
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::BeginUpdateSync()
{
    n_assert(!this->inUpdateSync);
    n_assert(this->size != 0);
    n_assert(0 != this->buffer);
    this->inUpdateSync = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::EndUpdateSync()
{
    n_assert(this->inUpdateSync);
    this->inUpdateSync = false;
    this->isDirty = false;
}

} // namespace Base