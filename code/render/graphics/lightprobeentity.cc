//------------------------------------------------------------------------------
//  lightprobeentity.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
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
    numMips(0),
	shape(Box),
	correctionMethod(None),
	probe(Lighting::EnvironmentProbe::DefaultEnvironmentProbe)
{
	this->SetType(GraphicsEntityType::LightProbe);
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
	// run base class
	GraphicsEntity::OnActivate();

    this->shader = CoreGraphics::ShaderServer::Instance()->GetShader("shd:reflectionprojector");

    // light probe variables
    this->lightProbeReflectionVar = this->shader->GetVariableByName(NEBULA3_SEMANTIC_ENVIRONMENT);
	this->lightProbeIrradianceVar = this->shader->GetVariableByName(NEBULA3_SEMANTIC_IRRADIANCE);
	this->lightProbeDepthVar = this->shader->GetVariableByName(NEBULA3_SEMANTIC_DEPTHCONEMAP);

	// create variable buffer
	this->lightProbeVariableBuffer = CoreGraphics::ConstantBuffer::Create();
	this->lightProbeVariableBuffer->SetSync(true);
	this->lightProbeVariableBuffer->SetupFromBlockInShader(this->shader, "ReflectionProjectorBlock", 1);
	this->lightProbeBufferVar = this->shader->GetVariableByName("ReflectionProjectorBlock");

	// setup variables
	this->lightProbeFalloffVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_ENVFALLOFFDISTANCE);
	this->lightProbePowerVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_ENVFALLOFFPOWER);
	this->lightProbeReflectionNumMipsVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_NUMENVMIPS);
	this->lightProbeBboxMinVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_BBOXMIN);
	this->lightProbeBboxMaxVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_BBOXMAX);
	this->lightProbeBboxCenterVar = this->lightProbeVariableBuffer->GetVariableByName(NEBULA3_SEMANTIC_BBOXCENTER);
	this->lightProbeTransformVar = this->lightProbeVariableBuffer->GetVariableByName("Transform");
	this->lightProbeInvTransformVar = this->lightProbeVariableBuffer->GetVariableByName("InvTransform");
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::OnDeactivate()
{
    // discard variables
    this->lightProbeReflectionVar = 0;
    this->lightProbeIrradianceVar = 0;
    this->lightProbeFalloffVar = 0;
    this->lightProbePowerVar = 0;
    this->lightProbeReflectionNumMipsVar = 0;
    this->lightProbeBboxMinVar = 0;
    this->lightProbeBboxMaxVar = 0;
    this->lightProbeBboxCenterVar = 0;
    this->lightProbeTransformVar = 0;
	this->lightProbeInvTransformVar = 0;
	this->lightProbeBufferVar->SetBufferHandle(NULL);
	this->lightProbeBufferVar = 0;

	// discard buffer
	this->lightProbeVariableBuffer->Discard();
	this->lightProbeVariableBuffer = 0;

    // discard shader
    this->shader = 0;

	// run base class
	GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProbeEntity::ApplyProbe(const Ptr<Lighting::EnvironmentProbe>& probe)
{
    this->lightProbeReflectionVar->SetTexture(probe->GetReflectionMap()->GetTexture());
    this->lightProbeIrradianceVar->SetTexture(probe->GetIrradianceMap()->GetTexture());
	this->lightProbeDepthVar->SetTexture(probe->GetDepthMap()->GetTexture());
    uint mips = probe->GetReflectionMap()->GetTexture()->GetNumMipLevels();
	if (mips != this->numMips)
    {
        this->numMips = mips;
        this->isDirty = true;
    }

	// enable buffer
	this->lightProbeBufferVar->SetBufferHandle(this->lightProbeVariableBuffer->GetHandle());    
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
		const matrix44 trans = this->GetTransform();
		this->lightProbeVariableBuffer->CycleBuffers();
		this->lightProbeVariableBuffer->BeginUpdateSync();
		this->lightProbeReflectionNumMipsVar->SetInt(this->numMips);
        this->lightProbeFalloffVar->SetFloat(this->falloff);
        this->lightProbePowerVar->SetFloat(this->power);
        this->lightProbeBboxMinVar->SetFloat4(this->zone.pmin);
        this->lightProbeBboxMaxVar->SetFloat4(this->zone.pmax);
		this->lightProbeBboxCenterVar->SetFloat4(trans.get_position());
		this->lightProbeTransformVar->SetMatrix(trans);
		this->lightProbeInvTransformVar->SetMatrix(matrix44::inverse(trans));
		this->lightProbeVariableBuffer->EndUpdateSync();
        this->isDirty = false;
    }    
}

} // namespace Graphics