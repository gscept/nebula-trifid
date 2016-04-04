//------------------------------------------------------------------------------
//  editorlightprobeproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "editorlightprobeproperty.h"
#include "graphicsfeatureunit.h"
#include "lightprobemanager.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "properties/editorproperty.h"
#include "coregraphics/rendershape.h"
#include "debugrender/debugshaperenderer.h"
#include "leveleditor2app.h"
#include "managers/entitymanager.h"
#include "leveleditor2/leveleditor2protocol.h"

using namespace Math;
using namespace Lighting;
using namespace GraphicsFeature;
using namespace Graphics;
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::EditorLightProbeProperty, 'ELPP', GraphicsFeature::LightProbeProperty);

//------------------------------------------------------------------------------
/**
*/
EditorLightProbeProperty::EditorLightProbeProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EditorLightProbeProperty::~EditorLightProbeProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::SetupCallbacks()
{
	LightProbeProperty::SetupCallbacks();
	this->entity->RegisterPropertyCallback(this, Render);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::OnActivate()
{
	LightProbeProperty::OnActivate();

	// get matrix
	matrix44 transform = this->entity->GetMatrix44(Attr::Transform);

	// create light probe
	this->environmentCapturer = EnvironmentProbeCapturer::Create();
	this->environmentCapturer->SetPosition(transform.get_position());
	this->environmentCapturer->SetResolution(this->entity->GetInt(Attr::ProbeResolutionWidth), this->entity->GetInt(Attr::ProbeResolutionHeight));
	this->environmentCapturer->SetGenerateMipmaps(this->entity->GetBool(Attr::ProbeGenerateMipmaps));
	this->environmentCapturer->SetRenderIrradiance(this->entity->GetBool(Attr::ProbeBuildIrradiance));
	this->environmentCapturer->SetRenderReflections(this->entity->GetBool(Attr::ProbeBuildReflections));
	this->environmentCapturer->SetGenerateDepthCube(this->entity->GetBool(Attr::ProbeBuildDepth));
	this->environmentCapturer->SetOutput(this->entity->GetString(Attr::ProbeOutputFolder), this->entity->GetString(Attr::ProbeOutputFilename));
	this->environmentCapturer->SetName(this->entity->GetString(Attr::ProbeName));
	this->environmentCapturer->SetEntity(this->entity);

	// add to manager
	LightProbeManager::Instance()->RegisterProbe(this->environmentCapturer);

	// create reflection map preview
	this->reflectionMapPreview = ModelEntity::Create();
	this->reflectionMapPreview->SetResourceId("mdl:system/lightprobesphere.n3");
	this->reflectionMapPreview->SetPickingId(this->entity->GetUniqueId());
	this->reflectionMapPreview->SetEnvironmentProbe(this->environmentProbe);
	transform.set_xaxis(Math::float4::normalize(transform.get_xaxis()));
	transform.set_yaxis(Math::float4::normalize(transform.get_yaxis()));
	transform.set_zaxis(Math::float4::normalize(transform.get_zaxis()));
	this->reflectionMapPreview->SetTransform(transform);

	bbox zone(transform.get_position(), this->entity->GetFloat4(Attr::ProbeParallaxBox));
	this->environmentCapturer->SetCaptureZone(zone);

	// attach model to stage
	Ptr<Graphics::Stage> stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	n_assert(stage);
	stage->AttachEntity(this->reflectionMapPreview.upcast<Graphics::GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::OnDeactivate()
{
	// remove from manager
	LightProbeManager::Instance()->UnregisterProbe(this->environmentCapturer);

	// remove references to model and probe
	this->environmentCapturer->Discard();
	this->environmentCapturer = 0;

	// remove model
	Ptr<Graphics::Stage> stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	n_assert(stage);
	stage->RemoveEntity(this->reflectionMapPreview.upcast<Graphics::GraphicsEntity>());
	this->reflectionMapPreview = 0;

	LightProbeProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::SetupAcceptedMessages()
{
	LightProbeProperty::SetupAcceptedMessages();
	this->RegisterMessage(LevelEditor2::BeginProbeBuild::Id);
	this->RegisterMessage(LevelEditor2::EndProbeBuild::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
	{
		matrix44 transform = (msg.cast<BaseGameFeature::UpdateTransform>())->GetMatrix();

		// update zone
		Math::bbox zone(transform.get_position(), this->entity->GetFloat4(Attr::ProbeParallaxBox));
		this->lightProbeEntity->SetZone(zone);
		this->environmentCapturer->SetCaptureZone(zone);
		
		// call base class
		LightProbeProperty::HandleMessage(msg);

		// set position of capture probe and move preview
		this->environmentCapturer->SetPosition(transform.get_position());
		this->reflectionMapPreview->SetTransform(matrix44::translation(transform.get_position()));
		
	}
	else if (msg->CheckId(BaseGameFeature::SetAttribute::Id))
	{
		// call base class
		LightProbeProperty::HandleMessage(msg);

		// update zone
		Math::bbox zone(this->entity->GetMatrix44(Attr::Transform).get_position(), this->entity->GetFloat4(Attr::ProbeParallaxBox));
		this->lightProbeEntity->SetZone(zone);

		// update light probe from attributes
		this->environmentCapturer->SetResolution(this->entity->GetInt(Attr::ProbeResolutionWidth), this->entity->GetInt(Attr::ProbeResolutionHeight));
		this->environmentCapturer->SetGenerateMipmaps(this->entity->GetBool(Attr::ProbeGenerateMipmaps));
		this->environmentCapturer->SetRenderIrradiance(this->entity->GetBool(Attr::ProbeBuildIrradiance));
		this->environmentCapturer->SetRenderReflections(this->entity->GetBool(Attr::ProbeBuildReflections));
		this->environmentCapturer->SetGenerateDepthCube(this->entity->GetBool(Attr::ProbeBuildDepth));
		this->environmentCapturer->SetOutput(this->entity->GetString(Attr::ProbeOutputFolder), this->entity->GetString(Attr::ProbeOutputFilename));
		this->environmentCapturer->SetName(this->entity->GetString(Attr::ProbeName));
	}
	else if (msg->CheckId(GraphicsFeature::GetModelEntity::Id))
	{
		Ptr<GraphicsFeature::GetModelEntity> gMsg = msg.downcast<GraphicsFeature::GetModelEntity>();
		gMsg->SetEntity(this->reflectionMapPreview);
	}
	else if (msg->CheckId(GraphicsFeature::SetGraphicsVisible::Id))
	{
		Ptr<GraphicsFeature::SetGraphicsVisible> gMsg = msg.downcast<GraphicsFeature::SetGraphicsVisible>();
		this->reflectionMapPreview->SetVisible(gMsg->GetVisible());
		LightProbeProperty::HandleMessage(msg);
	}
	else if (msg->CheckId(LevelEditor2::BeginProbeBuild::Id))
	{
		this->reflectionMapPreview->SetVisible(false);
	}
	else if (msg->CheckId(LevelEditor2::EndProbeBuild::Id))
	{
		this->reflectionMapPreview->SetVisible(true);
	}
	else
	{
		LightProbeProperty::HandleMessage(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EditorLightProbeProperty::OnRender()
{
	if (this->entity->GetBool(Attr::IsSelected) && this->entity->GetBool(Attr::ProbeDebugRender))
	{
		int influenceType = this->entity->GetString(Attr::ProbeInfluenceShapeType) == "sphere" ? 0 : 1;
		Math::matrix44 trans;
		
		trans = this->entity->GetMatrix44(Attr::Transform);
	
		// draw capture zone as box
		Math::bbox zone(trans.get_position(), this->entity->GetFloat4(Attr::ProbeParallaxBox));
		Math::float4 color = float4(1) - LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
		color.set_w(0.5f);
		Debug::DebugShapeRenderer::Instance()->DrawBox(zone.to_matrix44(), color, CoreGraphics::RenderShape::CheckDepth);

		if (influenceType == 0)
		{
			// sphere
			Math::float4 color = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
			color.set_w(0.5f);
			Debug::DebugShapeRenderer::Instance()->DrawSphere(trans, color, CoreGraphics::RenderShape::CheckDepth);
		}
		else
		{
			// box
			Math::float4 color = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
			color.set_w(0.5f);
			Debug::DebugShapeRenderer::Instance()->DrawBox(trans, color, CoreGraphics::RenderShape::CheckDepth);
		}
	}
}

} // namespace LevelEditor2