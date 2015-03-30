//------------------------------------------------------------------------------
//  OGL4ShaderVariation.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4shadervariation.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4ShaderVariation, 'D1SV', Base::ShaderVariationBase);

using namespace CoreGraphics;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderVariation::OGL4ShaderVariation() :
	program(0),
	usePatches(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderVariation::~OGL4ShaderVariation()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::Setup( AnyFX::EffectProgram* program )
{
	n_assert(0 == this->program);

	this->program = program;
	String mask = program->GetAnnotationString("Mask").c_str();
	String name = program->GetName().c_str();
	this->usePatches = program->SupportsTessellation();

	this->SetFeatureMask(ShaderServer::Instance()->FeatureStringToMask(mask));
	this->SetName(name);
	this->SetNumPasses(1);
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::Apply()
{
	n_assert(this->program);
	this->program->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::Commit()
{
	n_assert(this->program);
	this->program->Commit();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::PreDraw()
{
    n_assert(this->program);
    this->program->PreDraw();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::PostDraw()
{
    n_assert(this->program);
    this->program->PostDraw();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShaderVariation::SetWireframe(bool b)
{
    if (b)    this->program->GetRenderState()->SetFillMode(AnyFX::EffectRenderState::Line);
    else      this->program->GetRenderState()->Reset();
}
} // namespace OpenGL4
