#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::PostEffectFeatureUnit

    (C) 2008 Radon Labs GmbH
*/
#include "game/featureunit.h"
#include "posteffect/posteffectmanager.h"
#include "posteffectregistry.h"

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

	/// apply a posteffect preset
	void ApplyPreset(const Util::String & preset);
	/// return the last applied preset
	const Util::String & GetLastPreset() const;

	/// setup posteffect entities (skybox), called on state handler switches
	void SetupDefaultWorld();
	/// cleanup entities
	void CleanupDefaultWorld();

	/// get default post effect entity
	const Ptr<PostEffect::PostEffectEntity>& GetDefaultPostEffect() const;

private:	
    Ptr<PostEffectManager> postEffectManager;
	Ptr<PostEffect::PostEffectEntity> defaultPostEffect;
	Ptr<PostEffect::PostEffectRegistry> postEffectRegistry;   
	Ptr<Graphics::ModelEntity> skyEntity;
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