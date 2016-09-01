//------------------------------------------------------------------------------
//  volumetriclightalgorithm.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "volumetriclightalgorithm.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/transformdevice.h"
#include "graphics/graphicsserver.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "graphics/globallightentity.h"

using namespace Resources;
using namespace CoreGraphics;
using namespace Math;
using namespace Graphics;
using namespace Lighting;
namespace Algorithm
{
__ImplementClass(Algorithm::VolumetricLightAlgorithm, 'VLAL', Algorithm::AlgorithmBase);

//------------------------------------------------------------------------------
/**
*/
VolumetricLightAlgorithm::VolumetricLightAlgorithm() :
	globalLight(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VolumetricLightAlgorithm::~VolumetricLightAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
VolumetricLightAlgorithm::Setup()
{
	n_assert(this->targets.Size() == 1);
	n_assert(this->targets[0].isvalid());
	n_assert(this->outputNames.Size() == 1);
	
	ResourceManager* resManager = ResourceManager::Instance();
	ShaderServer* shdServer = ShaderServer::Instance();

	AlgorithmBase::Setup();

	_setup_timer(VolumetricLightTimer);

	// load the light source shapes
	this->globalLightMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/sunshape.nvx2")).downcast<ManagedMesh>();
	this->pointLightMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/sphere.nvx2")).downcast<ManagedMesh>();
	this->spotLightMesh = resManager->CreateManagedResource(Mesh::RTTI, ResourceId("msh:system/cone.nvx2")).downcast<ManagedMesh>();

	// setup the shared light project map resource
	Util::String whiteTexPath("tex:system/white");
	whiteTexPath.Append(NEBULA3_TEXTURE_EXTENSION);
	this->whiteMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(whiteTexPath), NULL, true).downcast<ManagedTexture>();

	this->pointLightFeatureBits				= shdServer->FeatureStringToMask("Point");
	this->spotLightFeatureBits				= shdServer->FeatureStringToMask("Spot");
	this->globalLightFeatureBits			= shdServer->FeatureStringToMask("Global");
	this->globalLightMeshFeatureBits		= shdServer->FeatureStringToMask("Alt0");
	this->globalScatterFeatureBits			= globalLightFeatureBits;

	// setup shader
	this->volumeLightShader					= shdServer->GetShader("shd:volumelight")->CreateState({ NEBULAT_DEFAULT_GROUP });
	this->lightScatterShader				= shdServer->GetShader("shd:lightscatter")->CreateState({ NEBULAT_DEFAULT_GROUP });
	this->verticalBloom						= shdServer->GetShader("shd:verticalbloom")->CreateState({ NEBULAT_DEFAULT_GROUP });
	this->horizontalBloom					= shdServer->GetShader("shd:horizontalbloom")->CreateState({ NEBULAT_DEFAULT_GROUP });

	// scatter variables
	this->lightScatterUnshadedTextureVar	= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_COLORSOURCE);
	this->lightScatterLightPosVar			= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPOS);
	this->lightScatterDensityVar			= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_DENSITY);
	this->lightScatterDecayVar				= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_DECAY);
	this->lightScatterWeightVar				= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_WEIGHT);
	this->lightScatterExposureVar			= this->lightScatterShader->GetVariableByName(NEBULA3_SEMANTIC_EXPOSURE);

	// bloom variables
	this->godrayTexVar						= this->verticalBloom->GetVariableByName("SourceTexture");
	this->bloomedTexVar						= this->horizontalBloom->GetVariableByName("SourceTexture");

	// volume light variables
	//this->volumeUnshadedTextureVar			= this->volumeLightShader->GetVariableByName("UnshadedTexture");
	this->lightScaleVar						= this->volumeLightShader->GetVariableByName("VolumetricScale");
	this->lightIntensityVar					= this->volumeLightShader->GetVariableByName("VolumetricIntensity");
    this->lightTransformVar                 = this->volumeLightShader->GetVariableByName("Transform");
	this->lightProjMapVar					= this->volumeLightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPROJMAP);
	this->lightProjCubeVar					= this->volumeLightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTPROJCUBE);
	this->lightColorVar						= this->volumeLightShader->GetVariableByName(NEBULA3_SEMANTIC_LIGHTCOLOR);
	this->lightCenterPosVar					= this->volumeLightShader->GetVariableByName("LightCenter");

	// apply white texture to sun texture as default
	this->lightProjMapVar->SetTexture(this->whiteMap->GetTexture());

	// get unshaded render target
	this->albedoRenderTarget = this->targets[0];

	// setup  render target
	this->volumeLightBuffer1 = RenderTarget::Create();
	this->volumeLightBuffer1->SetResolveTextureResourceId("VolumeLight1");
	this->volumeLightBuffer1->SetClearColor(float4(0,0,0,0));
    this->volumeLightBuffer1->SetClearFlags(RenderTarget::ClearColor);
    this->volumeLightBuffer1->SetRelativeWidth(1.0f);
    this->volumeLightBuffer1->SetRelativeHeight(1.0f);
	this->volumeLightBuffer1->SetAntiAliasQuality(AntiAliasQuality::None);
	this->volumeLightBuffer1->SetColorBufferFormat(PixelFormat::A16B16G16R16F);

    // set depth-stencil in  buffer
	this->volumeLightBuffer1->SetDepthStencilTarget(this->albedoRenderTarget->GetDepthStencilTarget());
	this->volumeLightBuffer1->Setup();

	// setup render target
	this->volumeLightBuffer2 = RenderTarget::Create();
	this->volumeLightBuffer2->SetResolveTextureResourceId("VolumeLight2");
    this->volumeLightBuffer2->SetRelativeWidth(1.0f);
    this->volumeLightBuffer2->SetRelativeHeight(1.0f);
	this->volumeLightBuffer2->SetAntiAliasQuality(AntiAliasQuality::None);
	this->volumeLightBuffer2->SetColorBufferFormat(PixelFormat::A16B16G16R16F);
	this->volumeLightBuffer2->Setup();

	// setup output
	this->output = RenderTarget::Create();
	this->output->SetResolveTextureResourceId(this->outputNames[0]);
    this->output->SetRelativeWidth(1.0f);
    this->output->SetRelativeHeight(1.0f);
	this->output->SetAntiAliasQuality(AntiAliasQuality::None);
	this->output->SetColorBufferFormat(PixelFormat::A16B16G16R16F);
	this->output->Setup();

	// setup fsq
	this->quad.Setup(this->output->GetWidth(), this->output->GetHeight());
}

//------------------------------------------------------------------------------
/**
*/
void 
VolumetricLightAlgorithm::Discard()
{
	AlgorithmBase::Discard();

	this->volumeLightShader = 0;
	this->lightScatterShader = 0;
	this->verticalBloom = 0;
	this->horizontalBloom = 0;

	this->volumeLightBuffer1->Discard();
	this->volumeLightBuffer1 = 0;

    this->volumeLightBuffer2->Discard();
    this->volumeLightBuffer2 = 0;

	//this->volumeUnshadedTextureVar = 0;
	this->lightScaleVar = 0;
	this->lightIntensityVar = 0;
	this->lightProjCubeVar = 0;
	this->lightProjMapVar = 0;
	this->lightColorVar = 0;

	this->output->Discard();
	this->output = 0;

	this->lightScatterUnshadedTextureVar = 0;
	this->godrayTexVar = 0;
	this->bloomedTexVar;

	this->quad.Discard();

	this->lightScatterLightPosVar = 0;
	this->lightScatterDensityVar = 0;
	this->lightScatterDecayVar = 0;
	this->lightScatterWeightVar = 0;
	this->lightScatterExposureVar = 0;

	this->pointLights.Clear();
	this->spotLights.Clear();
	this->globalLight = 0;

	ResourceManager::Instance()->DiscardManagedResource(this->globalLightMesh.upcast<ManagedResource>());
	this->globalLightMesh = 0;
	ResourceManager::Instance()->DiscardManagedResource(this->spotLightMesh.upcast<ManagedResource>());
	this->spotLightMesh = 0;
	ResourceManager::Instance()->DiscardManagedResource(this->pointLightMesh.upcast<ManagedResource>());
	this->pointLightMesh = 0;
	
	ResourceManager::Instance()->DiscardManagedResource(this->whiteMap.upcast<ManagedResource>());
	this->whiteMap = 0;

	_discard_timer(VolumetricLightTimer);
}

//------------------------------------------------------------------------------
/**
*/
void 
VolumetricLightAlgorithm::Render()
{
	RenderDevice* renderDevice = RenderDevice::Instance();
	ShaderServer* shaderServer = ShaderServer::Instance();
	if (this->enabled)
	{
		_start_timer(VolumetricLightTimer);

		//shaderServer->SetActiveShaderInstance(this->volumeLightShader);

		// apply view settings to current shader
		//TransformDevice::Instance()->ApplyViewSettings();

		// set light scatter shader to use the global implementation

		// setup variables
		//this->lightScatterDensityVar->SetFloat(0.98f);

		// render lights
		this->RenderGlobalLight();

		// perform vertical bloom, this is also our output
        this->verticalBloom->Apply();
        renderDevice->BeginPass(this->volumeLightBuffer1);
		this->godrayTexVar->SetTexture(this->volumeLightBuffer2->GetResolveTexture());
        this->verticalBloom->Commit();
		this->quad.Draw();
		renderDevice->EndPass();

		// now perform the blooming process
        this->horizontalBloom->Apply();
        renderDevice->BeginPass(this->output);
		this->bloomedTexVar->SetTexture(this->volumeLightBuffer1->GetResolveTexture());
        this->horizontalBloom->Commit();
		this->quad.Draw();
		renderDevice->EndPass();

		_stop_timer(VolumetricLightTimer);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
VolumetricLightAlgorithm::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(EnableVolumetricLighting::Id))
	{
		const Ptr<EnableVolumetricLighting>& rMsg = msg.downcast<EnableVolumetricLighting>();
		this->SetEnabled(rMsg->GetEnabled());
	}
	else if (msg->CheckId(AddVolumetricLight::Id))
	{
		const Ptr<AddVolumetricLight>& rMsg = msg.downcast<AddVolumetricLight>();
		const Ptr<AbstractLightEntity>& light = rMsg->GetLight();

		if (light->IsA(GlobalLightEntity::RTTI))
		{
			this->globalLight = light.downcast<GlobalLightEntity>();
		}
		else if (light->IsA(SpotLightEntity::RTTI))
		{
			IndexT index = this->spotLights.FindIndex(light.downcast<SpotLightEntity>());
			if(index == InvalidIndex) this->spotLights.Append(light.downcast<SpotLightEntity>());
		}
		else if (light->IsA(PointLightEntity::RTTI))
		{
			IndexT index = this->pointLights.FindIndex(light.downcast<PointLightEntity>());
			if (index == InvalidIndex) this->pointLights.Append(light.downcast<PointLightEntity>());
		}
	}
	else if (msg->CheckId(RemoveVolumetricLight::Id))
	{
		const Ptr<RemoveVolumetricLight>& rMsg = msg.downcast<RemoveVolumetricLight>();
		const Ptr<AbstractLightEntity>& light = rMsg->GetLight();

		if (light->IsA(GlobalLightEntity::RTTI))
		{
			this->globalLight = 0;
		}
		else if (light->IsA(SpotLightEntity::RTTI))
		{
			IndexT index = this->spotLights.FindIndex(light.downcast<SpotLightEntity>());
			if (index != InvalidIndex)	this->spotLights.EraseIndex(index);
		}
		else if (light->IsA(PointLightEntity::RTTI))
		{
			IndexT index = this->pointLights.FindIndex(light.downcast<PointLightEntity>());
			if (index != InvalidIndex)	this->pointLights.EraseIndex(index);
		}
	}

	// we don't want to catch any messages here seeing as they should also be handled in the LightScatterAlgorithm.
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
VolumetricLightAlgorithm::RenderGlobalLight()
{
	n_assert(this->globalLightMesh.isvalid());
	TransformDevice* transformDevice = TransformDevice::Instance();
	RenderDevice* renderDevice = RenderDevice::Instance();
	ShaderServer* shdServer = ShaderServer::Instance();

	// render global light if set
	if (this->globalLight.isvalid())
	{
		// begin shader update
		this->volumeLightShader->BeginUpdateSync();

		// setup camera
		const Ptr<CameraEntity>& cam = GraphicsServer::Instance()->GetCurrentView()->GetCameraEntity();
		matrix44 viewProj = cam->GetViewProjTransform();
		matrix44 camTrans = cam->GetTransform();
		const Ptr<GlobalLightEntity>& curLight = this->globalLight;
		const Ptr<ManagedTexture>& tex = curLight->GetProjectionTexture();
		this->lightColorVar->SetFloat4(curLight->GetColor());
		if (tex.isvalid())	this->lightProjMapVar->SetTexture(curLight->GetProjectionTexture()->GetTexture());
		else				this->lightProjMapVar->SetTexture(this->whiteMap->GetTexture());

		// calculate position
		float4 pos = curLight->GetLightDirection() * 10 + camTrans.get_position();
		pos.w() = 1.0f;

		// calculate screen position
		float2 screenPos = cam->CalculateScreenSpacePosition(pos);

		// creates transform matrix where mesh is constantly pointing towards camera
		float4 camPos = camTrans.get_position();
		float4 toCamera = float4::normalize(pos - camPos);
		float4 upVec = vector(0,1,0);
		float4 xAxis = float4::normalize(float4::cross3(toCamera, upVec));
		upVec =  float4::normalize(float4::cross3(xAxis, toCamera));
		matrix44 sunTransform = matrix44(xAxis, upVec, toCamera, pos);

		// set light vars
		this->lightScaleVar->SetFloat(curLight->GetVolumetricScale());
		this->lightIntensityVar->SetFloat(curLight->GetVolumetricIntensity());
        //this->volumeUnshadedTextureVar->SetTexture(this->albedoRenderTarget->GetResolveTexture());
        this->lightTransformVar->SetMatrix(sunTransform);
		this->lightCenterPosVar->SetFloat2(screenPos);
        this->volumeLightShader->EndUpdateSync();

		// begin pass
		renderDevice->BeginPass(this->volumeLightBuffer1);

		// select variation for mesh
		this->volumeLightShader->SelectActiveVariation(this->globalLightFeatureBits);
		this->volumeLightShader->Apply();

		// apply global light mesh
		this->globalLightMesh->GetMesh()->ApplyPrimitives(0);

		// commit shader, only need to do this once
		this->volumeLightShader->Commit();

		// draw
		renderDevice->Draw();

		// select variation for global light base plate and draw
		this->volumeLightShader->SelectActiveVariation(this->globalLightMeshFeatureBits);
		this->volumeLightShader->Apply();

		// apply global light mesh
		this->globalLightMesh->GetMesh()->ApplyPrimitives(0);

		// draw
		renderDevice->Draw();

        // end pass
		renderDevice->EndPass();
	
		// perform scattering
		this->Scatter(screenPos);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
VolumetricLightAlgorithm::Scatter(const Math::float2& lightPos)
{
	ShaderServer* shdServer = ShaderServer::Instance();
	TransformDevice* transformDevice = TransformDevice::Instance();
	RenderDevice* renderDevice = RenderDevice::Instance();

	// begin scatter pass
    this->lightScatterShader->SelectActiveVariation(this->globalScatterFeatureBits);
    this->lightScatterShader->Apply();
	renderDevice->BeginPass(this->volumeLightBuffer2);

	// set position of light
    this->lightScatterShader->BeginUpdateSync();
	this->lightScatterLightPosVar->SetFloat2(lightPos);
    this->lightScatterUnshadedTextureVar->SetTexture(this->volumeLightBuffer1->GetResolveTexture());
    this->lightScatterShader->EndUpdateSync();

	// draw effect
	this->lightScatterShader->Commit();
	this->quad.Draw();

	// end pass
	renderDevice->EndPass();
}

//------------------------------------------------------------------------------
/**
*/
void 
VolumetricLightAlgorithm::OnDisplayResized(SizeT width, SizeT height)
{
	n_assert(this->volumeLightBuffer1.isvalid());
	n_assert(this->volumeLightBuffer2.isvalid());

	// resize rendertargets
	this->volumeLightBuffer1->OnDisplayResized(width, height);
	this->volumeLightBuffer2->OnDisplayResized(width, height);
	this->output->OnDisplayResized(width, height);

	// set light scatter texture again
	this->lightScatterUnshadedTextureVar->SetTexture(this->volumeLightBuffer1->GetResolveTexture());

    // reset quad
    this->quad.Discard();  
    this->quad.Setup(this->output->GetWidth(), this->output->GetHeight());
}
} // namespace Algorithm