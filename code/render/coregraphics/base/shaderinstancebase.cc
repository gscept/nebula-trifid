//------------------------------------------------------------------------------
//  shaderinstancebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shader.h"
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
    n_assert(this->variables.IsEmpty());
    n_assert(this->variablesByName.IsEmpty());
    n_assert(this->variablesBySemantic.IsEmpty());
    n_assert(this->variations.IsEmpty());    
    n_assert(!this->activeVariation.isvalid());
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
	IndexT i;
	for (i = 0; i < this->variables.Size(); i++)
	{
		this->variables[i]->Cleanup();
	}
    this->variables.Clear();
    this->variablesByName.Clear();
    this->variablesBySemantic.Clear();
    this->variations.Clear();
    this->activeVariation = 0;
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
ShaderInstanceBase::Commit()
{
    n_assert(this->inBeginPass);
}

//------------------------------------------------------------------------------
/**
*/
void 
ShaderInstanceBase::PostDraw()
{
    n_assert(this->inBeginPass);
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
ShaderInstanceBase::SelectActiveVariation(CoreGraphics::ShaderFeature::Mask featureMask)
{
    n_assert(!this->inBegin);
    IndexT i = this->variations.FindIndex(featureMask);
    if (InvalidIndex != i)
    {
        const Ptr<ShaderVariation>& shdVar = this->variations.ValueAtIndex(i);
        if (shdVar != this->activeVariation)
        {
            this->activeVariation = shdVar;
            return true;
        }
    }
    else
    {
        n_error("Unknown shader variation '%s' in shader '%s'\n",
            ShaderServer::Instance()->FeatureMaskToString(featureMask).AsCharPtr(),
            this->originalShader->GetResourceId().Value());
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<CoreGraphics::ShaderVariable>&
ShaderInstanceBase::GetVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& s) const
{
    #if NEBULA3_DEBUG
    if (!this->HasVariableBySemantic(s))
    {
        n_warning("Invalid shader variable semantic '%s' in shader '%s'",
            s.Value(), this->originalShader->GetResourceId().Value());
    }
    #endif
    return this->variablesBySemantic[s];
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<CoreGraphics::ShaderVariable>&
ShaderInstanceBase::GetVariableByName(const CoreGraphics::ShaderVariable::Name& n) const
{
    #if NEBULA3_DEBUG
    if (!this->HasVariableByName(n))
    {
        n_error("Invalid shader variable name '%s' in shader '%s'",
            n.Value(), this->originalShader->GetResourceId().Value());
    }
    #endif
    return this->variablesByName[n];
}

//------------------------------------------------------------------------------
/**
*/
const CoreGraphics::ShaderIdentifier::Code&
ShaderInstanceBase::GetCode() const
{
    return this->originalShader->GetShaderCode();
}

} // namespace Base