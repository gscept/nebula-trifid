//------------------------------------------------------------------------------
//  sm50lightserver.cc
//  (C) 2011-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lighting/sm50/sm50lightserver.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/texture.h"
#include "lighting/shadowserver.h"
#include "coregraphics/transformdevice.h"
#include "sm50shadowserver.h"
#include "math/float4.h"
#include "frame/frameserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "coregraphics/constantbuffer.h"

namespace Lighting
{
__ImplementClass(Lighting::SM50LightServer, 'SM5L', Lighting::LightServerBase);

using namespace Graphics;
using namespace CoreGraphics;
using namespace Resources;
using namespace Frame;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
SM50LightServer::SM50LightServer() :
	renderBuffersAssigned(false)
{
	globalLightFeatureBits[NoShadows] = 0;
	globalLightFeatureBits[CastShadows] = 0;
	pointLightFeatureBits[NoShadows] = 0;
	pointLightFeatureBits[CastShadows] = 0;
	spotLightFeatureBits[NoShadows] = 0;
	spotLightFeatureBits[CastShadows] = 0;
}

//------------------------------------------------------------------------------
/**
*/
SM50LightServer::~SM50LightServer()
{
	n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
bool 
SM50LightServer::NeedsLightModelLinking() const
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
SM50LightServer::Open()
{
	n_assert(!this->IsOpen());
	ResourceManager* resManager = ResourceManager::Instance();
	ShaderServer* shdServer = ShaderServer::Instance();

	// call parent class
	LightServerBase::Open();

	// load the light source shapes
	this->pointLightMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/pointlightshape.nvx2")).downcast<ManagedMesh>();
	this->spotLightMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/spotlightshape.nvx2")).downcast<ManagedMesh>();
	this->lightProbeMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/box.nvx2")).downcast<ManagedMesh>();

	Util::String lightTexPath("tex:system/white");
	lightTexPath.Append(NEBULA3_TEXTURE_EXTENSION);

	// setup the shared light project map resource
	this->lightProjMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(lightTexPath)).downcast<ManagedTexture>();

	this->lightShader							= shdServer->GetShader("shd:lights");
	this->lightProbeShader						= shdServer->GetShader("shd:reflectionprojector");

	this->globalLightFeatureBits[NoShadows]		= shdServer->FeatureStringToMask("Global");
	this->globalLightFeatureBits[CastShadows]	= shdServer->FeatureStringToMask("Global|Alt0");
	this->pointLightFeatureBits[NoShadows]		= shdServer->FeatureStringToMask("Point");    
	this->pointLightFeatureBits[CastShadows]	= shdServer->FeatureStringToMask("Point|Alt0");    
	this->spotLightFeatureBits[NoShadows]		= shdServer->FeatureStringToMask("Spot"); 
	this->spotLightFeatureBits[CastShadows]		= shdServer->FeatureStringToMask("Spot|Alt0");

	// todo: rename variations in shader...
	this->lightProbeFeatureBits[LightProbeEntity::Box] = shdServer->FeatureStringToMask("Alt0");
	this->lightProbeFeatureBits[LightProbeEntity::Sphere] = shdServer->FeatureStringToMask("Alt1");
	this->lightProbeFeatureBits[LightProbeEntity::Box + 2] = shdServer->FeatureStringToMask("Alt0|Alt2");
	this->lightProbeFeatureBits[LightProbeEntity::Sphere + 2] = shdServer->FeatureStringToMask("Alt1|Alt2");

	// global light variables used for shadowing
	this->globalLightCascadeOffset				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_CASCADEOFFSET);
	this->globalLightCascadeScale				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_CASCADESCALE);
	this->globalLightMinBorderPadding			= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_MINBORDERPADDING);
	this->globalLightMaxBorderPadding			= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_MAXBORDERPADDING);
	this->globalLightPartitionSize				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWPARTITIONSIZE);

    // setup block for global light
    this->globalLightBlockVar                   = this->lightShader->GetVariableByName("GlobalLightBlock");
    this->globalLightBuffer                     = ConstantBuffer::Create();
    this->globalLightBuffer->SetupFromBlockInShader(this->lightShader, "GlobalLightBlock");
    this->globalLightDir                        = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTDIR);
    this->globalLightColor                      = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_GLOBALLIGHTCOLOR);
    this->globalBackLightColor                  = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_GLOBALBACKLIGHTCOLOR);
    this->globalAmbientLightColor               = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_GLOBALAMBIENTLIGHTCOLOR);
    this->globalBackLightOffset                 = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_GLOBALBACKLIGHTOFFSET);
    this->globalLightShadowMatrixVar            = this->globalLightBuffer->GetVariableByName(NEBULA3_SEMANTIC_CSMSHADOWMATRIX);
    this->globalLightBlockVar->SetBufferHandle(this->globalLightBuffer->GetHandle());

	// local light variables
	this->lightPosRange							= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPOSRANGE);
	this->lightColor             				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTCOLOR);
	this->lightProjTransform     				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPROJTRANSFORM);
	this->lightProjMapVar						= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPROJMAP); 
	this->lightProjCubeVar						= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPROJCUBE); 
	this->shadowProjMapVar						= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWPROJMAP);
    this->shadowProjCubeVar                     = this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWPROJCUBE);
	this->shadowProjTransform    				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWPROJTRANSFORM);
	this->shadowOffsetScaleVar   				= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWOFFSETSCALE);
	this->shadowIntensityVar          			= this->lightShader->GetVariableByName(NEBULA3_SEMANTIC_SHADOWINTENSITY);

	//this->shadowConstants->SetFloat4(float4(100.0f, 100.0f, 0.003f, 1024.0f));
}

//------------------------------------------------------------------------------
/**
*/
void
SM50LightServer::Close()
{
	n_assert(this->IsOpen());
	n_assert(this->pointLights[NoShadows].IsEmpty());   
	n_assert(this->pointLights[CastShadows].IsEmpty());
	n_assert(this->spotLights[NoShadows].IsEmpty());
	n_assert(this->spotLights[CastShadows].IsEmpty());

	if (this->renderBuffersAssigned)
	{
		// discard fullscreen quad renderer
		this->fullScreenQuadRenderer.Discard();
	}	

	// discard shader stuff
	this->lightShader = 0;

	this->lightPosRange = 0;
	this->lightColor = 0;
	this->lightProjTransform = 0;
	this->lightProjMapVar = 0;

	this->shadowIntensityVar = 0;
	this->shadowProjTransform = 0;
	this->shadowOffsetScaleVar = 0;
	this->shadowProjMapVar = 0;

    this->globalLightBuffer->Discard();
    this->globalLightBuffer = 0;
    this->globalLightBlockVar = 0;
	this->globalAmbientLightColor = 0;
	this->globalBackLightColor = 0;
	this->globalBackLightOffset = 0;
	this->globalLightColor = 0;
	this->globalLightDir = 0;
	this->globalLightColor = 0;
	this->globalLightCascadeOffset = 0;
	this->globalLightCascadeScale = 0;				
	this->globalLightMinBorderPadding = 0;
	this->globalLightMaxBorderPadding = 0;		
	this->globalLightPartitionSize = 0;			

	// discard resources
	ResourceManager* resManager = ResourceManager::Instance();
	resManager->DiscardManagedResource(this->pointLightMesh.upcast<ManagedResource>());
	this->pointLightMesh = 0;
	resManager->DiscardManagedResource(this->spotLightMesh.upcast<ManagedResource>());
	this->spotLightMesh = 0;
	resManager->DiscardManagedResource(this->lightProbeMesh.upcast<ManagedResource>());
	this->lightProbeMesh = 0;

	LightServerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::AttachVisibleLight( const Ptr<AbstractLightEntity>& lightEntity )
{
	if (lightEntity->GetLightType() == LightType::Point)
	{
		// TODO: allow shadow casting spotlights, define shadow casting transform?
		n_assert(lightEntity->IsInstanceOf(PointLightEntity::RTTI));
		if (lightEntity->GetCastShadowsThisFrame())
		{
			this->pointLights[CastShadows].Append(lightEntity.cast<PointLightEntity>());
		}
		else
		{
			this->pointLights[NoShadows].Append(lightEntity.cast<PointLightEntity>());
		}
	}
	else if (lightEntity->GetLightType() == LightType::Spot)
	{
		// TODO: sort out the three most important shadow casting lights
		n_assert(lightEntity->IsInstanceOf(SpotLightEntity::RTTI));
		if (lightEntity->GetCastShadowsThisFrame())
		{
			this->spotLights[CastShadows].Append(lightEntity.cast<SpotLightEntity>());
		}
		else
		{
			this->spotLights[NoShadows].Append(lightEntity.cast<SpotLightEntity>());
		}
	}
	LightServerBase::AttachVisibleLight(lightEntity);
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::EndFrame()
{
	this->pointLights[NoShadows].Clear();
	this->pointLights[CastShadows].Clear();
	this->spotLights[NoShadows].Clear();
	this->spotLights[CastShadows].Clear();
	LightServerBase::EndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::AssignRenderBufferTextures()
{
	n_assert(!this->renderBuffersAssigned);
	const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
	const Ptr<FrameServer> frameServer = FrameServer::Instance();
	this->renderBuffersAssigned = true;

	ResourceId lightBufferId("LightBuffer");
	if (!resManager->HasResource(lightBufferId))
	{
		n_error("SM50LightServer::RenderLights(): LightBuffer render target not found!\n");
	}
	Ptr<Texture> lightBuffer = resManager->LookupResource(lightBufferId).downcast<Texture>();

	// also setup the fullscreen quad renderer here
	this->fullScreenQuadRenderer.Setup(lightBuffer->GetWidth(), lightBuffer->GetHeight());       
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::RenderLights()
{
	ShaderServer* shdServer = ShaderServer::Instance();
    ShadowServer* shadowServer = ShadowServer::Instance();

	// if not happened yet, set the NormalDepthBuffer and LightBuffer
	// as shader variables
	if (!this->renderBuffersAssigned)
	{
		this->AssignRenderBufferTextures();
	}

	// general preparations
	shdServer->SetActiveShader(this->lightShader);

	// render the global light
    this->shadowProjMapVar->SetTexture(shadowServer->GetGlobalLightShadowBufferTexture());
	this->RenderGlobalLight();

	if (this->spotLights[CastShadows].Size() > 0)
	{      
		// now set shadow buffer for local lights    
		this->shadowProjMapVar->SetTexture(shadowServer->GetSpotLightShadowBufferTexture());
	}	
	
	// render point lights
	this->RenderPointLights();

	// render spot lights
	this->RenderSpotLights();
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::RenderGlobalLight()
{
	if (this->globalLightEntity.isvalid())
	{
		TransformDevice* transDev = TransformDevice::Instance();
		const matrix44& view = transDev->GetViewTransform();
		float4 worldSpaceLightDir = this->globalLightEntity->GetLightDirection();
		float4 viewSpaceLightDir = matrix44::transform(worldSpaceLightDir, view);

		// normalize was done in fragment shader, better here for gfx card's sake
		viewSpaceLightDir = float4::normalize(viewSpaceLightDir);

        // begin pass
        if (this->globalLightEntity->GetCastShadows()) this->lightShader->SelectActiveVariation(this->globalLightFeatureBits[CastShadows]);
        else                                           this->lightShader->SelectActiveVariation(this->globalLightFeatureBits[NoShadows]);

        // start pass
        this->lightShader->Apply();
		
		// setup general global light stuff
        this->globalLightBuffer->BeginUpdateSync();
		this->globalLightDir->SetFloat4(viewSpaceLightDir);
		this->globalLightColor->SetFloat4(this->globalLightEntity->GetColor());
		this->globalBackLightColor->SetFloat4(this->globalLightEntity->GetBackLightColor());
		this->globalAmbientLightColor->SetFloat4(this->globalLightEntity->GetAmbientLightColor());
		this->globalBackLightOffset->SetFloat(this->globalLightEntity->GetBackLightOffset());

		matrix44 shadowView = *ShadowServer::Instance()->GetShadowView();
        shadowView = matrix44::multiply(transDev->GetInvViewTransform(), shadowView);
        this->globalLightShadowMatrixVar->SetMatrix(shadowView);
		
		// handle casting shadows using CSM
		if (this->globalLightEntity->GetCastShadows())
		{
			Ptr<CoreGraphics::Texture> CSMTexture = ShadowServer::Instance()->GetGlobalLightShadowBufferTexture();
			float CSMBufferWidth = (CSMTexture->GetWidth() / (float)ShadowServerBase::SplitsPerRow);
#if __DX11__
			matrix44 textureScale = matrix44::scaling(0.5f, -0.5f, 1.0f);
#elif __OGL4__
			matrix44 textureScale = matrix44::scaling(0.5f, 0.5f, 1.0f);
#endif
			matrix44 textureTranslation = matrix44::translation(0.5f, 0.5f, 0);
			const float* CSMDistances = ShadowServer::Instance()->GetSplitDistances();
			const matrix44* CSMTransforms = ShadowServer::Instance()->GetSplitTransforms();
			float4 cascadeScales[CSMUtil::NumCascades];
			float4 cascadeOffsets[CSMUtil::NumCascades];
			for (int splitIndex = 0; splitIndex < CSMUtil::NumCascades; ++splitIndex)
			{
				matrix44 shadowTexture = matrix44::multiply(
					CSMTransforms[splitIndex],
					matrix44::multiply(textureScale, textureTranslation));
				float4 scale;
				scale.x() = shadowTexture.getrow0().x();
				scale.y() = shadowTexture.getrow1().y();
				scale.z() = shadowTexture.getrow2().z();
				scale.w() = 1;
				float4 offset = shadowTexture.getrow3();
				offset.w() = 0;
				cascadeOffsets[splitIndex] = offset;
				cascadeScales[splitIndex] = scale;
			}
			this->globalLightCascadeOffset->SetFloat4Array(cascadeOffsets, CSMUtil::NumCascades);
			this->globalLightCascadeScale->SetFloat4Array(cascadeScales, CSMUtil::NumCascades);
			this->globalLightMinBorderPadding->SetFloat(1.0f / float(CSMBufferWidth));
			this->globalLightMaxBorderPadding->SetFloat((CSMBufferWidth - 1.0f) / float(CSMBufferWidth));
			this->globalLightPartitionSize->SetFloat(1 / float(ShadowServerBase::SplitsPerRow));

			this->shadowIntensityVar->SetFloat(this->globalLightEntity->GetShadowIntensity());
		}
		this->globalLightBuffer->EndUpdateSync();

		// commit changes
		this->lightShader->Commit();

		// render
		this->fullScreenQuadRenderer.Draw();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::RenderPointLights()
{
	if ((this->pointLightMesh->GetState() == Resource::Loaded)
		&& (this->pointLights[CastShadows].Size() > 0
		|| this->pointLights[NoShadows].Size() > 0))
	{
		TransformDevice* tformDevice = TransformDevice::Instance();
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shaderServer = ShaderServer::Instance();

		// apply mesh
		this->pointLightMesh->GetMesh()->ApplyPrimitives(0);

		IndexT shadowIdx;
		for (shadowIdx = 0; shadowIdx < NumShadowFlags; ++shadowIdx)
		{
			if(this->pointLights[shadowIdx].Size())
			{
				this->lightShader->SelectActiveVariation(this->pointLightFeatureBits[shadowIdx]);
                this->lightShader->Apply();

				IndexT i;
				for (i = 0; i < this->pointLights[shadowIdx].Size(); i++)
				{            
					const Ptr<PointLightEntity>& curLight = this->pointLights[shadowIdx][i];
					const matrix44& lightTransform = curLight->GetTransform();
					tformDevice->SetModelTransform(lightTransform);

					// light position in view space, and set .w to inverted light range
					const matrix44& viewTransform = tformDevice->GetViewTransform();
					float4 posAndRange = matrix44::transform(lightTransform.get_position(), viewTransform);
					posAndRange.w() = 1.0f / lightTransform.get_zaxis().length();

					// set projection map
                    this->lightShader->BeginUpdate();
					if (curLight->GetProjectionTexture().isvalid())
					{
						this->lightProjCubeVar->SetTexture(curLight->GetProjectionTexture()->GetTexture());
					}

					this->lightPosRange->SetFloat4(posAndRange);
					this->lightColor->SetFloat4(curLight->GetColor());
					//this->lightShadowBias->SetFloat(curLight->GetShadowBias());

					if (CastShadows == (ShadowFlag)shadowIdx
						&& curLight->GetCastShadowsThisFrame())
					{   
                        // set shadow cube if valid
                        if (curLight->GetShadowCube().isvalid())
                        {
                            this->shadowProjCubeVar->SetTexture(curLight->GetShadowCube()->GetResolveTexture());
                        }

						// needed for tex coordinates to lookup correct shadowbuffer texel   
						const matrix44& invViewTransform = tformDevice->GetInvViewTransform();           
						matrix44 fromViewToLightProj = matrix44::multiply(invViewTransform, curLight->GetShadowInvLightProjTransform());            
						this->shadowProjTransform->SetMatrix(fromViewToLightProj);
						
						// set shadowmap offset index for multiple shadows
						const float4& shadowOffsetScale = curLight->GetShadowBufferUvOffsetAndScale();
						this->shadowOffsetScaleVar->SetFloat4(shadowOffsetScale);

						// set shadow intensity
						this->shadowIntensityVar->SetFloat(curLight->GetShadowIntensity());
					}

					// update shader variables
					tformDevice->ApplyModelTransforms(this->lightShader);
                    this->lightShader->EndUpdate();

					// commit and draw
					this->lightShader->Commit();
                    renderDevice->Draw();
				}
			}
		}                             
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
SM50LightServer::RenderSpotLights()
{
	if ((this->spotLightMesh->GetState() == Resource::Loaded)
		&& (this->spotLights[CastShadows].Size() > 0
		|| this->spotLights[NoShadows].Size() > 0))
	{
		TransformDevice* tformDevice = TransformDevice::Instance();
		RenderDevice* renderDevice = RenderDevice::Instance();

		// apply mesh
		this->spotLightMesh->GetMesh()->ApplyPrimitives(0);

		IndexT shadowIdx;
		for (shadowIdx = 0; shadowIdx < NumShadowFlags; ++shadowIdx)
		{
			if (this->spotLights[shadowIdx].Size())
			{
				this->lightShader->SelectActiveVariation(this->spotLightFeatureBits[shadowIdx]);
                this->lightShader->Apply();

				IndexT i;
				for (i = 0; i < this->spotLights[shadowIdx].Size(); i++)
				{            
					const Ptr<SpotLightEntity>& curLight = this->spotLights[shadowIdx][i];
					const matrix44& lightTransform = curLight->GetTransform();
					tformDevice->SetModelTransform(lightTransform);

					// light position in view space, and set .w to inverted light range
					const matrix44& viewTransform = tformDevice->GetViewTransform();
					float4 posAndRange = matrix44::transform(lightTransform.get_position(), viewTransform);
					posAndRange.w() = 1.0f / lightTransform.get_zaxis().length();

					// set projection map
                    this->lightShader->BeginUpdate();
					if (curLight->GetProjectionTexture().isvalid())
					{
						this->lightProjMapVar->SetTexture(curLight->GetProjectionTexture()->GetTexture());
					}					
					else
					{
						this->lightProjMapVar->SetTexture(this->lightProjMap->GetTexture());
					}

					this->lightPosRange->SetFloat4(posAndRange);
					this->lightColor->SetFloat4(curLight->GetColor());
					//this->lightShadowBias->SetFloat(curLight->GetShadowBias());

					// needed for tex coordinates to lookup correct spotlight lightmap texel
					const matrix44& invViewTransform = tformDevice->GetInvViewTransform();
					matrix44 fromViewToLightProj = matrix44::multiply(invViewTransform, curLight->GetInvLightProjTransform());            
					this->lightProjTransform->SetMatrix(fromViewToLightProj);

					if (CastShadows == (ShadowFlag)shadowIdx
						&& curLight->GetCastShadowsThisFrame())
					{                
						// needed for tex coordinates to lookup correct spotlight shadowmap texel
						matrix44 fromViewToShadowLightProj = matrix44::multiply(invViewTransform, curLight->GetShadowInvLightProjTransform());            
						this->shadowProjTransform->SetMatrix(fromViewToShadowLightProj);

						// set shadow map offset index for multiple shadows
						const float4& shadowOffsetScale = curLight->GetShadowBufferUvOffsetAndScale();
						this->shadowOffsetScaleVar->SetFloat4(shadowOffsetScale);

						// set shadow intensity
						this->shadowIntensityVar->SetFloat(curLight->GetShadowIntensity());
					}

					// update shader variables
					tformDevice->ApplyModelTransforms(this->lightShader);
                    this->lightShader->EndUpdate();

					// commit and draw
					this->lightShader->Commit();
					renderDevice->Draw();
				}
			}
		}                     
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
SortProbes(const Ptr<LightProbeEntity>& lhs, const Ptr<LightProbeEntity>& rhs)
{
	int lhsPrio = lhs->GetLayer();
	int rhsPrio = rhs->GetLayer();
	return lhsPrio < rhsPrio;
}

//------------------------------------------------------------------------------
/**
*/
void
SM50LightServer::RenderLightProbes()
{
	// sort light probes based on layers
	this->visibleLightProbes.SortWithFunc(SortProbes);

	// get transform and render device
	TransformDevice* transformDevice = TransformDevice::Instance();
	RenderDevice* renderDevice = RenderDevice::Instance();

	// apply mesh
	this->lightProbeMesh->GetMesh()->ApplyPrimitives(0);

	// traverse and render them!
	IndexT probeIdx;
	for (probeIdx = 0; probeIdx < this->visibleLightProbes.Size(); probeIdx++)
	{
		const Ptr<LightProbeEntity>& entity = this->visibleLightProbes[probeIdx];
		const Ptr<EnvironmentProbe>& probe = entity->GetEnvironmentProbe();
        const Ptr<CoreGraphics::Shader>& shader = entity->GetShader();

		// skip rendering invisible probes
		if (!entity->IsVisible()) continue;

		// get shape type as int
		int shapeType = entity->GetShapeType();
		 
		// 0 is for box, 1 is for sphere
		shader->SelectActiveVariation(this->lightProbeFeatureBits[shapeType + (entity->GetParallaxCorrected() ? 2 : 0)]);

		// apply mesh at shape type
		entity->ApplyProbe(probe);

		// apply shader and draw
		shader->Apply();		
        shader->Commit();
		renderDevice->Draw();
	}
}

} // namespace Lighting