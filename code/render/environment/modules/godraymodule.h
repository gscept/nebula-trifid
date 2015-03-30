#pragma once
//------------------------------------------------------------------------------
/**
    @class Environment::GodrayModule
    
    Implements god rays.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include "environment/base/environmentmodulebase.h"
#include "coregraphics/shaderinstance.h"
#include "graphics/modelentity.h"
#include "frame/frameshader.h"
#include "coregraphics/shadervariable.h"
#include "resources/managedtexture.h"
namespace Environment
{
class GodrayModule : public Base::EnvironmentModuleBase
{
	__DeclareClass(GodrayModule);
public:
	/// constructor
	GodrayModule();
	/// destructor
	virtual ~GodrayModule();

	/// setup godrays
	void Setup();
	/// discard godrays
	void Discard();

	/// handles on-frame stuff
	void BeginFrame(const Ptr<Graphics::CameraEntity>& cam);
	/// render godrays
	void Render();
	/// ends frame
	void EndFrame();
	/// set attached/detached on module
	virtual void SetAttached(bool);

	/// handles messages
	void HandleMessage(const Ptr<Messaging::Message>& msg);

private:

	/// enables/disables god rays
	void SetEnabled(bool b);
	/// sets the sun texture
	void SetSunTexture(const Resources::ResourceId& res);


	Ptr<Graphics::CameraEntity> camera;
	Ptr<Frame::FrameShader> godRayFrameShader;
	Ptr<Graphics::ModelEntity> sunEntity;
	Ptr<Resources::ManagedTexture> whiteMap;

	Ptr<CoreGraphics::ShaderInstance> godRayShader;
	Ptr<CoreGraphics::ShaderVariable> godRayTex;
	Ptr<CoreGraphics::ShaderVariable> godRayLightPos;
	Ptr<CoreGraphics::ShaderVariable> godRayDensity;
	Ptr<CoreGraphics::ShaderVariable> godRayDecay;
	Ptr<CoreGraphics::ShaderVariable> godRayWeight;
	Ptr<CoreGraphics::ShaderVariable> godRayExposure;

	Ptr<CoreGraphics::ShaderInstance> sunShader;
	Ptr<CoreGraphics::ShaderVariable> sunTexture;
	Ptr<CoreGraphics::ShaderVariable> sunColor;

}; 
} // namespace Environment
//------------------------------------------------------------------------------