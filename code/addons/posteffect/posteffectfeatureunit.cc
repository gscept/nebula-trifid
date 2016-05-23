//------------------------------------------------------------------------------
//  posteffect/posteffectfeatureunit.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffectfeatureunit.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphics/modelentity.h"
#include "basegamefeatureunit.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "managers/levelattrsmanager.h"


namespace PostEffect
{
__ImplementClass(PostEffect::PostEffectFeatureUnit, 'PXFU', Game::FeatureUnit);
__ImplementSingleton(PostEffect::PostEffectFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
PostEffectFeatureUnit::PostEffectFeatureUnit()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PostEffectFeatureUnit::~PostEffectFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();
	
	// setup post effect registry from db
	this->postEffectRegistry = PostEffectRegistry::Create();
	this->postEffectRegistry->OnActivate();

	// setup default post effect
	this->defaultPostEffect = PostEffect::PostEffectEntity::Create();
	this->defaultPostEffect->SetDefaultEntity(true);

	// setup post effect system
	this->postEffectManager = PostEffect::PostEffectManager::Create();
	this->AttachManager(this->postEffectManager.cast<Game::Manager>());

	// reset post effect system to default and attach default entity
	this->postEffectManager->ResetPostEffectSystem();
	this->postEffectManager->AttachEntity(this->defaultPostEffect);

	// set name to default until something else is applied
	this->lastPreset = "Default";
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnDeactivate()
{	
	this->postEffectManager->RemoveEntity(this->defaultPostEffect);
	this->defaultPostEffect = 0;
	// remove post effect manager
	this->RemoveManager(this->postEffectManager.upcast<Game::Manager>());
	this->postEffectManager = 0;

	this->postEffectRegistry->OnDeactivate();
	this->postEffectRegistry = 0;

    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnFrame()
{
	FeatureUnit::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::ApplyPreset(const Util::String & preset)
{
	n_assert(this->defaultPostEffect.isvalid());
	n_assert2(this->postEffectRegistry->HasPreset(preset),"Unknown preset for posteffect");
	this->postEffectRegistry->ApplySettings(preset, this->defaultPostEffect);
	this->lastPreset = preset;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::SetupDefaultWorld()
{
	// create skybox
	this->skyEntity = Graphics::ModelEntity::Create();
	this->skyEntity->SetTransform(Math::matrix44::translation(0, 0, 0));
	this->skyEntity->SetResourceId("mdl:system/skybox.n3");
	Ptr<Graphics::Stage> stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();

	stage->AttachEntity(this->skyEntity.cast<Graphics::GraphicsEntity>());

	// set sky entity in post effect manager
	this->postEffectManager->SetSkyEntity(this->skyEntity);

	// set global light entity
	PostEffect::PostEffectServer::Instance()->SetGlobalLightEntity(Graphics::GraphicsServer::Instance()->GetCurrentGlobalLightEntity());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::CleanupDefaultWorld()
{	
	PostEffect::PostEffectServer::Instance()->StopAllBlending();
	Ptr<Graphics::Stage> stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
	if (stage.isvalid())
	{
		stage->RemoveEntity(this->skyEntity.cast<Graphics::GraphicsEntity>());
	}
	this->skyEntity = 0;
	PostEffect::PostEffectServer::Instance()->SetGlobalLightEntity(0);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnLoad()
{
	Util::String preset = BaseGameFeature::LevelAttrsManager::Instance()->GetString(Attr::PostEffectPreset);
	this->ApplyPreset(preset);	
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnBeforeLoad()
{
	this->SetupDefaultWorld();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectFeatureUnit::OnBeforeCleanup()
{
	this->CleanupDefaultWorld();
}

};