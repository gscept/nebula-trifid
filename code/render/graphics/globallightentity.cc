//------------------------------------------------------------------------------
//  globallightentity.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/globallightentity.h"
#include "lighting/shadowserver.h"
#include "lighting/lightserver.h"

namespace Graphics
{
__ImplementClass(Graphics::GlobalLightEntity, 'GLBE', Graphics::AbstractLightEntity);

using namespace Math;
using namespace Messaging;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
GlobalLightEntity::GlobalLightEntity() :
    backLightColor(0.0f, 0.0f, 0.0f, 0.0f),
    ambientLightColor(0.0f, 0.0f, 0.0f, 0.0f),
    lightDir(0.0f, 0.0f, -1.0f),
    backLightOffset(0.3)
{
    this->SetLightType(LightType::Global);
	this->SetAlwaysVisible(true);
}

//------------------------------------------------------------------------------
/**
*/
ClipStatus::Type
GlobalLightEntity::ComputeClipStatus(const Math::bbox& box)
{
    // since we are essentially a directional light, everything is visible,
    // we depend on the visibility detection code in the Cell class to
    // only create light links for objects that are actually visible
    return ClipStatus::Inside;
}

//------------------------------------------------------------------------------
/**
*/
void
GlobalLightEntity::OnTransformChanged()
{
	AbstractLightEntity::OnTransformChanged();

	// if the light transforms then all shadows must be updated
	this->castShadowsThisFrame = this->castShadows;

    // extract the light's direction from the transformation matrix
    this->lightDir = float4::normalize(this->transform.getrow2());
	
	// calculate shadow transform by doing a simple lookat
	this->shadowTransform = matrix44::lookatrh(point(0), this->lightDir, vector::upvec());

    // extend shadow casting frustum so that the global light resolves everything within the shadow casting range
    this->transform.set_xaxis(float4::normalize(this->transform.get_xaxis()) * 1000000.0f);
    this->transform.set_yaxis(float4::normalize(this->transform.get_yaxis()) * 1000000.0f);
    this->transform.set_zaxis(float4::normalize(this->transform.get_zaxis()) * 1000000.0f);
}

//------------------------------------------------------------------------------
/**
*/
void 
GlobalLightEntity::OnRenderDebug()
{
    // TODO: render shadow frustum?
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
GlobalLightEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(GlobalLightEntity, this, msg);
}


} // namespace Graphics
