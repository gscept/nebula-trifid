//------------------------------------------------------------------------------
//  godrayalgorithm.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godrayalgorithm.h"
#include "graphics/graphicsserver.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/renderdevice.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shadersemantics.h"
#include "graphics/modelentity.h"
#include "math/float4.h"
#include "lighting/globallightentity.h"
#include "graphics/cameraentity.h"
#include "graphics/graphicsprotocol.h"

using namespace Math;
using namespace Lighting;
using namespace Graphics;
using namespace CoreGraphics;
using namespace Resources;
namespace Algorithm
{
__ImplementClass(Algorithm::LightScatterAlgorithm, 'GOAL', Algorithm::AlgorithmBase);

//------------------------------------------------------------------------------
/**
*/
LightScatterAlgorithm::LightScatterAlgorithm() :
	colorDirty(false),
	sunLightRelative(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
LightScatterAlgorithm::~LightScatterAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Setup()
{
	n_assert(this->inputs.Size() == 1);
	n_assert(this->outputNames.Size() == 1);
	n_assert(this->inputs[0].isvalid());

	AlgorithmBase::Setup();

	ShaderServer* shdServer = ShaderServer::Instance();
	Util::String whiteTexPath("tex:system/white");
	whiteTexPath.Append(NEBULA3_TEXTURE_EXTENSION);

	// setup the shared light project map resource
	this->whiteMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(whiteTexPath), NULL, true).downcast<ManagedTexture>();

	this->sunShader								= shdServer->CreateShaderInstance("shd:sun");
	this->lightScatterShader					= shdServer->CreateShaderInstance("shd:lightscatter");
	this->verticalBloom							= shdServer->CreateShaderInstance("shd:verticalbloom");
	this->horizontalBloom						= shdServer->CreateShaderInstance("shd:horizontalbloom");

	// god ray variables
	this->unshadedTexVar						= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_COLORSOURCE);
	this->lightScatterLightPosVar				= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_LIGHTPOS);
	this->lightScatterDensityVar				= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_DENSITY);
	this->lightScatterDecayVar					= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_DECAY);
	this->lightScatterWeightVar					= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_WEIGHT);
	this->lightScatterExposureVar				= this->lightScatterShader->GetVariableBySemantic(NEBULA3_SEMANTIC_EXPOSURE);

	// sun variables
	this->sunTextureVarInst						= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNTEXTURE)->CreateInstance();
	this->sunColorVarInst						= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNCOLOR)->CreateInstance();

	// bloom variables
	this->godrayTexVar							= this->verticalBloom->GetVariableByName("SourceTexture");
	this->bloomedTexVar							= this->horizontalBloom->GetVariableByName("SourceTexture");

	// apply white texture to sun texture as default
	this->sunTextureVarInst->SetTexture(this->whiteMap->GetTexture());

	// setup  render target
	this->GodrayBuffer = RenderTarget::Create();
	this->GodrayBuffer->SetResolveTextureResourceId("GodrayBuffer");
	this->GodrayBuffer->SetRelativeWidth(1);
	this->GodrayBuffer->SetRelativeHeight(1);
	this->GodrayBuffer->SetWidth(this->inputs[0]->GetWidth());
	this->GodrayBuffer->SetHeight(this->inputs[0]->GetHeight());
	this->GodrayBuffer->SetAntiAliasQuality(AntiAliasQuality::None);
	this->GodrayBuffer->SetColorBufferFormat(PixelFormat::A16B16G16R16F);
	this->GodrayBuffer->Setup();

	// setup output
	this->output = RenderTarget::Create();
	this->output->SetResolveTextureResourceId(this->outputNames[0]);
	this->output->SetRelativeWidth(1);
	this->output->SetRelativeHeight(1);
	this->output->SetWidth(this->inputs[0]->GetWidth());
	this->output->SetHeight(this->inputs[0]->GetHeight());
	this->output->SetClearFlags(RenderTarget::ClearColor);
	this->output->SetClearColor(float4(0,0,0,0));
	this->output->SetAntiAliasQuality(AntiAliasQuality::None);
	this->output->SetColorBufferFormat(PixelFormat::A16B16G16R16F);
	this->output->Setup();

	// set textures
	this->unshadedTexVar->SetTexture(this->inputs[0]);

	// setup fsq
	this->quad.Setup(this->inputs[0]->GetWidth(), this->inputs[0]->GetHeight());

	// create sun
	this->sunEntity = ModelEntity::Create();
	this->sunEntity->SetResourceId("mdl:system/sun.n3");
	this->sunEntity->SetAlwaysVisible(true);

	// this algorithm needs a deferred setup since we need the stage to be properly setup before we can attach the sun
	this->deferredSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Discard()
{
	AlgorithmBase::Discard();

	this->lightScatterShader->Discard();
	this->lightScatterShader = 0;

	this->sunShader->Discard();
	this->sunShader = 0;

	this->verticalBloom->Discard();
	this->verticalBloom = 0;

	this->horizontalBloom->Discard();
	this->horizontalBloom = 0;

	this->GodrayBuffer->Discard();
	this->GodrayBuffer = 0;

	this->output->Discard();
	this->output = 0;

	ResourceManager::Instance()->DiscardManagedResource(this->whiteMap.upcast<ManagedResource>());
	this->whiteMap = 0;

	this->unshadedTexVar = 0;
	this->godrayTexVar = 0;
	this->bloomedTexVar;

	this->quad.Discard();

	this->lightScatterLightPosVar = 0;
	this->lightScatterDensityVar = 0;
	this->lightScatterDecayVar = 0;
	this->lightScatterWeightVar = 0;
	this->lightScatterExposureVar = 0;

	this->sunEntity->MarkRemove();
	this->sunEntity = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Prepare()
{
	if (this->deferredSetup)
	{
		// attach sun to scene
		GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->sunEntity.upcast<GraphicsEntity>());

		// we have performed our deferred setup, so set flag to false to avoid this happening again
		this->deferredSetup = false;
	}

	if (this->enabled)
	{
		const Ptr<CameraEntity>& cam = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
		if (cam.isvalid())
		{
			// set sun position
			float4 pos;
			if (this->sunLightRelative)
			{
				// get global light
				const Ptr<GlobalLightEntity>& globalLight = GraphicsServer::Instance()->GetCurrentGlobalLightEntity();
				pos = globalLight->GetLightDirection() * 10 + cam->GetTransform().get_position();
				pos.w() = 1.0f;

				// set color
				this->sunColorVarInst->SetFloat4(globalLight->GetColor());
			}
			else
			{
				pos = this->sunPosition + cam->GetTransform().get_position();
			}

			float4 camPos = cam->GetTransform().get_position();
			float4 toCamera = float4::normalize(pos - camPos);
			float4 upVec = vector(0,1,0);
			float4 xAxis = float4::normalize(float4::cross3(toCamera, upVec));
			upVec =  float4::normalize(float4::cross3(xAxis, toCamera));
			matrix44 sunTransform = matrix44(xAxis, upVec, toCamera, pos);
			this->sunEntity->SetTransform(sunTransform);		

			matrix44 viewProj = cam->GetViewProjTransform();
			float4 screenPos = matrix44::transform(pos, viewProj);
			screenPos.x() /= screenPos.w();
			screenPos.y() /= screenPos.w();
			screenPos.x() = (screenPos.x() + 1.0f) * 0.5f;
			screenPos.y() = 1.0f - ((screenPos.y() + 1.0f) * 0.5f);
			this->lightScatterLightPosVar->SetFloat2(float2(screenPos.x(), screenPos.y()));
		}	

		// setup color
		if (this->colorDirty)
		{
			// set sun color
			this->sunColorVarInst->SetFloat4(this->sunColor);
			this->colorDirty = false;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Execute()
{
	if (this->enabled)
	{
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shdServer = ShaderServer::Instance();

		this->sunColorVarInst->Apply();
		this->sunTextureVarInst->Apply();

		// do initial render and draw to first buffer
		shdServer->SetActiveShaderInstance(this->lightScatterShader);
		renderDevice->BeginPass(this->output, this->lightScatterShader);
		this->quad.Draw();
		renderDevice->EndPass();

		// perform vertical bloom, this is also our output
		this->godrayTexVar->SetTexture(this->output->GetResolveTexture());
		shdServer->SetActiveShaderInstance(this->verticalBloom);
		renderDevice->BeginPass(this->GodrayBuffer, this->verticalBloom);
		this->quad.Draw();
		renderDevice->EndPass();

		// now perform the blooming process
		this->bloomedTexVar->SetTexture(this->GodrayBuffer->GetResolveTexture());
		shdServer->SetActiveShaderInstance(this->horizontalBloom);
		renderDevice->BeginPass(this->output, this->horizontalBloom);
		this->quad.Draw();
		renderDevice->EndPass();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::OnDisplayResize()
{
	// resize render targets
	this->GodrayBuffer->OnDisplayResized();
	this->output->OnDisplayResized();

	// reattach textures
	this->unshadedTexVar->SetTexture(this->inputs[0]);
	this->godrayTexVar->SetTexture(this->output->GetResolveTexture());
	this->bloomedTexVar->SetTexture(this->GodrayBuffer->GetResolveTexture());
}

//------------------------------------------------------------------------------
/**
*/
bool 
LightScatterAlgorithm::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(EnableGodrays::Id))
	{
		const Ptr<EnableGodrays>& rMsg = msg.downcast<EnableGodrays>();
		this->SetEnabled(rMsg->GetEnabled());
		return true;
	}
	else if (msg->CheckId(SetSunTexture::Id))
	{
		const Ptr<SetSunTexture>& rMsg = msg.downcast<SetSunTexture>();
		// set sun texture
		if (this->currentTexture.isvalid())
		{
			ResourceManager::Instance()->DiscardManagedResource(this->currentTexture.upcast<ManagedResource>());
			this->currentTexture = 0;
		}
		this->currentTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(rMsg->GetTexture())).downcast<ManagedTexture>();
		this->sunTextureVarInst->SetTexture(this->currentTexture->GetTexture());
		return true;
	}
	else if (msg->CheckId(SetSunColor::Id))
	{
		const Ptr<SetSunColor>& rMsg = msg.downcast<SetSunColor>();
		this->sunColor = rMsg->GetColor();
		this->colorDirty = true;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::SetEnabled( bool b )
{
	AlgorithmBase::SetEnabled(b);
	if (b)
	{
		this->Enable();
	}
	else
	{
		this->Disable();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Enable()
{
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->sunEntity.upcast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void 
LightScatterAlgorithm::Disable()
{
	RenderDevice* renderDevice = RenderDevice::Instance();

	// start and end pass in renderdevice to disable effect
	this->output->Clear();
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->RemoveEntity(this->sunEntity.upcast<GraphicsEntity>());
}

} // namespace Algorithm