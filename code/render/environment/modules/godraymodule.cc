//------------------------------------------------------------------------------
//  godraymodule.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "godraymodule.h"
#include "frame\frameserver.h"
#include "lighting\globallightentity.h"
#include "graphics\graphicsserver.h"
#include "resources\resourcemanager.h"
#include "coregraphics\shadersemantics.h"
#include "coregraphics\shaderserver.h"
#include "graphics\view.h"
#include "lighting\lightserver.h"

using namespace Frame;
using namespace Graphics;
using namespace Math;
using namespace Resources;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Lighting;
namespace Environment
{
__ImplementClass(Environment::GodrayModule, 'GDMO', Base::EnvironmentModuleBase);

//------------------------------------------------------------------------------
/**
*/
GodrayModule::GodrayModule()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GodrayModule::~GodrayModule()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::Setup()
{
	ShaderServer* shdServer = ShaderServer::Instance();

	Util::String whiteTexPath("tex:system/white");
	whiteTexPath.Append(NEBULA3_TEXTURE_EXTENSION);

	// setup the shared light project map resource
	this->whiteMap = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, ResourceId(whiteTexPath)).downcast<ManagedTexture>();

	// get frame shader and create shader instances
	this->godRayFrameShader						= Frame::FrameServer::Instance()->LookupFrameShader("godrays");
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
	this->sunTexture							= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNTEXTURE);
	this->sunColor								= this->sunShader->GetVariableBySemantic(NEBULA3_SEMANTIC_SUNCOLOR);

	// apply white texture to sun texture as default
	this->sunTexture->SetTexture(this->whiteMap->GetTexture());

	// create sun
	this->sunEntity = ModelEntity::Create();
	this->sunEntity->SetResourceId("mdl:system/sun.n3");
	this->sunEntity->SetAlwaysVisible(true);
	this->sunEntity->AddRef();
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::Discard()
{
	this->camera = 0;

	this->godRayFrameShader->Discard();
	this->godRayFrameShader = 0;

	this->sunEntity->MarkRemove();
	this->sunEntity = 0;

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

	this->sunTexture = 0;
	this->sunColor = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::BeginFrame( const Ptr<Graphics::CameraEntity>& cam )
{
	// get global light
	const Ptr<GlobalLightEntity>& globalLight = GraphicsServer::Instance()->GetCurrentGlobalLightEntity();
	this->camera = cam;
	
	// update sun position, currently, we are limited to one of these babies
	if (globalLight.isvalid() && this->sunEntity->IsAttachedToStage())
	{
		// set sun color
		this->sunColor->SetFloat4(globalLight->GetColor());

		float4 dir = globalLight->GetLightDirection();
		dir = dir * 10 + this->camera->GetTransform().get_position();
		float4 camPos = this->camera->GetTransform().get_position();
		float4 toCamera = float4::normalize(dir - camPos);
		float4 upVec = vector(0,1,0);
		float4 xAxis = float4::normalize(float4::cross3(toCamera, upVec));
		upVec =  float4::normalize(float4::cross3(xAxis, toCamera));
		matrix44 sunTransform = matrix44(xAxis, upVec, toCamera, dir);
		this->sunEntity->SetTransform(sunTransform);

		matrix44 viewProj = cam->GetViewProjTransform();
		float4 screenPos = matrix44::transform(this->sunEntity->GetTransform().get_position(), viewProj);
		screenPos.x() /= screenPos.w();
		screenPos.y() /= screenPos.w();
		screenPos.x() = (screenPos.x() + 1.0f) * 0.5f;
		screenPos.y() = 1.0f - ((screenPos.y() + 1.0f) * 0.5f);
		this->godRayLightPos->SetFloat2(float2(screenPos.x(), screenPos.y()));

		// set black geometry buffer to god ray texture		
		Ptr<Texture> texture = ResourceManager::Instance()->LookupResource("UnshadedBuffer").downcast<Texture>();
		this->godRayTex->SetTexture(texture);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::Render()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::EndFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::SetAttached(bool attach)
{
	if(attach)
	{	
		if(!this->sunEntity->IsAttachedToStage())
		{
			GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->sunEntity.upcast<GraphicsEntity>());
		}	
	}	
	else
	{		
		if (this->sunEntity->IsAttachedToStage())
		{
			this->sunEntity->MarkRemove();	
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::SetEnabled( bool b )
{
	n_assert(this->godRayFrameShader.isvalid());
	EnvironmentModuleBase::SetEnabled(b);

	this->godRayFrameShader->SetEnabled(this->isEnabled);
	this->sunEntity->SetVisible(this->isEnabled);
	this->sunEntity->SetAlwaysVisible(this->isEnabled);
	this->godRayFrameShader->GetRenderTargetByName("GodrayBuffer0")->Clear(CoreGraphics::RenderTarget::ClearColor, float4(0,0,0,0), 0, 0);
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::SetSunTexture( const Resources::ResourceId& res )
{
	n_assert(res.IsValid());
	Ptr<ManagedTexture> texture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, "tex:" + res.AsString() + NEBULA3_TEXTURE_EXTENSION).downcast<ManagedTexture>();
	ResourceManager::Instance()->WaitForPendingResources(0);
	this->sunTexture->SetTexture(texture->GetTexture());
}

//------------------------------------------------------------------------------
/**
*/
void 
GodrayModule::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(Graphics::EnableGodrays::Id))
	{
		Ptr<Graphics::EnableGodrays> rMsg = msg.downcast<Graphics::EnableGodrays>();
		this->SetEnabled(rMsg->GetEnabled());
		msg->SetHandled(true);
	}
}

} // namespace Environment