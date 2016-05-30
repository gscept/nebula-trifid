//------------------------------------------------------------------------------
//  posteffectserver.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "posteffect/rt/posteffectserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/modelentity.h"
#include "coregraphics/shaderserver.h"
#include "graphics/globallightentity.h"
#include "models/nodes/statenodeinstance.h"
#include "models/nodes/statenode.h"
#include "resources/managedtexture.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shadersemantics.h"
#include "frame/frameserver.h"
#include "graphics/graphicsinterface.h"
#include "algorithm/algorithmprotocol.h"



namespace PostEffect
{
__ImplementClass(PostEffect::PostEffectServer, 'POSR', Core::RefCounted);
__ImplementSingleton(PostEffectServer);

using namespace Util;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Resources;
using namespace Math;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
PostEffectServer::PostEffectServer():
    lastTime(0.0),
    fadeTime(0.0),
    currentFadeValue(1.0f),
    curFadeMode(NoFade),
    postEffects(NumPostEffectTypes),
    frameShader(0),
	skyEntity(0),	
    skyLoaded(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PostEffectServer::~PostEffectServer()
{
    this->Close();

    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectServer::Open()
{
    n_assert(!this->frameShader.isvalid());

    // get frame shader from default view
    this->frameShader = GraphicsServer::Instance()->GetDefaultView()->GetFrameShader();

	// some variables belong in the compose shader
	const Ptr<Shader>& composeShader = this->frameShader->GetFramePassBaseByName("Finalize")->GetShader();
    const Ptr<Shader>& gatherShader = this->frameShader->GetFramePassBaseByName("Gather")->GetShader();
	const Ptr<Shader>& vertBloom = this->frameShader->GetFramePassBaseByName("VerticalBloomBlur")->GetShader();
	const Ptr<Shader>& horiBloom = this->frameShader->GetFramePassBaseByName("HorizontalBloomBlur")->GetShader();

	// some variables belong in the brightpass
	const Ptr<Shader>& brightPassShader = this->frameShader->GetFramePassBaseByName("BrightPass")->GetShader();

    // lookup the shared post effect fade variable
    this->fadeShaderVariable = composeShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_FADEVALUE));
    
    // color stuff
    this->saturationShaderVariable = composeShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_SATURATION));
    this->balanceShaderVariable = composeShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_BALANCE));
    this->maxLuminanceShaderVar = composeShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_MAXLUMINANCE));

	// fog stuff
    this->fogColorVariable = gatherShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_FOGCOLOR));
    this->fogDistancesVariable = gatherShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_FOGDISTANCES));

	// dof stuff
    this->dofShaderVariable = composeShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_DOFDISTANCES));

    // hdr stuff
    this->hdrColorVariable = brightPassShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_HDRBLOOMCOLOR));
    this->hdrThresholdVariable = brightPassShader->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_HDRBRIGHTPASSTHRESHOLD));
    this->hdrVerticalScaleVariable = vertBloom->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_HDRBLOOMSCALE));
    this->hdrHorizontalScaleVariable = horiBloom->GetVariableByName(ShaderVariable::Name(NEBULA3_SEMANTIC_HDRBLOOMSCALE));

	// setup default blends
	this->postEffects[Color].current	= ColorParams::Create();
	this->postEffects[DoF].current		= DepthOfFieldParams::Create();
	this->postEffects[Fog].current		= FogParams::Create();
	this->postEffects[Hdr].current		= HdrParams::Create();
	this->postEffects[Light].current	= LightParams::Create();
	this->postEffects[Sky].current		= SkyParams::Create();
	this->postEffects[AO].current		= AoParams::Create();

	// preload default sky
	this->PreloadTexture("tex:system/sky_refl");
}


//------------------------------------------------------------------------------
/**
*/

bool
PostEffectServer::FindCurrentSkyEntities()
{
	// check if our sky entity is set
	if (this->skyEntity.isvalid())
	{
		// get model instance
		Ptr<ModelInstance> inst = this->skyEntity->GetModelInstance();

		if (inst.isvalid())
		{
			const Array<Ptr<ModelNodeInstance> > nodes = inst->GetNodeInstances();
			IndexT i;
			for (i = 0; i < nodes.Size(); i++)
			{
				const Ptr<ModelNodeInstance>& node = nodes[i];
				if (node->IsA(StateNodeInstance::RTTI))
				{
					const Ptr<StateNodeInstance>& stateNodeInst = node.downcast<StateNodeInstance>();
					const Ptr<StateNode>& stateNode = stateNodeInst->GetModelNode().downcast<StateNode>();
                    const Ptr<Materials::SurfaceInstance>& surface = stateNodeInst->GetSurfaceInstance();

					// create variables
                    this->skyBaseTexture = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_SKY1));
                    this->skyBlendTexture = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_SKY2));
                    this->skyBlendFactor = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_SKYBLENDFACTOR));
                    this->skyBrightness = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_BRIGHTNESS));
                    this->skyContrast = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_CONTRAST));
                    this->skyRotationFactor = surface->GetConstant(ShaderVariable::Name(NEBULA3_SEMANTIC_SKYROTATIONFACTOR));;

					// set base texture
					this->skyBaseTexture->SetTexture(this->FindTexture("tex:system/sky")->GetTexture());

					return true;
				}
			}
		}
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectServer::Close()
{
    // delete all posteffect structs
    IndexT index;
    for (index = 0; index < this->postEffects.Size(); index++)
    {
        this->postEffects[index].current = 0;
        this->postEffects[index].target = 0;
    }

    this->curFadeMode = NoFade;
    this->currentFadeValue = 1.0f;
    this->fadeShaderVariable = 0;
    this->saturationShaderVariable = 0;
	this->maxLuminanceShaderVar = 0;
    this->balanceShaderVariable = 0;
    this->fogDistancesVariable = 0;
    this->fogColorVariable = 0;
    this->hdrVerticalScaleVariable = 0;
	this->hdrHorizontalScaleVariable = 0;
    this->hdrColorVariable = 0;
    this->hdrThresholdVariable = 0;
    this->dofShaderVariable = 0;
	this->skyEntity = 0;
    this->skyContrast = 0;
    this->skyBrightness = 0;
    this->skyBlendFactor = 0;
    this->skyRotationFactor = 0;
    this->skyBlendTexture = 0;
    this->skyBaseTexture = 0;    
    this->globalLight = 0;
    this->skyLoaded = false;
    this->frameShader = 0;

    this->UnloadTextures();
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectServer::OnFrame(Timing::Time time)
{
    // if the shader variables are not yet connected to the sky shader, try
    if (!skyLoaded)
    {
        this->skyLoaded = this->FindCurrentSkyEntities();        
    }

	// make sure to apply sky parameters
    /*
	if (this->skyBlendTexture.isvalid() &&
		this->skyBaseTexture.isvalid() &&
		this->skyBlendTexture->IsValid() &&
		this->skyBaseTexture->IsValid())
	{
		this->skyBlendTexture->Apply();
		this->skyBaseTexture->Apply();
	}
    */

	Timing::Time frameTime = time - this->lastTime;
	this->lastTime = time;

	// compute simple fading
	if (this->curFadeMode != NoFade)
	{
		this->currentFadeValue = this->ComputeFadeValue(frameTime);
		// apply fadeValue on shader variable
		this->fadeShaderVariable->SetFloat(this->currentFadeValue);
	}
	else if (this->callback.IsValid())
	{
		this->callback(time);
		this->callback = Delegate<Timing::Time>();
	}

    // if no global light entity set, find the new one
    //   (can be used for setting a new, in case of changing the view
    //    the old one should be reseted and here we will find the new one)
    if (!this->globalLight.isvalid())
    {        
		this->globalLight = Graphics::GraphicsServer::Instance()->GetCurrentGlobalLightEntity();
        // return, the light still can be invalid
        return;
    }

    // now compute further post effect blendings
    IndexT index;
    for(index = 0; index < NumPostEffectTypes; index++)
    {
        if(this->ComputeBlending((PostEffectType)index))
        {
            this->ApplyParameters((PostEffectType)index);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
PostEffectServer::ComputeBlending(PostEffectType type)
{    
    const Ptr<ParamBase>& target = this->postEffects[(int)type].target;
    const Ptr<ParamBase>& current = this->postEffects[(int)type].current;
    
    if(target.isvalid())
    { 
		n_assert(current.isvalid());
        if (!current->Equals(target, TINY))
        {
            // how much have we progressed since last time?
            // and how much time left to the end?
            Timing::Time timeDiff = this->lastTime - this->postEffects[(int)type].lastTime;
            Timing::Time timeToEnd = n_max(this->postEffects[(int)type].blendEndTime - this->lastTime, 0.0001);

            // get normalized lerp value and perform blending
            float lerpVal = n_saturate(float(timeDiff / timeToEnd));
            current->BlendTo( target , lerpVal);

            // store current time for parameterblend
            this->postEffects[(int)type].lastTime = this->lastTime;

            return true;
        }
		else if (current->Equals(target, TINY))
		{
			// do not blend, just set directly
			this->postEffects[type].current->Copy(target);
            this->StopBlending(type);
            return true;
		}
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyParameters(PostEffectType type)
{   
    switch(type)
    {
        case Color:
            this->ApplyColorParameters();
            break;
        case DoF:
            this->ApplyDepthOfFieldParameters();
            break;
        case Fog:
            this->ApplyFogParameters();
            break;
        case Hdr:
            this->ApplyHdrParameters();
            break;
        case Sky:
            this->ApplySkyParameters();
            break;
        case Light:
            this->ApplyLightParameters();
            break;
        case AO:
            this->ApplyAOParameters();
            break;
        default:
            n_error("Wrong PostEffectParameter Type set!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyColorParameters()
{   
    this->balanceShaderVariable->SetFloat4(this->postEffects[Color].current.cast<ColorParams>()->GetColorBalance());
    this->saturationShaderVariable->SetFloat(this->postEffects[Color].current.cast<ColorParams>()->GetColorSaturation());
	this->maxLuminanceShaderVar->SetFloat4(this->postEffects[Color].current.cast<ColorParams>()->GetColorMaxLuminance());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyDepthOfFieldParameters()
{
    // TODO
    Math::vector dofDistances(  this->postEffects[DoF].current.cast<DepthOfFieldParams>()->GetFocusDistance(),
                                this->postEffects[DoF].current.cast<DepthOfFieldParams>()->GetFocusLength(),
                                this->postEffects[DoF].current.cast<DepthOfFieldParams>()->GetFilterSize());
	this->dofShaderVariable->SetFloat4(dofDistances);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyFogParameters()
{
    this->fogColorVariable->SetFloat4(this->postEffects[Fog].current.cast<FogParams>()->GetFogColorAndIntensity());
    // build distances stuff
    Math::float4 fogDistances(  this->postEffects[Fog].current.cast<FogParams>()->GetFogNearDistance(),
                                this->postEffects[Fog].current.cast<FogParams>()->GetFogFarDistance(),
                                this->postEffects[Fog].current.cast<FogParams>()->GetFogHeight(),
                                this->postEffects[Fog].current.cast<FogParams>()->GetFogFarDistance() - this->postEffects[Fog].current.cast<FogParams>()->GetFogNearDistance());
    this->fogDistancesVariable->SetFloat4(fogDistances);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyHdrParameters()
{
    this->hdrColorVariable->SetFloat4(this->postEffects[Hdr].current.cast<HdrParams>()->GetHdrBloomColor());
	this->hdrVerticalScaleVariable->SetFloat(this->postEffects[Hdr].current.cast<HdrParams>()->GetHdrBloomIntensity());
	this->hdrHorizontalScaleVariable->SetFloat(this->postEffects[Hdr].current.cast<HdrParams>()->GetHdrBloomIntensity());
    this->hdrThresholdVariable->SetFloat(this->postEffects[Hdr].current.cast<HdrParams>()->GetHdrBloomThreshold());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplyLightParameters()
{
    // do not apply any shader variables,
    // modify globallight instead
    const Ptr<LightParams>& paras = this->postEffects[Light].current.cast<LightParams>();
    this->globalLight->SetColor(paras->GetLightColor() * paras->GetLightIntensity());
    this->globalLight->SetAmbientLightColor(paras->GetLightAmbientColor() * paras->GetLightIntensity());
    this->globalLight->SetBackLightColor(paras->GetLightOppositeColor() * paras->GetLightIntensity());
#ifdef USE_POSTEFFECT_GLOBALLIGHTTRANSFORM
    this->globalLight->SetTransform(paras->GetLightTransform());    
#endif
    this->globalLight->SetShadowIntensity(paras->GetLightShadowIntensity());
	this->globalLight->SetShadowBias(paras->GetLightShadowBias());
	this->globalLight->SetCastShadows(paras->GetLightCastsShadows());
    this->globalLight->SetBackLightOffset(paras->GetBackLightFactor());
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectServer::ApplyAOParameters()
{
    const Ptr<AoParams>& paras = this->postEffects[AO].current.cast<AoParams>();
    Ptr<Algorithm::SetAmbientOcclusionParams> msg = Algorithm::SetAmbientOcclusionParams::Create();
    msg->SetStrength(paras->GetStrength());
    msg->SetAngleBias(paras->GetAngleBias());
    msg->SetPowerExponent(paras->GetPower());
    msg->SetRadius(paras->GetRadius());
    Graphics::GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::ApplySkyParameters()
{
    // if sky isn't loaded yet, do nothing     
    if (!this->skyLoaded || !this->skyEntity->GetModelInstance().isvalid())
    {
		// return
        return;
    }

    // get textures
    const Ptr<SkyParams>& targetPara = this->postEffects[Sky].target.cast<SkyParams>();
    const Ptr<SkyParams>& currentPara = this->postEffects[Sky].current.cast<SkyParams>();
	
	// preload textures
	this->PreloadTexture(targetPara->GetSkyTexturePath());
	this->PreloadTexture(currentPara->GetSkyTexturePath());

    this->skyBrightness->SetValue(currentPara->GetSkyBrightness());
    this->skyContrast->SetValue(currentPara->GetSkyContrast());
    this->skyRotationFactor->SetValue(currentPara->GetSkyRotationFactor());    
    
    // if blending finished, apply blend texture as current
    if(1.0f - currentPara->GetTextureBlendFactor() <= N_TINY || targetPara == currentPara)
    {        
        // set current as target, reset blend factor and delete target
        currentPara->ResetTextureBlendFactor();
        currentPara->SetSkyTexturePath(targetPara->GetSkyTexturePath());
        currentPara->SetIrradianceTexturePath(targetPara->GetIrradianceTexturePath());
        currentPara->SetReflectanceTexturePath(targetPara->GetReflectanceTexturePath());
        this->StopBlending(Sky);

		// set texture
		this->skyBaseTexture->SetTexture(this->FindTexture(currentPara->GetSkyTexturePath())->GetTexture());

        // set base texture, other one is not needed
        this->skyBlendFactor->SetValue(currentPara->GetTextureBlendFactor());
        // apply reflectance and irradiance
        Lighting::EnvironmentProbe::DefaultEnvironmentProbe->AssignReflectionMap(currentPara->GetReflectanceTexturePath() + NEBULA3_TEXTURE_EXTENSION);
        Lighting::EnvironmentProbe::DefaultEnvironmentProbe->AssignIrradianceMap(currentPara->GetIrradianceTexturePath() + NEBULA3_TEXTURE_EXTENSION);
    }
    else
    {
		// set blend texture
		this->skyBlendTexture->SetTexture(this->FindTexture(targetPara->GetSkyTexturePath())->GetTexture());       

	    // set base and blend texture
        this->skyBlendFactor->SetValue(currentPara->GetTextureBlendFactor());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::StartBlending(const Ptr<ParamBase>& target, Timing::Time fadeTime, PostEffectType postEffectType)
{
    //n_printf("PostEffectServer: Starting Blending (%i) at '%f' - fadeTime (%f)\n", (int)postEffectType, this->lastTime, fadeTime);

    // update times
    this->postEffects[postEffectType].blendTime = fadeTime;
    this->postEffects[postEffectType].blendEndTime = this->lastTime + fadeTime;
    this->postEffects[postEffectType].lastTime = this->lastTime;

    // set new target
    this->postEffects[postEffectType].target = target;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::StopBlending(PostEffectType postEffectType)
{
    //n_printf("PostEffectServer: Stopping Blending (%i) at '%f'\n", (int)postEffectType, this->lastTime);

    this->postEffects[(int)postEffectType].target = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::PreloadTexture(const Util::String &resource)
{
    // soft check
    ResourceId fullTexResId = String(resource + NEBULA3_TEXTURE_EXTENSION);
    Ptr<ManagedTexture> managedTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, fullTexResId, 0, true).downcast<ManagedTexture>();
	ResourceManager::Instance()->RequestResourceForLoading(managedTexture.upcast<ManagedResource>());   
	this->texturePool.Append(managedTexture);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::UnloadTexture(const Util::String &resource)
{
	ResourceId fullTexResId = String(resource + NEBULA3_TEXTURE_EXTENSION);
	Ptr<ManagedTexture> tex = ResourceManager::Instance()->LookupManagedResource(fullTexResId).downcast<ManagedTexture>();
	ResourceManager::Instance()->DiscardManagedResource(tex.upcast<ManagedResource>());

	// just remove ANY reference to this texture, we don't really care which pointer we have
	this->texturePool.EraseIndex(this->texturePool.FindIndex(tex));
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ManagedTexture>
PostEffectServer::FindTexture(const Util::String& resource)
{
	ResourceId fullTexResId = String(resource + NEBULA3_TEXTURE_EXTENSION);
	return ResourceManager::Instance()->LookupManagedResource(fullTexResId).downcast<ManagedTexture>();
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::UnloadTextures()
{
	IndexT i;
	for (i = 0; i < this->texturePool.Size(); i++)
	{
		ResourceManager::Instance()->DiscardManagedResource(this->texturePool[i].upcast<ManagedResource>());
	}
    this->texturePool.Clear();
}

//------------------------------------------------------------------------------
/**
*/
float 
PostEffectServer::ComputeFadeValue(Timing::Time frameTime)
{
    float result = this->currentFadeValue;
    if (this->curFadeMode == FadeOut)
    {
        const float targetValue = this->currentFadeTarget;
        if (this->fadeTime == 0.0f)
        {
            result = targetValue;
        }
        if (result <= targetValue)
        {
            this->curFadeMode = NoFade;
            return targetValue;
        }
        else
        {
            // always compute time in ratio of full black and full transparent
            float stepSize = (float)frameTime / (float)this->fadeTime; 
            result -= stepSize;
        }
    }
    else
    {
        const float targetValue = this->currentFadeTarget;
        if (this->fadeTime == 0.0f)
        {
            result = targetValue;
        }
        if (result >= targetValue)
        {
            this->curFadeMode = NoFade;
            return targetValue;
        }
        else
        {
            // always compute time in ratio of full black and full transparent
            float stepSize = (float)frameTime / (float)this->fadeTime; 
            result += stepSize;
        }
    }



    return result;
}

//------------------------------------------------------------------------------
/**
*/
void 
PostEffectServer::SetSkyEntity( const Ptr<Graphics::ModelEntity>& entity )
{
	n_assert(entity.isvalid());

	// remove current variables
	if (this->skyEntity.isvalid())
	{
		this->skyContrast = 0;
		this->skyBrightness = 0;
		this->skyBlendFactor = 0;
        this->skyRotationFactor = 0;
		this->skyBlendTexture = 0;
		this->skyBaseTexture = 0;
		this->skyLoaded = false;
	}
	
	this->skyEntity = entity;
	this->skyEntity->SetAlwaysVisible(true);
}

//------------------------------------------------------------------------------
/**
*/
void
PostEffectServer::StopAllBlending()
{
	for (int i = 0; i < this->postEffects.Size(); i++)
	{
		this->postEffects[i].target = 0;
	}
}

} // namespace PostEffect
