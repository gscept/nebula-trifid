//------------------------------------------------------------------------------
//  tonemappingalgorithm.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tonemappingalgorithm.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "algorithm/algorithmprotocol.h"
#include "framesync/framesynctimer.h"

using namespace CoreGraphics;
namespace Algorithm
{
__ImplementClass(Algorithm::ToneMappingAlgorithm, 'TOAL', Algorithm::AlgorithmBase);

//------------------------------------------------------------------------------
/**
*/
ToneMappingAlgorithm::ToneMappingAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ToneMappingAlgorithm::~ToneMappingAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::Setup()
{
	n_assert(this->inputs.Size() == 1);
	n_assert(this->outputNames.Size() == 1);
	n_assert(this->inputs[0].isvalid());

	AlgorithmBase::Setup();

	// setup output render target, this is the 1x1 luminance render target
	this->output = RenderTarget::Create();
	this->output->SetResolveTextureResourceId(this->outputNames[0]);
	this->output->SetWidth(1);
	this->output->SetHeight(1);
	this->output->SetAntiAliasQuality(AntiAliasQuality::None);
	this->output->SetColorBufferFormat(PixelFormat::G16R16F);
	this->output->Setup();

	// create output copy which will be used to smoothly tone between luminance values
	this->outputCopy = RenderTarget::Create();
	this->outputCopy->SetResolveTextureResourceId("ToneMappingLuminanceCopy");
	this->outputCopy->SetWidth(1);
	this->outputCopy->SetHeight(1);
	this->outputCopy->SetAntiAliasQuality(AntiAliasQuality::None);
	this->outputCopy->SetColorBufferFormat(PixelFormat::G16R16F);
	this->outputCopy->Setup();

	// setup  color render target, this has a fixed size
	this->color = RenderTarget::Create();
	this->color->SetResolveTextureResourceId("ToneMappingColor");
	this->color->SetWidth(512);
	this->color->SetHeight(512);
	this->color->SetMipMaps(8); // we need 8 mip levels to mip from 512x512 down to 2x2
	this->color->SetAntiAliasQuality(AntiAliasQuality::None);
	this->color->SetColorBufferFormat(this->inputs[0]->GetPixelFormat());
	this->color->Setup();

	// get  color texture
	this->downscaledColor = this->color->GetResolveTexture();

	// create shaders
	this->averageLum = ShaderServer::Instance()->GetShader("shd:averagelum");
    this->downscale = ShaderServer::Instance()->GetShader("shd:downscale");
    this->copy = ShaderServer::Instance()->GetShader("shd:copy");

	// get luminance variables
	this->colorBufferVar = this->averageLum->GetVariableByName("ColorSource");
	this->previousLuminanceVar = this->averageLum->GetVariableByName("PreviousLum");
	this->timeDiffVar = this->averageLum->GetVariableByName("TimeDiff");

	// get downscale variable
	this->downscaleBufferVar = this->downscale->GetVariableByName("ColorSource");	

	// get copy variable
	this->copyBufferVar = this->copy->GetVariableByName("CopyBuffer");

	// set luminance variables
	this->colorBufferVar->SetTexture(this->downscaledColor);
	this->previousLuminanceVar->SetTexture(this->outputCopy->GetResolveTexture());

	// set downscale variables
	this->downscaleBufferVar->SetTexture(this->inputs[0]);
	
	// set copy variable
	this->copyBufferVar->SetTexture(this->output->GetResolveTexture());	

	// setup fsq
	this->quad.Setup(this->inputs[0]->GetWidth(), this->inputs[0]->GetHeight());

	// setup default values for our render target
	this->output->SetClearColor(Math::float4(0.1f, 0, 0, 0));
	this->outputCopy->SetClearColor(Math::float4(0, 0, 0, 0));
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::Discard()
{
	AlgorithmBase::Discard();

	// discard variables
	this->downscaleBufferVar = 0;
	this->colorBufferVar = 0;

	// deref shaders
	this->averageLum = 0;
	this->downscale = 0;
	this->copy = 0;

	// just release texture, since they are just reference copies
	this->downscaledColor = 0;

	this->output->Discard();
	this->output = 0;

	this->color->Discard();
	this->color = 0;	

	this->outputCopy->Discard();
	this->outputCopy = 0;

	// discard FSQ
	this->quad.Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::Render()
{
	if (this->enabled)
	{
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shaderServer = ShaderServer::Instance();

		// copy buffer and downscale to 512x512
        this->downscale->Apply();
        renderDevice->BeginPass(this->color, this->downscale);
        this->downscale->BeginUpdate();
		this->copyBufferVar->SetTexture(this->inputs[0]);
        this->downscale->EndUpdate();
        this->downscale->Commit();
		this->quad.Draw();
		renderDevice->EndPass();

		// generate mips for the just rendered downscaled color
		this->downscaledColor->GenerateMipmaps();

        // apply shader
        this->averageLum->Apply();
        renderDevice->BeginPass(this->output, this->averageLum);

        // update time
        this->averageLum->BeginUpdate();
		Timing::Time time = FrameSync::FrameSyncTimer::Instance()->GetFrameTime();
		this->timeDiffVar->SetFloat((float)time); // adjust the constant multiplier to increase transition time, the higher the slower
        this->averageLum->EndUpdate();

		// now render shader which calculates the average luminance to the output
        this->averageLum->Commit();
		this->quad.Draw();
		renderDevice->EndPass();

        // now copy from the output to the input
        this->output->Copy(this->outputCopy);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::OnDisplayResized(SizeT width, SizeT height)
{
    // discard and setup fsq
    this->quad.Discard();
    this->quad.Setup(this->inputs[0]->GetWidth(), this->inputs[0]->GetHeight());
}

//------------------------------------------------------------------------------
/**
*/
bool 
ToneMappingAlgorithm::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(EnableToneMapping::Id))
	{
		Ptr<EnableToneMapping> rMsg = msg.downcast<EnableToneMapping>();
		this->SetEnabled(rMsg->GetEnabled());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::Enable()
{
	// do nothing
}

//------------------------------------------------------------------------------
/**
*/
void 
ToneMappingAlgorithm::Disable()
{
    // clear output
    this->output->Clear(RenderTarget::ClearColor);
}

} // namespace Algorithm