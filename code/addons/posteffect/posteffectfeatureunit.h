#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::PostEffectFeatureUnit

    (C) 2008 Radon Labs GmbH
*/
#include "game/featureunit.h"
#include "posteffect/posteffectmanager.h"
#include "posteffectregistry.h"
#include "clouds/cloudsaddon.h"

namespace PostEffect
{
class PostEffectFeatureUnit : public Game::FeatureUnit

{
	__DeclareClass(PostEffectFeatureUnit);
	__DeclareSingleton(PostEffectFeatureUnit);
public:
    /// Constructor
	PostEffectFeatureUnit();
    /// Destructor
	~PostEffectFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();	
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();
	
	/// called from within GameServer::NotifyBeforeLoad() before the database is loaded
	virtual void OnBeforeLoad();
	/// called from within GameServer::NotifyBeforeCleanup() before shutting down a level
	virtual void OnBeforeCleanup();
	/// called from within GameServer::Load() after attributes are loaded
	virtual void OnLoad();

	/// apply a posteffect preset
	void ApplyPreset(const Util::String & preset);
	/// return the last applied preset
	const Util::String & GetLastPreset() const;

	

	/// get default post effect entity
	const Ptr<PostEffect::PostEffectEntity>& GetDefaultPostEffect() const;

private:	
	/// setup posteffect entities (skybox), called on state handler switches
	void SetupDefaultWorld();
	/// cleanup entities
	void CleanupDefaultWorld();
    Ptr<PostEffectManager> postEffectManager;
	Ptr<PostEffect::PostEffectEntity> defaultPostEffect;
	Ptr<PostEffect::PostEffectRegistry> postEffectRegistry;   
	Ptr<Graphics::ModelEntity> skyEntity;
	Ptr<Clouds::CloudsAddon> cloudsAddon;
	Util::String lastPreset;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PostEffect::PostEffectEntity>&
PostEffectFeatureUnit::GetDefaultPostEffect() const
{
	return this->defaultPostEffect;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
PostEffectFeatureUnit::GetLastPreset() const
{
	return this->lastPreset;
}
}; // namespace PostEffect