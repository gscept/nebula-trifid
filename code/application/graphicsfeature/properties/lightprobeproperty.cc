//------------------------------------------------------------------------------
//  lightprobeproperty.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lightprobeproperty.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphicsfeatureunit.h"
#include "lighting/environmentprobe.h"

using namespace Math;
using namespace Graphics;
using namespace Lighting;
namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::LightProbeProperty, 'LIPP', Game::Property);

//------------------------------------------------------------------------------
/**
*/
LightProbeProperty::LightProbeProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
LightProbeProperty::~LightProbeProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeProperty::OnActivate()
{
	Property::OnActivate();

	// create environment probe
	this->environmentProbe = EnvironmentProbe::Create();
	this->environmentProbe->AssignIrradianceMap("tex:" + this->entity->GetString(Attr::ProbeIrradianceMap) + NEBULA3_TEXTURE_EXTENSION);
	this->environmentProbe->AssignReflectionMap("tex:" + this->entity->GetString(Attr::ProbeReflectionMap) + NEBULA3_TEXTURE_EXTENSION);

	// get matrix
	Math::matrix44 transform = this->entity->GetMatrix44(Attr::Transform);

	// setup light probe entity
	this->lightProbeEntity = LightProbeEntity::Create();
	this->lightProbeEntity->SetEnvironmentProbe(this->environmentProbe);
	this->lightProbeEntity->SetTransform(transform);

	// set capture zone in probe entity
	Math::bbox zone(transform.get_position(), this->entity->GetFloat4(Attr::ProbeBBExtents));
	this->lightProbeEntity->SetZone(zone);

	// set shape type
	LightProbeEntity::LightProbeShapeType type = this->entity->GetString(Attr::ProbeInfluenceShapeType) == "sphere" ? LightProbeEntity::Sphere : LightProbeEntity::Box;
	this->lightProbeEntity->SetShapeType(type);
	this->lightProbeEntity->SetLayer(this->entity->GetInt(Attr::ProbeInfluenceLayer));
	this->lightProbeEntity->SetFalloff(this->entity->GetFloat(Attr::ProbeInfluenceFalloff));
	this->lightProbeEntity->SetPower(this->entity->GetFloat(Attr::ProbeInfluencePower));
	this->lightProbeEntity->SetParallaxCorrected(this->entity->GetBool(Attr::ProbeParallaxCorrected));

	// attach model to stage
	Ptr<Graphics::Stage> stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	n_assert(stage);
	stage->AttachEntity(this->lightProbeEntity.upcast<Graphics::GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeProperty::OnDeactivate()
{
	// remove model
	Ptr<Graphics::Stage> stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	n_assert(stage);
	stage->RemoveEntity(this->lightProbeEntity.upcast<Graphics::GraphicsEntity>());
	this->lightProbeEntity = 0;

	Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeProperty::SetupAcceptedMessages()
{
	Property::SetupAcceptedMessages();
	this->RegisterMessage(BaseGameFeature::UpdateTransform::Id);
	this->RegisterMessage(BaseGameFeature::SetAttribute::Id);
	this->RegisterMessage(BaseGameFeature::GetAttribute::Id);
	this->RegisterMessage(GraphicsFeature::GetModelEntity::Id);
	this->RegisterMessage(GraphicsFeature::SetGraphicsVisible::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(BaseGameFeature::UpdateTransform::Id))
	{
		Math::matrix44 transform = (msg.cast<BaseGameFeature::UpdateTransform>())->GetMatrix();
		this->lightProbeEntity->SetTransform(transform);
	}
	else if (msg->CheckId(GraphicsFeature::SetGraphicsVisible::Id))
	{
		Ptr<GraphicsFeature::SetGraphicsVisible> gmsg = msg.cast<GraphicsFeature::SetGraphicsVisible>();
		this->lightProbeEntity->SetVisible(gmsg->GetVisible());
	}
	else if (msg->CheckId(BaseGameFeature::SetAttribute::Id))
	{
		Ptr<BaseGameFeature::SetAttribute> gmsg = msg.cast<BaseGameFeature::SetAttribute>();
		this->entity->SetAttr(gmsg->GetAttr());

		// update light probe from attributes
		this->environmentProbe->AssignReflectionMap("tex:" + this->entity->GetString(Attr::ProbeReflectionMap) + NEBULA3_TEXTURE_EXTENSION);
		this->environmentProbe->AssignIrradianceMap("tex:" + this->entity->GetString(Attr::ProbeIrradianceMap) + NEBULA3_TEXTURE_EXTENSION);

		// 0 is sphere, 1 is box
		LightProbeEntity::LightProbeShapeType type = this->entity->GetString(Attr::ProbeInfluenceShapeType) == "sphere" ? LightProbeEntity::Sphere : LightProbeEntity::Box;
		this->lightProbeEntity->SetShapeType(type);
		this->lightProbeEntity->SetLayer(this->entity->GetInt(Attr::ProbeInfluenceLayer));
		this->lightProbeEntity->SetFalloff(this->entity->GetFloat(Attr::ProbeInfluenceFalloff));
		this->lightProbeEntity->SetPower(this->entity->GetFloat(Attr::ProbeInfluencePower));
		this->lightProbeEntity->SetParallaxCorrected(this->entity->GetBool(Attr::ProbeParallaxCorrected));

		// notify entity that attributes have been changed
		Ptr<BaseGameFeature::AttributesUpdated> upd = BaseGameFeature::AttributesUpdated::Create();
		this->entity->SendSync(upd.cast<Messaging::Message>());
	}
	else
	{
		Property::HandleMessage(msg);
	}
}

} // namespace GraphicsFeature