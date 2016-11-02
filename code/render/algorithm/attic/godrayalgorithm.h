#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::GodrayAlgorithm
    
    
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "algorithmbase.h"
#include "renderutil/drawfullscreenquad.h"

namespace CoreGraphics
{
	class ShaderInstance;
	class ShaderVariable;
	class ShaderVariableInstance;
	class RenderTarget;
	class Texture;
}

namespace Graphics
{
	class ModelEntity;
}

namespace Resources
{
	class ManagedTexture;
}

namespace Algorithm
{
class LightScatterAlgorithm : public AlgorithmBase
{
	__DeclareClass(LightScatterAlgorithm);
public:
	/// constructor
	LightScatterAlgorithm();
	/// destructor
	virtual ~LightScatterAlgorithm();

	/// setup godrays
	void Setup();
	/// discard godrays
	void Discard();

	/// calculates sun entity position
	void Prepare();
	/// renders godrays
	void Execute();

	/// called whenever the display resizes
	void OnDisplayResize();

	/// enables/disables algorithm
	void SetEnabled(bool b);

	/// handles messages
	bool HandleMessage(const Ptr<Messaging::Message>& msg);

private:
	/// enabled algorithm
	void Enable();
	/// disabled algorithm
	void Disable();

	bool sunLightRelative;
	Math::float4 sunPosition;
	Math::float4 sunColor;
	bool colorDirty;

	RenderUtil::DrawFullScreenQuad quad;
	Ptr<CoreGraphics::ShaderInstance> lightScatterShader;
	Ptr<CoreGraphics::ShaderInstance> sunShader;
	Ptr<CoreGraphics::ShaderInstance> verticalBloom;
	Ptr<CoreGraphics::ShaderInstance> horizontalBloom;

	Ptr<Graphics::ModelEntity> sunEntity;
	Ptr<Resources::ManagedTexture> whiteMap;
	Ptr<Resources::ManagedTexture> currentTexture;

	Ptr<CoreGraphics::ShaderVariable> lightScatterLightPosVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterDensityVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterDecayVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterWeightVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterExposureVar;

	Ptr<CoreGraphics::ShaderVariableInstance> sunTextureVarInst;
	Ptr<CoreGraphics::ShaderVariableInstance> sunColorVarInst;

	Ptr<CoreGraphics::ShaderVariable> unshadedTexVar;
	Ptr<CoreGraphics::ShaderVariable> godrayTexVar;
	Ptr<CoreGraphics::ShaderVariable> bloomedTexVar;

	Ptr<CoreGraphics::RenderTarget> GodrayBuffer;
	Ptr<CoreGraphics::RenderTarget> output;

}; 

__RegisterClass(LightScatterAlgorithm);
} // namespace Algorithm
//------------------------------------------------------------------------------