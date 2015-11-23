//------------------------------------------------------------------------------
//  constantbufferbase.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "constantbufferbase.h"
#include "coregraphics/shaderinstance.h"
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
    buffer(0)
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
ConstantBufferBase::Setup()
{
    n_assert(this->size > 0);
}

//------------------------------------------------------------------------------
/**
*/
void
ConstantBufferBase::SetupFromBlockInShader(const Ptr<CoreGraphics::ShaderInstance>& shader, const Util::String& blockName)
{
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
    this->bufferIndex = (this->bufferIndex + 1) % NumBuffers;
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