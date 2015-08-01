//------------------------------------------------------------------------------
//  lightprobeentity.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lightprobeentity.h"
#include "lighting/lightserver.h"
#include "math/clipstatus.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadersemantics.h"

namespace Graphics
{
__ImplementClass(Graphics::LightProbeEntity, 'LPRE', Graphics::GraphicsEntity);

using namespace Math;
//------------------------------------------------------------------------------
/**
*/
LightProbeEntity::LightProbeEntity() :
    isDirty(false),
	layer(0),
	falloff(0.2f),
	power(16.0f),
	shape(Box),
	parallaxCorrected(true),
	probe(Lighting::EnvironmentProbe::DefaultEnvironmentProbe)
{
	this->SetType(GraphicsEntityType::LightProbe);
}

//------------------------------------------------------------------------------
/**
*/
Math::ClipStatus::Type
LightProbeEntity::ComputeClipStatus(const Math::bbox& box)
{	
	ClipStatus::Type clipStatus = box.clipstatus(this->transform);
	return clipStatus;
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::OnActivate()
{
    this->shader = CoreGraphics::ShaderServer::Instance()->CreateShaderInstance("shd:reflectionprojector");

    // light probe variables
    this->lightProbeReflectionVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_ENVIRONMENT);
    this->lightProbeIrradianceVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_IRRADIANCE);
    this->lightProbeFalloffVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_ENVFALLOFFDISTANCE);
    this->lightProbePowerVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_ENVFALLOFFPOWER);
    this->lightProbeReflectionNumMipsVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_NUMENVMIPS);
    this->lightProbeBboxMinVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_BBOXMIN);
    this->lightProbeBboxMaxVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_BBOXMAX);
    this->lightProbeBboxCenterVar = this->shader->CreateVariableInstance(NEBULA3_SEMANTIC_BBOXCENTER);
    this->lightProbeTransformVar = this->shader->CreateVariableInstance("Transform");
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::OnDeactivate()
{
    // discard variables
    this->lightProbeReflectionVar->Discard();
    this->lightProbeReflectionVar = 0;
    this->lightProbeIrradianceVar->Discard();
    this->lightProbeIrradianceVar = 0;
    this->lightProbeFalloffVar->Discard();
    this->lightProbeFalloffVar = 0;
    this->lightProbePowerVar->Discard();
    this->lightProbePowerVar = 0;
    this->lightProbeReflectionNumMipsVar->Discard();
    this->lightProbeReflectionNumMipsVar = 0;
    this->lightProbeBboxMinVar->Discard();
    this->lightProbeBboxMinVar = 0;
    this->lightProbeBboxMaxVar->Discard();
    this->lightProbeBboxMaxVar = 0;
    this->lightProbeBboxCenterVar->Discard();
    this->lightProbeBboxCenterVar = 0;
    this->lightProbeTransformVar->Discard();
    this->lightProbeTransformVar = 0;

    // discard shader
    this->shader->Discard();
    this->shader = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::ApplyProbe(const Ptr<Lighting::EnvironmentProbe>& probe)
{
    this->lightProbeReflectionVar->SetTexture(probe->GetReflectionMap()->GetTexture());
    this->lightProbeIrradianceVar->SetTexture(probe->GetIrradianceMap()->GetTexture());
    this->lightProbeReflectionNumMipsVar->SetInt(probe->GetReflectionMap()->GetTexture()->GetNumMipLevels());
}

//------------------------------------------------------------------------------
/**
*/
LightProbeEntity::~LightProbeEntity()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod)
{
	Lighting::LightServer::Instance()->AttachVisibleLightProbe(this);
    if (this->isDirty)
    {
        this->lightProbeFalloffVar->SetFloat(this->falloff);
        this->lightProbePowerVar->SetFloat(this->power);
        this->lightProbeBboxMinVar->SetFloat4(this->zone.pmin);
        this->lightProbeBboxMaxVar->SetFloat4(this->zone.pmax);
        this->lightProbeBboxCenterVar->SetFloat4(this->GetTransform().get_position());
        this->lightProbeTransformVar->SetMatrix(this->GetTransform());
        this->isDirty = false;
    }    
}

} // namespace Graphics