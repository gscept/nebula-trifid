//------------------------------------------------------------------------------
//  shaderbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/shaderbase.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shader.h"

namespace Base
{
__ImplementClass(Base::ShaderBase, 'SHDB', Resources::Resource);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ShaderBase::ShaderBase() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderBase::~ShaderBase()
{
    n_assert(0 == this->shaderInstances.Size());
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ShaderInstance>
ShaderBase::CreateShaderInstance()
{
    Ptr<ShaderInstance> newInst = ShaderInstance::Create();
    Ptr<ShaderBase> thisPtr(this);
    newInst->Setup(thisPtr.downcast<Shader>());
    this->shaderInstances.Append(newInst);
    return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::DiscardShaderInstance(const Ptr<ShaderInstance>& inst)
{
    inst->Cleanup();
    IndexT i = this->shaderInstances.FindIndex(inst);
    n_assert(InvalidIndex != i);
    this->shaderInstances.EraseIndex(i);
}

} // namespace Base