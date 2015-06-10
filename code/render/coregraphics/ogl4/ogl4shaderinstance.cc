//------------------------------------------------------------------------------
//  ogl4shaderinstance.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4shaderinstance.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shader.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shadervariation.h"
#include "coregraphics/shaderserver.h"
#include "resources/resourcemanager.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4ShaderInstance, 'D1SI', Base::ShaderInstanceBase);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderInstance::OGL4ShaderInstance() :
    inWireframe(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderInstance::~OGL4ShaderInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    This method is called by Shader::CreateInstance() to setup the 
    new shader instance.
*/
void
OGL4ShaderInstance::Setup(const Ptr<CoreGraphics::Shader>& origShader)
{
    n_assert(origShader.isvalid());
    const Ptr<OGL4Shader>& ogl4Shader = origShader.upcast<OGL4Shader>();

    // call parent class
    ShaderInstanceBase::Setup(origShader);

	// copy effect pointer
	this->effect = origShader->GetOGL4Effect();

    int programCount = this->effect->GetNumPrograms();
	for (int i = 0; i < programCount; i++)
	{
        // a shader variation in Nebula is equal to a program object in AnyFX
		Ptr<ShaderVariation> variation = ShaderVariation::Create();

        // get program object from shader subsystem
        AnyFX::EffectProgram* program = this->effect->GetProgramByIndex(i);

		if (program->IsValid())
		{
			variation->Setup(program);
			this->variations.Add(variation->GetFeatureMask(), variation);
		}		
	}

    int variableCount = this->effect->GetNumVariables();
	for (int i = 0; i < variableCount; i++)
	{
		// create new variable
		Ptr<ShaderVariable> var = ShaderVariable::Create();

		// get AnyFX variable
        AnyFX::EffectVariable* effectVar = this->effect->GetVariableByIndex(i);

		if (effectVar->IsActive())
		{
			// setup variable from AnyFX variable
			var->Setup(effectVar);
			this->variables.Append(var);
			this->variablesByName.Add(var->GetName(), var);
			this->variablesBySemantic.Add(var->GetSemantic(), var);
		}		
	}

    int varbufferCount = this->effect->GetNumVarbuffers();
	for (int i = 0; i < varbufferCount; i++)
	{
		// create new variable
		Ptr<ShaderVariable> var = ShaderVariable::Create();
		
		// get AnyFX variable
        AnyFX::EffectVarbuffer* effectBuf = this->effect->GetVarbufferByIndex(i);

		var->Setup(effectBuf);
		this->variables.Append(var);
		this->variablesByName.Add(var->GetName(), var);
		this->variablesBySemantic.Add(var->GetSemantic(), var);
	}

	// use the default variation as the standard one
	this->activeVariation = this->variations.ValueAtIndex(0);

}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderInstance::Reload( const Ptr<CoreGraphics::Shader>& origShader )
{
	n_assert(origShader.isvalid());
	// FIXME!!
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::Cleanup()
{
    ShaderInstanceBase::Cleanup();
	this->effect = 0;

	this->variations.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
OGL4ShaderInstance::SelectActiveVariation(ShaderFeature::Mask featureMask)
{
    if (ShaderInstanceBase::SelectActiveVariation(featureMask))
        return true;
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::OnLostDevice()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::OnResetDevice()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SizeT
OGL4ShaderInstance::Begin()
{
	ShaderInstanceBase::Begin();
	return 1;    
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::BeginPass(IndexT passIndex)
{
	n_assert(this->activeVariation.isvalid());
    ShaderInstanceBase::BeginPass(passIndex);
	this->activeVariation->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::Commit()
{
	n_assert(this->activeVariation.isvalid());
    ShaderInstanceBase::Commit(); 

	this->activeVariation->Commit();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderInstance::PostDraw()
{
    n_assert(this->activeVariation.isvalid());
    ShaderInstanceBase::PostDraw();

    this->activeVariation->PostDraw();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::EndPass()
{
	n_assert(this->activeVariation.isvalid());
    ShaderInstanceBase::EndPass();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderInstance::End()
{
	n_assert(this->activeVariation.isvalid());
    ShaderInstanceBase::End();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderInstance::SetWireframe(bool b)
{
    this->inWireframe = b;
    this->activeVariation->SetWireframe(b);
}

} // namespace OpenGL4

