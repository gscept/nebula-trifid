#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::AOAlgorithm
    
    Performs horizon-based ambient occlusion.

	Inputs:
	
	Slot 0 - Depth Texture
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "algorithmbase.h"
#include "renderutil/drawfullscreenquad.h"
#include "coregraphics/shaderreadwritetexture.h"

// define this to use compute shaders for HBAO instead of pixel shader using a full screen quad
#define HBAO_COMPUTE 1


namespace CoreGraphics
{
	class ShaderInstance;
	class ShaderVariable;
	class RenderTarget;
    class ShaderFeature;
	class Texture;
}

namespace Algorithm
{
class AOAlgorithm : public AlgorithmBase
{
	__DeclareClass(AOAlgorithm);
public:
	/// constructor
	AOAlgorithm();
	/// destructor
	virtual ~AOAlgorithm();

	/// setup algorithm
	void Setup();
	/// discards resources
	void Discard();

	/// prepare deferred initialization
	void Begin();
	/// execute algorithm
	void Render();

	/// handle display resizing
	void OnDisplayResized(SizeT width, SizeT height);

	/// handles messages
	bool HandleMessage(const Ptr<Messaging::Message>& msg);
private:

	/// calculates variables
	void Calculate();

    /// disable algorithm
	void Disable();

	RenderUtil::DrawFullScreenQuad quad;
	Ptr<CoreGraphics::Shader> hbao;
	Ptr<CoreGraphics::Shader> blur;
	Ptr<CoreGraphics::ShaderVariable> depthTextureVar;

#ifdef HBAO_COMPUTE
	Ptr<CoreGraphics::ShaderVariable> hbao0Var;
	Ptr<CoreGraphics::ShaderVariable> hbao1Var;
	Ptr<CoreGraphics::ShaderVariable> hbaoBlurLinearVar;
	Ptr<CoreGraphics::ShaderVariable> hbaoBlurPointVar;
	Ptr<CoreGraphics::ShaderVariable> hbaoBlurRGVar;
	Ptr<CoreGraphics::ShaderVariable> hbaoBlurRVar;
#else
	Ptr<CoreGraphics::ShaderVariable> hbaoTextureVar;
    Ptr<CoreGraphics::ShaderVariable> hbaoBlurredTextureVar;
    Ptr<CoreGraphics::ShaderVariable> hbaoFinalTextureVar;
	Ptr<CoreGraphics::ShaderVariable> randomTextureVar;
	Ptr<CoreGraphics::Texture> randomTexture;
	Ptr<CoreGraphics::ShaderVariable> focalLengthVar;
	Ptr<CoreGraphics::ShaderVariable> maxRadiusPixelsVar;
	Ptr<CoreGraphics::ShaderVariable> negInvR2Var;

#endif

#if HBAO_COMPUTE
	Ptr<CoreGraphics::ShaderReadWriteTexture> internalTargets[2];
	Ptr<CoreGraphics::ShaderReadWriteTexture> output;
#else
	Ptr<CoreGraphics::RenderTarget> internalTargets[2];
	Ptr<CoreGraphics::RenderTarget> output;
#endif
	CoreGraphics::ShaderFeature::Mask xDirection;
	CoreGraphics::ShaderFeature::Mask yDirection;


	Ptr<CoreGraphics::ShaderVariable> uvToViewAVar;
	Ptr<CoreGraphics::ShaderVariable> uvToViewBVar;
	Ptr<CoreGraphics::ShaderVariable> r2Var;
	Ptr<CoreGraphics::ShaderVariable> rVar;
	Ptr<CoreGraphics::ShaderVariable> aoResolutionVar;
	Ptr<CoreGraphics::ShaderVariable> invAOResolutionVar;
	Ptr<CoreGraphics::ShaderVariable> strengthVar;
	Ptr<CoreGraphics::ShaderVariable> tanAngleBiasVar;
	Ptr<CoreGraphics::ShaderVariable> powerExponentVar;
	Ptr<CoreGraphics::ShaderVariable> blurFalloff;
	Ptr<CoreGraphics::ShaderVariable> blurDepthThreshold;

	struct AOVariables
	{
		Math::float2 uvToViewA;
		Math::float2 uvToViewB;
		float r;
		float r2;
		float negInvR2;
		Math::float2 focalLength;
		Math::float2 aoResolution;
		Math::float2 invAOResolution;
		float maxRadiusPixels;
		float strength;
		float tanAngleBias;
		float blurThreshold;
		float blurFalloff;
	} vars;

	struct BlurVariables
	{
		float radius;
		float sharpness;
	};

	float nearZ;
	float farZ;
	float sceneScale;

	float fullWidth;
	float fullHeight;

	float width;
	float height;

	float radius;
	float blurSharpness;
	float blurRadius;
	float downSample;
}; 

__RegisterClass(AOAlgorithm);
} // namespace Algorithm
//------------------------------------------------------------------------------