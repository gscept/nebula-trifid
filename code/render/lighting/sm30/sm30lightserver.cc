//------------------------------------------------------------------------------
//  sm30lightserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lighting/sm30/sm30lightserver.h"
#include "coregraphics/shaderserver.h"
#include "resources/resourcemanager.h"
#include "lighting/shadowserver.h"
#include "util/priorityarray.h"

namespace Lighting
{
__ImplementClass(Lighting::SM30LightServer, 'S3LS', Lighting::LightServerBase);

using namespace Math;
using namespace Util;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
SM30LightServer::SM30LightServer() :
    shdFeatureMasks(MaxLocalLights + 1, 0)
{
    Memory::Clear(this->lightPos, sizeof(this->lightPos));
    Memory::Clear(this->lightInvRange, sizeof(this->lightInvRange));
    Memory::Clear(this->lightProjTransform, sizeof(this->lightProjTransform));
    Memory::Clear(this->lightColor, sizeof(this->lightColor));
    Memory::Clear(this->lightProjMapUvOffset, sizeof(this->lightProjMapUvOffset));
    Memory::Clear(this->lightType, sizeof(this->lightType));
    Memory::Clear(this->lightCastShadows, sizeof(this->lightCastShadows));
    Memory::Clear(this->lightShadowBufferUvOffset, sizeof(this->lightShadowBufferUvOffset));
}

//------------------------------------------------------------------------------
/**
*/
SM30LightServer::~SM30LightServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
SM30LightServer::Open()
{
    // call parent class
    LightServerBase::Open();

    String lightTexPath("systex:lighting/lightcones");
    lightTexPath.Append(NEBULA3_TEXTURE_EXTENSION);

    // setup the shared light project map resource
    this->lightProjMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(lightTexPath)).downcast<ManagedTexture>();

    // setup shared shader variables
    ShaderServer* shdServer = ShaderServer::Instance();
    this->shdGlobalLightDir              = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightDirection"));
    this->shdGlobalLightColor            = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightColor"));
    this->shdGlobalBackLightColor        = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalBackLightColor"));
    this->shdGlobalBackLightOffset       = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalBackLightOffset"));
    this->shdGlobalAmbientLightColor     = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalAmbientLightColor"));
    this->shdGlobalLightCastShadows      = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightCastShadows"));
    this->shdGlobalLightPSSMDistances    = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightPSSMDistances"));
    this->shdGlobalLightPSSMTransforms   = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightPSSMTransforms"));
    this->shdGlobalLightPSSMShadowBuffer = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("GlobalLightPSSMShadowBuffer"));
    this->shdLightPos                    = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightPosition"));
    this->shdLightInvRange               = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightInvRange"));
    this->shdLightProjTransform          = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightProjTransform"));
    this->shdLightColor                  = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightColor"));
    this->shdLightProjMapUvOffset        = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightProjMapUvOffset"));
    this->shdLightType                   = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightType"));
    this->shdLightCastShadows            = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightCastShadows"));
    this->shdLightShadowBufferUvOffset   = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightShadowBufferUvOffset"));
    this->shdLightShadowBuffer           = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightShadowBuffer"));
    this->shdLightShadowBufferSize       = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightShadowBufferSize"));
    this->shdLightProjMap                = shdServer->GetSharedVariableBySemantic(ShaderVariable::Semantic("LightProjMap"));

    // setup the shader feature masks for number of local lights
    // (this is used to select the right shader variation by number of lights)
    this->allLightsFeatureMask = 0;
    IndexT i;
    for (i = 0; i <= MaxLocalLights; i++)
    {
        String featureName;
        featureName.Format("LocalLights%d", i);
        this->shdFeatureMasks[i] = shdServer->FeatureStringToMask(featureName);
        this->allLightsFeatureMask |= this->shdFeatureMasks[i];
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SM30LightServer::Close()
{
    // discard resources
    ResourceManager::Instance()->DiscardManagedResource(this->lightProjMap.upcast<ManagedResource>());
    this->lightProjMap = 0;

    // discard shader variables
    this->shdGlobalLightDir = 0;
    this->shdGlobalLightColor = 0;
    this->shdGlobalBackLightColor = 0;
    this->shdGlobalBackLightOffset = 0;
    this->shdGlobalAmbientLightColor = 0;
    this->shdGlobalLightCastShadows = 0;
    this->shdGlobalLightPSSMDistances = 0;
    this->shdGlobalLightPSSMTransforms = 0;
    this->shdGlobalLightPSSMShadowBuffer = 0;
    this->shdLightPos = 0;
    this->shdLightInvRange = 0;
    this->shdLightProjTransform = 0;
    this->shdLightColor = 0;
    this->shdLightProjMapUvOffset = 0;
    this->shdLightType = 0;
    this->shdLightCastShadows = 0;
    this->shdLightShadowBufferUvOffset = 0;
    this->shdLightShadowBuffer = 0;
    this->shdLightShadowBufferSize = 0;
    this->shdLightProjMap = 0;

    // call parent class
    LightServerBase::Close();
}

//------------------------------------------------------------------------------
/**
    @todo: set light properties only once per-frame and only set a
    bool array with active per-model-entity-lights here!
*/
void
SM30LightServer::ApplyModelEntityLights(const Ptr<ModelEntity>& modelEntity)
{
    // get light entities which influence the model entity
    Array<Ptr<GraphicsEntity> > localLights = modelEntity->GetLinks(GraphicsEntity::LightLink);
	
    // do just, if there is at least one light
	SizeT numLocalLights = localLights.Size();
    if(numLocalLights > 0)
    {
        // FIXME: check and remove globallight from locallight array   
        if (localLights[0]->IsA(GlobalLightEntity::RTTI))
        {
	        localLights.EraseIndex(0);
        }
   	    numLocalLights = localLights.Size();

	    IndexT lightIndex;
        PriorityArray<IndexT> prioLightArray(MaxLocalLights);
        // if model influenced by more then max number of lights,
        // sort by priority and drop the least-priority-lights
        bool prioritizeLights = localLights.Size() > MaxLocalLights;
        if (prioritizeLights)
        {    
            for (lightIndex = 0; lightIndex < localLights.Size(); lightIndex++)
            {       
                const Ptr<AbstractLightEntity>& lightEntity = localLights[lightIndex].downcast<AbstractLightEntity>();                    
                vector distVec = lightEntity->GetTransform().getrow3() - modelEntity->GetTransform().getrow3();
                float dist = distVec.length();
                float range = lightEntity->GetTransform().getrow2().length();
                float priority = -(dist / range);
                prioLightArray.Add(lightIndex, priority);

                // constrain numLights to MaxLights
                numLocalLights = MaxLocalLights;
            }
        }
        // let each light source prepare for applying shader params
        IndexT i;
        for (i = 0; i < numLocalLights; i++)
        {  
            IndexT mappedIndex = prioritizeLights ? prioLightArray[i] : i;
            const Ptr<AbstractLightEntity>& lightEntity = localLights[mappedIndex].downcast<AbstractLightEntity>();
            this->lightPos[i]             = lightEntity->GetTransform().getrow3();
            this->lightInvRange[i]        = 1.0f / lightEntity->GetTransform().getrow2().length();
            this->lightProjTransform[i]   = lightEntity->GetInvLightProjTransform();
            this->lightColor[i]           = lightEntity->GetColor();
            this->lightProjMapUvOffset[i] = lightEntity->GetProjMapUvOffsetAndScale();
            this->lightType[i]            = lightEntity->GetLightType();
            if (lightEntity->GetCastShadows())
            {
                this->lightCastShadows[i] = true;
                this->lightShadowBufferUvOffset[i] = lightEntity->GetShadowBufferUvOffsetAndScale();
            }
            else
            {
                this->lightCastShadows[i] = false;
                this->lightShadowBufferUvOffset[i].set(0.5f, 0.5f, 0.5f, -0.5f);
            }
       }

        // apply shader parameters
        const Ptr<ShadowServer>& shadowServer = ShadowServer::Instance();
        const Ptr<Texture>& sbTexture = shadowServer->GetSpotLightShadowBufferTexture();
        float4 sbSize(float(sbTexture->GetWidth()), float(sbTexture->GetHeight()), 0.0f, 0.0f);

	    // local lights
        this->shdLightPos->SetFloat4Array(this->lightPos, numLocalLights);
        this->shdLightInvRange->SetFloatArray(this->lightInvRange, numLocalLights);
        this->shdLightProjTransform->SetMatrixArray(this->lightProjTransform, numLocalLights);
        this->shdLightColor->SetFloat4Array(this->lightColor, numLocalLights);
        this->shdLightProjMapUvOffset->SetFloat4Array(this->lightProjMapUvOffset, numLocalLights);
        this->shdLightType->SetIntArray((int*) this->lightType, numLocalLights);
        this->shdLightCastShadows->SetBoolArray(this->lightCastShadows, numLocalLights);
        this->shdLightShadowBufferUvOffset->SetFloat4Array(this->lightShadowBufferUvOffset, numLocalLights);
        this->shdLightShadowBuffer->SetTexture(sbTexture);
        this->shdLightShadowBufferSize->SetFloat4(sbSize);
        this->shdLightProjMap->SetTexture(this->lightProjMap->GetTexture());

        // global light
	    if (this->globalLightEntity.isvalid())
        {
            this->shdGlobalLightDir->SetFloat4(this->globalLightEntity->GetLightDirection());
            this->shdGlobalLightColor->SetFloat4(this->globalLightEntity->GetColor());
            this->shdGlobalAmbientLightColor->SetFloat4(this->globalLightEntity->GetAmbientLightColor());
            this->shdGlobalBackLightColor->SetFloat4(this->globalLightEntity->GetBackLightColor());
            this->shdGlobalBackLightOffset->SetFloat(this->globalLightEntity->GetBackLightOffset());
            this->shdGlobalLightCastShadows->SetBool(this->globalLightEntity->GetCastShadows());
            //this->shdGlobalLightPSSMShadowBuffer->SetTexture(shadowServer->GetGlobalLightShadowBufferTexture());
            //this->shdGlobalLightPSSMDistances->SetFloatArray(shadowServer->GetPSSMSplitDistances(), PSSMUtil::NumSplits + 1);
            //this->shdGlobalLightPSSMTransforms->SetMatrixArray(shadowServer->GetPSSMSplitLightProjTransforms(), PSSMUtil::NumSplits);
        }
        else
        {
            // no global light set, still need to setup shader variables
            this->shdGlobalLightDir->SetFloat4(-vector::upvec());
            this->shdGlobalLightColor->SetFloat4(vector::nullvec());
            this->shdGlobalBackLightColor->SetFloat4(vector::nullvec());
            this->shdGlobalLightCastShadows->SetBool(false);
        }
    }

    // set feature bit mask for the number of light sources, this is used
    // to select the right shader variation for the number of active light sources
    const Ptr<ShaderServer>& shdServer = ShaderServer::Instance();
    shdServer->ClearFeatureBits(this->allLightsFeatureMask);
    shdServer->SetFeatureBits(this->shdFeatureMasks[numLocalLights]);
}

} // namespace Lighting
