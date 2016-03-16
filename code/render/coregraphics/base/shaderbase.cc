//------------------------------------------------------------------------------
//  shaderbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/shaderbase.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shader.h"
#include "coregraphics/shaderserver.h"
#include "shaderserverbase.h"

namespace Base
{
__ImplementClass(Base::ShaderBase, 'SHDB', Resources::Resource);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
ShaderBase::ShaderBase() :
    inBeginUpdate(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderBase::~ShaderBase()
{
    n_assert(0 == this->shaderInstances.Size());
    n_assert(this->variations.IsEmpty());
    n_assert(this->variables.IsEmpty());
    n_assert(this->variablesByName.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::Unload()
{
    n_assert(0 == this->shaderInstances.Size());
    this->variations.Clear();
    IndexT i;
    for (i = 0; i < this->variables.Size(); i++)
    {
        this->variables[i]->Cleanup();
    }
    this->variables.Clear();
    this->variablesByName.Clear();
    Resource::Unload();
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

//------------------------------------------------------------------------------
/**
*/
bool
ShaderBase::SelectActiveVariation(CoreGraphics::ShaderFeature::Mask featureMask)
{
    IndexT i = this->variations.FindIndex(featureMask);
    if (InvalidIndex != i)
    {
        const Ptr<ShaderVariation>& shdVar = this->variations.ValueAtIndex(i);
        const Ptr<ShaderServer>& shdSrv = ShaderServer::Instance();
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
            this->GetResourceId().Value());
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::BeginUpdate()
{
    n_assert(!this->inBeginUpdate);
    this->inBeginUpdate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::EndUpdate()
{
    n_assert(this->inBeginUpdate);
    this->inBeginUpdate = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::Apply()
{
    n_assert(this->activeVariation.isvalid());
    this->activeVariation->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBase::Commit()
{
    n_assert(this->activeVariation.isvalid());
    this->activeVariation->Commit();
}

} // namespace Base