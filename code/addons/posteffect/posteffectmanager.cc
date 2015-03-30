//------------------------------------------------------------------------------
//  posteffect/posteffectmanager.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffect/posteffectmanager.h"
#include "basegametiming/gametimesource.h"
#include "basegamefeature/managers/enventitymanager.h"
#include "basegametiming/systemtimesource.h"
#include "posteffectprotocol.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsprotocol.h"
#include "rt/posteffectrtplugin.h"

namespace PostEffect
{
using namespace Graphics;
using namespace Math;
using namespace Input;

__ImplementClass(PostEffect::PostEffectManager, 'PEMA', Game::Manager);
__ImplementSingleton(PostEffect::PostEffectManager);

//------------------------------------------------------------------------------
/**
*/
PostEffectManager::PostEffectManager() :
    firstFrame(false),
	newSky(false),
	pointOfInterest(float4(0,0,0,1))
{   
    __ConstructSingleton;
}   

//------------------------------------------------------------------------------
/**
*/
PostEffectManager::~PostEffectManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::OnActivate()
{
    n_assert(this->entities.IsEmpty());
    n_assert(!this->defaultEntity.isvalid());
    n_assert(!this->currentEntity.isvalid());
    n_assert(!this->globalLightEntity.isvalid());
	n_assert(!this->skyEntity.isvalid());

    // Register the posteffect render-thread plugin
    Ptr<RegisterRTPlugin> msg = RegisterRTPlugin::Create();
    msg->SetType(&PostEffectRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Messaging::Message>());  

	// init
    this->firstFrame = true;

    this->curDepthOfFieldParams = DepthOfFieldParams::Create();
    this->curColorParams = ColorParams::Create();
    this->curFogParams = FogParams::Create();
    this->curHdrParams = HdrParams::Create();
    this->curLightParams = LightParams::Create();
    this->curSkyParams = SkyParams::Create();
    this->curAoParams = AoParams::Create();
    
    // call parent
    Game::Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::OnDeactivate()
{
    this->Cleanup();

    // Unregister the posteffect render-thread plugin
    Ptr<UnregisterRTPlugin> msg = UnregisterRTPlugin::Create();
    msg->SetType(&PostEffectRTPlugin::RTTI);
    GraphicsInterface::Instance()->Send(msg.cast<Messaging::Message>());

    // call parent
    Game::Manager::OnDeactivate();

    // release our variables
    this->curAoParams = 0;
    this->curDepthOfFieldParams = 0;
    this->curColorParams = 0;
    this->curFogParams = 0;
    this->curHdrParams = 0;
    this->curLightParams = 0;
    this->curSkyParams = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::OnFrame()
{
    // find the current "point of interest entity", which is the entity where the
    // point of interest is currently in (or the default entity), if there is
    // no POI-Entity found in the level this means there is no
    // PostEffects-Stuff at all in this level
    Ptr<PostEffectEntity> poiEntity = this->FindPointOfInterestEntity();
    if (poiEntity.isvalid())
    {
        // check if a new blend must be started, if currently a blend
        // is in progress this blend must not be disturbed to prevent
        // popping!
        if (this->currentEntity != poiEntity || poiEntity->IsDirty())
        {
            // start blending to values from the poi entity
			Timing::Time blendTime = poiEntity->Params().common->GetBlendTime();
            this->GetParamsFromEntity(poiEntity);
            this->SendColorParameters(blendTime);
            this->SendDepthOfFieldParameters(blendTime);
            this->SendFogParameters(blendTime);
            this->SendHdrParameters(blendTime);
            this->SendLightParameters(blendTime);
            this->SendSkyParameters(blendTime);
            this->SendAOParameters(blendTime);

            // now set current to poi, to avoid multiple para sending
            this->currentEntity = poiEntity;
			this->currentEntity->SetDirty(false);
        }

		// check if we have a new sky, if so, send it to the server
		if (this->newSky)
		{
			this->SendSky();
			this->newSky = false;
		}
    }

    // call parent
    Game::Manager::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::ResetPostEffectSystem()
{
    this->Cleanup();

    // send reset to render thread
    Ptr<PostEffect::ResetPostEffectSystem> resetMsg = PostEffect::ResetPostEffectSystem::Create();
    Graphics::GraphicsInterface::Instance()->Send(resetMsg.cast<Messaging::Message>());    
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::AttachEntity(const Ptr<PostEffectEntity>& entity)
{
    n_assert(0 != entity);
    this->entities.Append(entity);
    entity->OnAttach();

    // check if this post effect entity has a texture to preload
    Util::String texPath = entity->Params().sky->GetSkyTexturePath();
    if(texPath.IsValid())
    {
        PostEffectServer::Instance()->PreloadTexture(texPath);
    }

    if (entity->IsDefaultEntity())
    {
        n_assert2(this->defaultEntity == 0, "PostEffectManager::AttachEntity: Already a default posteffect entity set!\n");
        this->defaultEntity = entity;
    }   
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::RemoveEntity(const Ptr<PostEffectEntity>& entity)
{
    n_assert(0 != entity);
    int index = this->entities.FindIndex(entity);
    n_assert(InvalidIndex != index);

    // check if this post effect entity has a texture to preload
    Util::String texPath = entity->Params().sky->GetSkyTexturePath();
    if(texPath.IsValid())
    {
        PostEffectServer::Instance()->UnloadTexture(texPath);
    }
    entity->OnRemove();
    if (this->defaultEntity == entity)
    {
        this->defaultEntity = 0;
    }
    if (this->currentEntity == entity)
    {
        this->currentEntity = 0;
    }
    this->entities.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectManager::Cleanup()
{
    while (!this->entities.IsEmpty())
    {
        this->RemoveEntity(this->entities[0]);
    }
    n_assert(!this->defaultEntity.isvalid());
    n_assert(!this->currentEntity.isvalid());

    // release special graphics entities
    this->globalLightEntity = 0;
    this->skyEntity = 0;
}

//------------------------------------------------------------------------------
/**
    Set current post effect parameters to the values from the
    current blend target entity without blending.
*/
void
PostEffectManager::GetParamsFromEntity(const Ptr<PostEffectEntity>& entity)
{
    n_assert(0 != entity);
    this->curColorParams = entity->Params().color;
    this->curDepthOfFieldParams = entity->Params().dof;
    this->curFogParams = entity->Params().fog;
    this->curHdrParams = entity->Params().hdr;
    this->curLightParams = entity->Params().light;
    this->curSkyParams = entity->Params().sky;
    this->curAoParams = entity->Params().ao;

    // set the light transform for the global directional light, if the 
    // blend target entity is a sphere we just use its transformation,
    // if it is a box we use the transform of the default entity since
    // the orientation of the box cannot be used for the directional light
    if (entity->GetShapeType() == PostEffectEntity::Box)
    {
        if (this->defaultEntity.isvalid())
        {
            this->curLightParams->SetLightTransform(this->defaultEntity->GetTransform());
        }
    }
}

//------------------------------------------------------------------------------
/**
	This method finds the current "point of interest entity" which is the
	highest priority entity where the current point of interest is inside, 
	or the default entity if not inside a specific entity. 

	NOTE: The method may return NULL if there is no default entity in the
	level! 
*/
Ptr<PostEffectEntity>
PostEffectManager::FindPointOfInterestEntity()
{
    Ptr<PostEffectEntity> poiEntity = 0;

    // because of entity priorities we need to check all entities
    // every time...
    int entityIndex;    
    for (entityIndex = 0; entityIndex < this->entities.Size(); entityIndex++)
    {
        const Ptr<PostEffectEntity>& curEntity = this->entities[entityIndex];
        if (!curEntity->IsDefaultEntity() && 
             curEntity->IsInside(this->GetPointOfInterest()) &&
             curEntity->IsEnabled())
        {
            // check priorities...
            if (poiEntity.isvalid())
            {
                if (curEntity->GetPriority() > poiEntity->GetPriority())
                {
                    // new entity is higher priority
                    poiEntity = curEntity;
                }
            }
            else
            {
                // no previous poiEntity, assign without priority check
                poiEntity = curEntity;
            }
        }
    }

    // if the point of interest is outside all non-default entities, 
    // use the default entity
    if (!poiEntity.isvalid())
    {
        poiEntity = this->defaultEntity.get_unsafe();
    }

    // hmm, if there's still no point of interest entity at this point
    // there's nothing we can do... so we may return a NULL pointer
    return poiEntity;
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartDepthOfFieldBlend(const Ptr<DepthOfFieldParams>& params, Timing::Time blendTime)
{
    this->curDepthOfFieldParams = params;
    this->SendDepthOfFieldParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartFogBlend(const Ptr<FogParams>& params, Timing::Time blendTime)
{
    this->curFogParams = params;
    this->SendFogParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartColorBlend(const Ptr<ColorParams>& params, Timing::Time blendTime)
{
    this->curColorParams = params;
    this->SendColorParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartHdrBlend(const Ptr<HdrParams>& params, Timing::Time blendTime)
{
    this->curHdrParams = params;
    this->SendHdrParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartLightBlend(const Ptr<LightParams>& params, Timing::Time blendTime)
{
    this->curLightParams = params;
    this->SendLightParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::StartSkyBlend(const Ptr<SkyParams>& params, Timing::Time blendTime)
{
    this->curSkyParams = params;
    this->SendSkyParameters(blendTime);
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectManager::StartAOBlend( const Ptr<AoParams>& params, Timing::Time blendTime )
{
    this->curAoParams = params;
    this->SendAOParameters(blendTime);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::SendColorParameters(Timing::Time blendTime)
{
    PostEffectServer::Instance()->StartBlending(this->curColorParams.upcast<ParamBase>(), blendTime, PostEffectServer::Color);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::SendHdrParameters(Timing::Time blendTime)
{
    PostEffectServer::Instance()->StartBlending(this->curHdrParams.upcast<ParamBase>(), blendTime, PostEffectServer::Hdr);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectManager::SendDepthOfFieldParameters(Timing::Time blendTime)
{   
    PostEffectServer::Instance()->StartBlending(this->curDepthOfFieldParams.upcast<ParamBase>(), blendTime, PostEffectServer::DoF);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::SendFogParameters(Timing::Time blendTime)
{
    PostEffectServer::Instance()->StartBlending(this->curFogParams.upcast<ParamBase>(), blendTime, PostEffectServer::Fog);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::SendLightParameters(Timing::Time blendTime)
{
    PostEffectServer::Instance()->StartBlending(this->curLightParams.upcast<ParamBase>(), blendTime, PostEffectServer::Light);
}

//------------------------------------------------------------------------------
/**    
*/
void
PostEffectManager::SendSkyParameters(Timing::Time blendTime)
{
    PostEffectServer::Instance()->StartBlending(this->curSkyParams.upcast<ParamBase>(), blendTime, PostEffectServer::Sky);
}
//------------------------------------------------------------------------------
/**
*/
void 
PostEffectManager::SendAOParameters( Timing::Time blendTime )
{
    PostEffectServer::Instance()->StartBlending(this->curAoParams.upcast<ParamBase>(), blendTime, PostEffectServer::AO);
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectManager::SendSky()
{
	// set sky entity in post server
    PostEffectServer::Instance()->SetSkyEntity(this->skyEntity);
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectManager::Fade( Timing::Time time, float start, float target )
{
    PostEffectServer::Instance()->StartFade(time, start, target);
}


} // namespace Server
