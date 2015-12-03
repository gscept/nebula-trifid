//------------------------------------------------------------------------------
// ssralgorithm.cc
// (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ssralgorithm.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"

#define DivAndRoundUp(a, b) (a % b != 0) ? (a / b + 1) : (a / b)
using namespace CoreGraphics;
namespace Algorithm
{
__ImplementClass(Algorithm::SSRAlgorithm, 'SSRA', Algorithm::AlgorithmBase);
//------------------------------------------------------------------------------
/**
*/
SSRAlgorithm::SSRAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SSRAlgorithm::~SSRAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
SSRAlgorithm::Setup()
{
	n_assert(this->inputs.Size() == 5);

	AlgorithmBase::Setup();

	// setup output, it should be the last input
	this->output = this->inputs.Back();

	// setup shader
	this->ssrShader = ShaderServer::Instance()->GetShader("shd:ssr_cs");
	this->depthBuffer = this->ssrShader->GetVariableByName("DepthBuffer");
	this->colorBuffer = this->ssrShader->GetVariableByName("ColorBuffer");
	this->specularBuffer = this->ssrShader->GetVariableByName("SpecularBuffer");
	this->normalBuffer = this->ssrShader->GetVariableByName("NormalBuffer");
	this->emissiveBuffer = this->ssrShader->GetVariableByName("EmissiveBuffer");
	this->resolution = this->ssrShader->GetVariableByName("Resolution");
	this->invResolution = this->ssrShader->GetVariableByName("InvResolution");

	// update shader
	this->ssrShader->BeginUpdate();
	this->resolution->SetFloat2(Math::float2(float(this->output->GetWidth()), float(this->output->GetHeight())));
	this->invResolution->SetFloat2(Math::float2(1 / float(this->output->GetWidth()), 1 / float(this->output->GetHeight())));
	this->ssrShader->EndUpdate();

	// set textures
	this->depthBuffer->SetTexture(this->inputs[0]);
	this->colorBuffer->SetTexture(this->inputs[1]);
	this->specularBuffer->SetTexture(this->inputs[2]);
	this->normalBuffer->SetTexture(this->inputs[3]);
	this->emissiveBuffer->SetTexture(this->output);
}

//------------------------------------------------------------------------------
/**
*/
void
SSRAlgorithm::Discard()
{
	this->ssrShader = 0;
	this->depthBuffer = 0;
	this->colorBuffer = 0;
	this->specularBuffer = 0;
	this->normalBuffer = 0;
	this->emissiveBuffer = 0;
	this->resolution = 0;
	this->invResolution = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SSRAlgorithm::Render()
{
	// if enabled, render screen-space reflections (disabled for now)
	if (this->enabled && false)
	{
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shaderServer = ShaderServer::Instance();

#define TILE_WIDTH 32
		// calculate execution dimensions
		uint numGroupsX1 = DivAndRoundUp(this->output->GetWidth(), TILE_WIDTH);
		uint numGroupsX2 = this->output->GetWidth();
		uint numGroupsY1 = DivAndRoundUp(this->output->GetHeight(), TILE_WIDTH);
		uint numGroupsY2 = this->output->GetHeight();

		// run shader
		this->ssrShader->Apply();
		this->ssrShader->Commit();
		renderDevice->Compute(numGroupsX1, numGroupsY1, 1);
	}
}

} // namespace Algorithm