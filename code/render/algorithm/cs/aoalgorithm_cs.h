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
	void Prepare();
	/// execute algorithm
	void Execute();

	/// handle display resizing
	void OnDisplayResize();

	/// enables/disables algorithm
	void SetEnabled(bool b);

	/// handles messages
	bool HandleMessage(const Ptr<Messaging::Message>& msg);
private:

	/// calculates variables
	void Calculate();

    /// disable algorithm
	void Disable();

	Ptr<CoreGraphics::ShaderInstance> hbao;
	Ptr<CoreGraphics::ShaderInstance> blur;
	Ptr<CoreGraphics::ShaderVariable> depthTextureVar;
	Ptr<CoreGraphics::ShaderVariable> randomTextureVar;
    Ptr<CoreGraphics::ShaderVariable> hbaoOutput0;
    Ptr<CoreGraphics::ShaderVariable> hbaoOutput1;
    Ptr<CoreGraphics::ShaderVariable> hbaoInput0;
	Ptr<CoreGraphics::ShaderVariable> hbaoTextureVar;
    Ptr<CoreGraphics::ShaderVariable> hbaoBlurredTextureVar;
    Ptr<CoreGraphics::ShaderVariable> hbaoFinalTextureVar;

    static const SizeT NumInternalRTargets = 1;
    static const SizeT NumInternalRGTargets = 2;

    Ptr<CoreGraphics::RenderTarget> internalTargetsR[NumInternalRTargets];
    Ptr<CoreGraphics::RenderTarget> internalTargetsRG[NumInternalRGTargets];
	Ptr<CoreGraphics::RenderTarget> output;
	Ptr<CoreGraphics::Texture> randomTexture;

    CoreGraphics::ShaderFeature::Mask xDimShaderFeature;
    CoreGraphics::ShaderFeature::Mask yDimShaderFeature;

	Ptr<CoreGraphics::ShaderVariable> uvToViewAVar;
	Ptr<CoreGraphics::ShaderVariable> uvToViewBVar;
	Ptr<CoreGraphics::ShaderVariable> r2Var;
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
		float r2;
		Math::float2 aoResolution;
		Math::float2 invAOResolution;
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