#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::ToneMappingAlgorithm
    
    Performs true HDR using eye-adaptation and tone mapping.
	This would be much nicer to compute using a compute shader.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "algorithmbase.h"
#include "renderutil/drawfullscreenquad.h"

namespace CoreGraphics
{
	class ShaderInstance;
	class ShaderVariable;
	class RenderTarget;
	class Texture;
}

namespace Algorithm
{
class ToneMappingAlgorithm : public AlgorithmBase
{
	__DeclareClass(ToneMappingAlgorithm);
public:
	/// constructor
	ToneMappingAlgorithm();
	/// destructor
	virtual ~ToneMappingAlgorithm();

	/// sets up algorithm
	void Setup();
	/// discards algorithm
	void Discard();

	/// executes algorithm
	void Execute();

	/// called whenever we get a display resize event
	void OnDisplayResized(SizeT width, SizeT height);
	
	/// handles messages
	bool HandleMessage(const Ptr<Messaging::Message>& msg);

private:
	/// enables tone mapping
	void Enable();
	/// disables tone mapping
	void Disable();

	RenderUtil::DrawFullScreenQuad quad;
	Ptr<CoreGraphics::ShaderInstance> averageLum;
	Ptr<CoreGraphics::ShaderInstance> downscale;
	Ptr<CoreGraphics::ShaderInstance> copy;

	Ptr<CoreGraphics::ShaderVariable> previousLuminanceVar;
	Ptr<CoreGraphics::ShaderVariable> downscaleBufferVar;
	Ptr<CoreGraphics::ShaderVariable> colorBufferVar;
	Ptr<CoreGraphics::ShaderVariable> copyBufferVar;
	Ptr<CoreGraphics::ShaderVariable> timeDiffVar;

	Ptr<CoreGraphics::RenderTarget> output;
	Ptr<CoreGraphics::RenderTarget> color;
	Ptr<CoreGraphics::RenderTarget> outputCopy;

	Ptr<CoreGraphics::Texture> downscaledColor;
}; 

__RegisterClass(ToneMappingAlgorithm);
} // namespace Algorithm
//------------------------------------------------------------------------------