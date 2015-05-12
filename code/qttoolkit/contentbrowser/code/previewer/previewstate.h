#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::PreviewState
    
    Base state for previewer window
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "graphics/globallightentity.h"
#include "graphics/modelentity.h"
#include "game/entity.h"
#include "physics/model/physicsmodel.h"
#include "physics/resource/managedphysicsmodel.h"
#include "posteffect/posteffectmanager.h"
#include "godrays/godrayrendermodule.h"
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

	/// enables rendering of the physics shape
	void SetShowPhysics(bool enable);
	/// enables display of controls
	void SetShowControls(bool enable);
	/// enables worklight
	void SetUseWorklight(bool enable);

	/// returns pointer to light
	const Ptr<Graphics::GlobalLightEntity>& GetLight() const;

private:
	/// called whenever the skin list is returned
	void OnFetchedSkinList(const Ptr<Messaging::Message>& msg);

	/// handles mouse input
	void HandleInput();

	Ptr<Game::Entity> defaultCam;
	Ptr<Graphics::ModelEntity> modelEntity;	
	Ptr<Physics::ManagedPhysicsModel> physicsModel;	
	Util::Array<Ptr<Physics::PhysicsObject>> physicsObjects;

	Math::matrix44 lightTransform;
	bool enablePhysics;
	bool showControls;
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
inline void
PreviewState::SetShowPhysics(bool enable)
{
	this->enablePhysics = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PreviewState::SetShowControls( bool enable )
{
	this->showControls = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PreviewState::SetUseWorklight( bool enable )
{
	this->workLight = enable;
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