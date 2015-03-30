#pragma once
//------------------------------------------------------------------------------
/**
    @class Environment::AOModule
    
    Handles the AO rendering.
    
    (C) 2013 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "environment/base/environmentmodulebase.h"
#include "frame/frameshader.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/texture.h"
namespace Environment
{
class AOModule : public Base::EnvironmentModuleBase
{
	__DeclareClass(AOModule);
public:
	/// constructor
	AOModule();
	/// destructor
	virtual ~AOModule();

	/// setup ao
	void Setup();
	/// discard ao
	void Discard();

	/// handles on-frame stuff
	void BeginFrame(const Ptr<Graphics::CameraEntity>& cam);
	/// renders ao
	void Render();
	/// ends frame
	void EndFrame();

	/// handles messages
	void HandleMessage(const Ptr<Messaging::Message>& msg);

private:
	/// enables/disables module
	void SetEnabled(bool b);
	/// sets up the random texture
	void SetupRandomTexture();

	Ptr<Frame::FrameShader> aoFrameShader;
	Ptr<CoreGraphics::ShaderInstance> aoShader;
	Ptr<CoreGraphics::ShaderInstance> blurShader;
	Ptr<CoreGraphics::ShaderInstance> linearizeShader;

	Ptr<CoreGraphics::ShaderVariable> uvToViewAVar;
	Ptr<CoreGraphics::ShaderVariable> uvToViewBVar;
	Ptr<CoreGraphics::ShaderVariable> rVar;
	Ptr<CoreGraphics::ShaderVariable> r2Var;
	Ptr<CoreGraphics::ShaderVariable> negInvR2Var;
	Ptr<CoreGraphics::ShaderVariable> focalLengthVar;
	Ptr<CoreGraphics::ShaderVariable> aoResolutionVar;
	Ptr<CoreGraphics::ShaderVariable> invAOResolutionVar;
	Ptr<CoreGraphics::ShaderVariable> maxRadiusPixelsVar;
	Ptr<CoreGraphics::ShaderVariable> strengthVar;
	Ptr<CoreGraphics::ShaderVariable> tanAngleBiasVar;
	Ptr<CoreGraphics::ShaderVariable> powerExponent;

	Ptr<CoreGraphics::ShaderVariable> depthMap;
	Ptr<CoreGraphics::ShaderVariable> randomMap;
	Ptr<CoreGraphics::Texture> randomTexture;

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
} // namespace Environment
//------------------------------------------------------------------------------