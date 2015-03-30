//------------------------------------------------------------------------------
//  godrayserver.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godrayserver.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "resources/resourcemanager.h"
#include "lighting/internalgloballightentity.h"
#include "internalgraphics/internalview.h"
#include "coregraphics/shadersemantics.h"
#include "frame/frameserver.h"
#include "coregraphics/shaderserver.h"
#include "models/modelnode.h"

namespace Godrays
{
__ImplementSingleton(Godrays::GodrayServer);
__ImplementClass(Godrays::GodrayServer, 'GOSR', Core::RefCounted);

using namespace Frame;
using namespace InternalGraphics;
using namespace Math;
using namespace Resources;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
GodrayServer::GodrayServer() :
	isOpen(false),
	sunLightRelative(true),
	colorDirty(true),
	textureDirty(false),
	camera(0),
	sunEntity(0),
	sunColor(float4(1, 1, 1, 1)),
	sunPosition(float4(10, 10, 10, 1)),
	firstFrame(true)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GodrayServer::~GodrayServer()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
GodrayServer::Open()
{
	n_assert(!this->IsOpen());

	ShaderServer* shdServer = ShaderServer::Instance();

	Util::String whiteTexPath("tex:system/white");
	whiteTexPath.Append(NEBULA3_TEXTURE_EXTENSION);

	// setup the shared light project map resource
	this->whiteMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(whiteTexPath), NULL, true).downcast<ManagedTexture>();

	// get frame shader and create shader instances
	this->godRayFrameShader						= FrameServer::Instance()->LookupFrameShader("godrays");
	this->sunShader								= shdServer->CreateShaderInstance("shd:sun");
	this->godRayShader							= shdServer->CreateShaderInstance("shd:lightscatter");

	// god ray variables
	this->godRayTex								= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_COLORSOURCE);
	this->godRayLightPos						= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_LIGHTPOS);
	this->godRayDensity							= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_DENSITY);
	this->godRayDecay							= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_DECAY);
	this->godRayWeight							= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_WEIGHT);
	this->godRayExposure						= this->godRayShader->GetVariableBySemantic(NEBULA3_SEMANTIC_EXPOSURE);

	// sun variables
	this->sunTextureVar							= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNTEXTURE)->CreateInstance();
	this->sunColorVar							= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNCOLOR)->CreateInstance();

	// apply white texture to sun texture as default
	this->sunTextureVar->SetTexture(this->whiteMap->GetTexture());

	// create sun
	this->sunEntity = InternalModelEntity::Create();
	this->sunEntity->SetResourceId("mdl:system/sun.n3");
	this->sunEntity->SetAlwaysVisible(true);
	InternalGraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->sunEntity.upcast<InternalGraphicsEntity>());

	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
GodrayServer::Close()
{
	n_assert(this->IsOpen());

	this->godRayFrameShader->Discard();
	this->godRayFrameShader = 0;

	this->sunEntity->MarkRemove();
	this->sunEntity = 0;
	this->camera = 0;

	this->unshadedBuffer = 0;

	this->sunTextureVar->Discard();
	this->sunTextureVar = 0;
	this->sunColorVar->Discard();
	this->sunColorVar = 0;

	this->sunShader->Discard();
	this->sunShader = 0;
	this->godRayShader->Discard();
	this->godRayShader = 0;

	ResourceManager::Instance()->DiscardManagedResource(this->whiteMap.upcast<ManagedResource>());
	this->whiteMap = 0;

	this->godRayTex = 0;
	this->godRayLightPos = 0;
	this->godRayDensity = 0;
	this->godRayDecay = 0;
	this->godRayWeight = 0;
	this->godRayExposure = 0;

	this->isOpen = false;
	this->firstFrame = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayServer::OnRenderBefore()
{
	if (this->camera.isvalid())
	{
		float4 pos;
		if (this->sunLightRelative)
		{
			// get global light
			const Ptr<InternalGlobalLightEntity>& globalLight = InternalGraphicsServer::Instance()->GetCurrentGlobalLightEntity();
			pos = globalLight->GetLightDirection() * 10 + this->camera->GetTransform().get_position();
			pos.w() = 1.0f;

			// set color
			this->sunColorVar->SetFloat4(globalLight->GetColor());
		}
		else
		{
			pos = this->sunPosition + this->camera->GetTransform().get_position();
		}

		float4 camPos = this->camera->GetTransform().get_position();
		float4 toCamera = float4::normalize(pos - camPos);
		float4 upVec = vector(0,1,0);
		float4 xAxis = float4::normalize(float4::cross3(toCamera, upVec));
		upVec =  float4::normalize(float4::cross3(xAxis, toCamera));
		matrix44 sunTransform = matrix44(xAxis, upVec, toCamera, pos);
		this->sunEntity->SetTransform(sunTransform);

		matrix44 viewProj = this->camera->GetViewProjTransform();
		float4 screenPos = matrix44::transform(this->sunEntity->GetTransform().get_position(), viewProj);
		screenPos.x() /= screenPos.w();
		screenPos.y() /= screenPos.w();
		screenPos.x() = (screenPos.x() + 1.0f) * 0.5f;
		screenPos.y() = 1.0f - ((screenPos.y() + 1.0f) * 0.5f);
		this->godRayLightPos->SetFloat2(float2(screenPos.x(), screenPos.y()));
	}

	if (this->colorDirty)
	{
		// set sun color
		this->sunColorVar->SetFloat4(this->sunColor);
		this->colorDirty = false;
	}	

	if (this->textureDirty)
	{
		// set sun texture
		if (this->currentTexture.isvalid())
		{
			ResourceManager::Instance()->DiscardManagedResource(this->currentTexture.upcast<ManagedResource>());
			this->currentTexture = 0;
		}
		this->currentTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(this->sunTexture)).downcast<ManagedTexture>();
		this->sunTextureVar->SetTexture(this->currentTexture->GetTexture());
		this->textureDirty = false;
	}

	this->sunColorVar->Apply();
	this->sunTextureVar->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayServer::OnRenderAfter()
{
	// only perform this on the first frame
	if (this->firstFrame)
	{
		// set black geometry buffer to god ray texture			
		this->unshadedBuffer = ResourceManager::Instance()->LookupResource("UnshadedBuffer").downcast<Texture>();

		// get default frame shader
		Ptr<FrameShader> defaultFrameshader = FrameServer::Instance()->LookupFrameShader(NEBULA3_DEFAULT_FRAMESHADER_NAME);

		// get last pass and update variable
		Ptr<FramePassBase> finalize = defaultFrameshader->GetFramePassBaseByName("Finalize");
		finalize->GetVariableByName("GodrayTexture")->SetTexture(ResourceManager::Instance()->LookupResource("GodrayBuffer0").downcast<Texture>());

		this->camera = InternalGraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
		this->firstFrame = false;
	}

	// apply unshaded texture, may or may not be loaded, so we need to 'touch' it to be sure it's active
	this->godRayTex->SetTexture(this->unshadedBuffer);
}


} // namespace Godrays
