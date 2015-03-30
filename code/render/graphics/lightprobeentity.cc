//------------------------------------------------------------------------------
//  lightprobeentity.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lightprobeentity.h"
#include "lighting/lightserver.h"
#include "math/clipstatus.h"

namespace Graphics
{
__ImplementClass(Graphics::LightProbeEntity, 'LPRE', Graphics::GraphicsEntity);

using namespace Math;
//------------------------------------------------------------------------------
/**
*/
LightProbeEntity::LightProbeEntity() :
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
}

} // namespace Graphics