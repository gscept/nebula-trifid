#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::PreviewState
    
    Base state for previewer window
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "graphics/globallightentity.h"
#include "graphics/modelentity.h"
#include "game/entity.h"
#include "physics/model/physicsmodel.h"
#include "physics/resource/managedphysicsmodel.h"
#include "posteffect/posteffectmanager.h"
#include "godrays/godrayrendermodule.h"
#include "materials/surfaceinstance.h"
#include "materials/managedsurface.h"
//------------------------------------------------------------------------------
namespace ContentBrowser
{
class PreviewState : public BaseGameFeature::GameStateHandler
{
	__DeclareClass(PreviewState);
public:
	/// constructor
	PreviewState();
	/// destructor
	virtual ~PreviewState();

	/// override on state enter
	void OnStateEnter(const Util::String& prevState);
	/// override on state leave
	void OnStateLeave(const Util::String& nextState);
	/// override on frame
	Util::String OnFrame();

	/// sets the model to render, returns true if model is properly loaded
	bool SetModel(const Resources::ResourceId& resource);
	/// returns pointer to model entity
	const Ptr<Graphics::ModelEntity>& GetModel() const;
	/// sets the physics model to display
	bool SetPhysics(const Resources::ResourceId& resource);
	/// returns the pointer to the physics entity
	const Ptr<Physics::ManagedPhysicsModel>& GetPhysics() const;
	
	/// sets the surface material on the preview sphere
	void SetSurface(const Ptr<Materials::SurfaceInstance>& instance);
	/// discards the current surface preview and sets the placeholder one isntead
	void DiscardSurface();
	/// sets whether or not the surface preview should be shown
	void SetSurfacePreview(bool b);

	/// saves whatever is in the preview view to file, swapStage will use the rendered stage instead of the surface stage
	void SaveThumbnail(const Util::String& path, bool swapStage = false);

	/// returns pointer to light
	const Ptr<Graphics::GlobalLightEntity>& GetLight() const;

private:
	/// called whenever the skin list is returned
	void OnFetchedSkinList(const Ptr<Messaging::Message>& msg);

	/// display as wireframe
	void SetShowWireframe(bool enable);

	/// handles mouse input
	void HandleInput();

	Ptr<Game::Entity> defaultCam;
	Ptr<Graphics::ModelEntity> modelEntity;	
	Ptr<Physics::ManagedPhysicsModel> physicsModel;	
	Util::Array<Ptr<Physics::PhysicsObject>> physicsObjects;

	Ptr<Graphics::Stage> defaultStage;

	Ptr<Materials::ManagedSurface> placeholderSurface;
	Ptr<Graphics::Stage> surfaceStage;
	Ptr<Graphics::View> surfaceView;
	Ptr<Graphics::CameraEntity> surfaceCamera;
	Ptr<Graphics::GlobalLightEntity> surfaceLight;
	Ptr<Graphics::ModelEntity> surfaceModelEntity;
	Ptr<CoreGraphics::RenderTarget> surfaceViewTarget;

	Math::matrix44 lightTransform;

	bool showAO;
	bool showPhysics;
	bool showControls;
	bool showWireframe;
	bool showSurface;
	bool showSky;
	int showPassNumber;
	Ptr<CoreGraphics::Texture> showPass;
	bool workLight;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::ModelEntity>& 
PreviewState::GetModel() const
{
	return this->modelEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Physics::ManagedPhysicsModel>&
PreviewState::GetPhysics() const
{
	return this->physicsModel;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::GlobalLightEntity>& 
PreviewState::GetLight() const
{
	return PostEffect::PostEffectServer::Instance()->GetGlobalLightEntity();
}

} // namespace ContentBrowser
//------------------------------------------------------------------------------