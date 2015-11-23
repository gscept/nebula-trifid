//------------------------------------------------------------------------------
//  shaderinstancebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/shader.h"
#include "coregraphics/base/shaderinstancebase.h"
#include "coregraphics/shadervariation.h"
#include "coregraphics/shaderserver.h"

namespace Base
{
__ImplementClass(Base::ShaderInstanceBase, 'SIBS', Core::RefCounted);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ShaderInstanceBase::ShaderInstanceBase() :
    inBegin(false),
    inBeginPass(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderInstanceBase::~ShaderInstanceBase()
{
    // check if Discard() has been called...
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderInstanceBase::IsValid() const
{
    return this->originalShader.isvalid();
}

//------------------------------------------------------------------------------
/**
    This method must be called when the object is no longer needed
    for proper cleanup.
*/
void
ShaderInstanceBase::Discard()
{
    n_assert(this->IsValid());
    this->originalShader->DiscardShaderInstance((ShaderInstance*)this);
}

//------------------------------------------------------------------------------
/**
    Override this method in an API-specific subclass to setup the
    shader instance, and call the parent class for proper setup.
*/
void
ShaderInstanceBase::Setup(const Ptr<Shader>& origShader)
{
    n_assert(!this->IsValid());
    this->originalShader = origShader;
}

//------------------------------------------------------------------------------
/**
    Override this method in an API-specific subclass to undo the
    setup in OnInstantiate(), then call parent class to finalize
    the cleanup.
*/
void
ShaderInstanceBase::Cleanup()
{
    n_assert(this->IsValid());
    this->originalShader = 0;
}

//------------------------------------------------------------------------------
/**
*/
SizeT
ShaderInstanceBase::Begin()
{
    n_assert(!this->inBegin);
    n_assert(!this->inBeginPass);
    this->inBegin = true;
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderInstanceBase::BeginPass(IndexT passIndex)
{
    n_assert(this->inBegin);
    n_assert(!this->inBeginPass);
    this->inBeginPass = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderInstanceBase::Apply()
{
    IndexT i;
    for (i = 0; i < this->variableInstances.Size(); i++)
    {
        this->variableInstances[i]->Apply();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderInstanceBase::Commit()
{
    // also commit original shader
    this->originalShader->GetActiveVariation()->Commit();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderInstanceBase::PostDraw()
{
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderInstanceBase::EndPass()
{
    n_assert(this->inBeginPass);
    this->inBeginPass = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderInstanceBase::End()
{
    n_assert(this->inBegin);
    n_assert(!this->inBeginPass);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderInstanceBase::SelectActiveVariation(CoreGraphics::ShaderFeature::Mask mask)
{
    return this->originalShader->SelectActiveVariation(mask);
}

//------------------------------------------------------------------------------
/**
*/
const CoreGraphics::ShaderIdentifier::Code&
ShaderInstanceBase::GetCode() const
{
    return this->originalShader->GetCode();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<CoreGraphics::ShaderVariableInstance>
ShaderInstanceBase::CreateVariableInstance(const Base::ShaderVariableBase::Name& n)
{
    n_assert(!this->variableInstancesByName.Contains(n));
    Ptr<CoreGraphics::ShaderVariableInstance> instance = this->originalShader->GetVariableByName(n)->CreateInstance();
    this->variableInstances.Append(instance);
    this->variableInstancesByName.Add(n, instance);
    return instance;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<CoreGraphics::ShaderVariableInstance>&
ShaderInstanceBase::GetVariableInstance(const Base::ShaderVariableBase::Name& n)
{
    n_assert(this->variableInstancesByName.Contains(n));
    return this->variableInstancesByName[n];
}

} // namespace Base