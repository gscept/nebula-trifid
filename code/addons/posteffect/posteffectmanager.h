#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::PostEffectManager
  
    Server object of the post effect subsystem.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "posteffect/posteffectentity.h"
#include "timing/time.h"
#include "graphics/globallightentity.h"
#include "graphics/modelentity.h"
#include "posteffect/posteffectrendermodule.h"
#include "posteffect/rt/posteffectserver.h"
#include "attr/attrid.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class PostEffectManager : public Game::Manager
{
    __DeclareClass(PostEffectManager);
    __DeclareSingleton(PostEffectManager);
public:
    
    /// constructor
    PostEffectManager();
    /// destructor
    virtual ~PostEffectManager();

    /// open the posteffect server
    virtual void OnActivate();
    /// close the posteffect server
    virtual void OnDeactivate();    
    /// "render" the current post effect
    virtual void OnFrame();

    /// reset posteffect manager and trigger render thread to reset the server
    void ResetPostEffectSystem();

    /// set the main directional light which should be influenced by post effect entities
    /// (this is set from lightentity on creation of global light entity)
    void SetGlobalLightEntity(const Ptr<Graphics::GlobalLightEntity>& l);
    /// get the main directional light (may return 0)
    const Ptr<Graphics::GlobalLightEntity>& GetGlobalLightEntity() const;

    /// set the graphics entities which represent the sky (optional)
    void SetSkyEntity(const Ptr<Graphics::ModelEntity>& e);
    /// get the graphics entities which represent the sky (optional)
    const Ptr<Graphics::ModelEntity>& GetSkyEntity() const;

    /// attach a post effect entity to the server
    void AttachEntity(const Ptr<PostEffectEntity>& entity);
    /// remove a post effect entity from the server
    void RemoveEntity(const Ptr<PostEffectEntity>& entity);
	/// returns the default post effect entity
	const Ptr<PostEffectEntity>& GetDefaultEntity() const;
    
    /// set the current point of interest (usually camera pos)
    /// (this is set from the cameraproperty on render)
    void SetPointOfInterest(const Math::point& p);
    /// get the current point of interest
    const Math::point& GetPointOfInterest() const;

    /// set depth-of-field for blending (time 0 causes instant set, no fade)
    void StartDepthOfFieldBlend(const Ptr<DepthOfFieldParams>& params, Timing::Time duration);
    /// set the fog parameters for blending (time 0 causes instant set, no fade)
    void StartFogBlend(const Ptr<FogParams>& params, Timing::Time blendTime);
    /// set the color parameters for blending (time 0 causes instant set, no fade)
    void StartColorBlend(const Ptr<ColorParams>& params, Timing::Time blendTime);
    /// set the hdr parameters for blending (time 0 causes instant set, no fade)
    void StartHdrBlend(const Ptr<HdrParams>& params, Timing::Time blendTime);
    /// set the light parameters for blending (time 0 causes instant set, no fade)
    void StartLightBlend(const Ptr<LightParams>& params, Timing::Time blendTime);
    /// set the sky parameters for blending (time 0 causes instant set, no fade)
    void StartSkyBlend(const Ptr<SkyParams>& params, Timing::Time blendTime);
    /// set the ao parameterse for blending (time 0 causes instant set, no fade)
    void StartAOBlend(const Ptr<AoParams>& params, Timing::Time blendTime);

	/// fade
	void Fade(Timing::Time time, float start, float target);
	/// fade and wait till renderthread has faded
	template<class CLASS, void (CLASS::*METHOD)(Timing::Time)> void FadeAndCallback(Timing::Time time, float start, float target, CLASS* obj);

    /// get CurFogParams	
    const Ptr<FogParams>& GetCurFogParams() const;

private:
    /// find current "point-of-interest" entity
    Ptr<PostEffectEntity> FindPointOfInterestEntity();

    /// set current blend params directly from the entity without blending
    void GetParamsFromEntity(const Ptr<PostEffectEntity>& entity);

    /// send the depth-of-field render variables update
    void SendDepthOfFieldParameters(Timing::Time blendTime);
    /// send the fog parameters update
    void SendFogParameters(Timing::Time blendTime);
    /// send the color parameters update
    void SendColorParameters(Timing::Time blendTime);
    /// send the hdr parameters update
    void SendHdrParameters(Timing::Time blendTime);
    /// send the light parameters update
    void SendLightParameters(Timing::Time blendTime);
    /// send the sky parameters update
    void SendSkyParameters(Timing::Time blendTime);
    /// send ao parameters update
    void SendAOParameters(Timing::Time blendTime);
	/// send the sky
	void SendSky();

    /// cleans up entities
    void Cleanup();
  
    bool firstFrame;
	bool newSky;

    Util::Array<Ptr<PostEffectEntity> > entities;
    Ptr<PostEffectEntity> currentEntity;
    Ptr<PostEffectEntity> defaultEntity;

    Math::point pointOfInterest;

    Ptr<Graphics::GlobalLightEntity> globalLightEntity;     // the global directional light source
	Ptr<Graphics::ModelEntity> skyEntity;
      
    Ptr<DepthOfFieldParams> curDepthOfFieldParams;
    Ptr<ColorParams> curColorParams;
    Ptr<FogParams> curFogParams;
    Ptr<HdrParams> curHdrParams;
    Ptr<LightParams> curLightParams;
    Ptr<SkyParams> curSkyParams;
    Ptr<AoParams> curAoParams;
};


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PostEffectEntity>& 
PostEffectManager::GetDefaultEntity() const
{
	return this->defaultEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PostEffectManager::SetGlobalLightEntity(const Ptr<Graphics::GlobalLightEntity>& l)
{
    this->globalLightEntity = l;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::GlobalLightEntity>&
PostEffectManager::GetGlobalLightEntity() const
{
    return this->globalLightEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PostEffectManager::SetSkyEntity(const Ptr<Graphics::ModelEntity>& e)
{
	n_assert(e.isvalid());
    this->skyEntity = e;
	this->newSky = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::ModelEntity>&
PostEffectManager::GetSkyEntity() const
{
    return this->skyEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline void
PostEffectManager::SetPointOfInterest(const Math::point& p)
{
    this->pointOfInterest = p;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::point&
PostEffectManager::GetPointOfInterest() const
{
    return this->pointOfInterest;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<FogParams>& 
PostEffectManager::GetCurFogParams() const
{
    return this->curFogParams;
}

//------------------------------------------------------------------------------
/**
*/
template<class CLASS, void (CLASS::*METHOD)(Timing::Time)>
void PostEffectManager::FadeAndCallback(Timing::Time time, float start, float target, CLASS* obj)
{
	Util::Delegate<Timing::Time> del = Util::Delegate<Timing::Time>::FromMethod<CLASS, METHOD>(obj);
	PostEffectServer::Instance()->StartFadeCallback(time, start, target, del);
}

} // namespace PostEffect
//------------------------------------------------------------------------------
