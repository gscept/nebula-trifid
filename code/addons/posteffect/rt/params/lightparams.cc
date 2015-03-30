//------------------------------------------------------------------------------
//  lightparams.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lightparams.h"
#include "math/point.h"
#include "math/quaternion.h"

namespace PostEffect
{
__ImplementClass(PostEffect::LightParams, 'PELP', PostEffect::ParamBase);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
LightParams::LightParams() :
    backLightOffset(0.0f),
    lightShadowIntensity(0.5f),
    lightColor(1.0f, 1.0f, 1.0f, 1.0f),
    lightOppositeColor(0.0f, 0.0f, 0.0f, 1.0f),
    lightAmbientColor(0.0f, 0.0f, 0.0f, 1.0f),
    lightIntensity(0.4f),
	lightShadowBias(0.005f),
	lightCastsShadows(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LightParams::~LightParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
LightParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(ParamBase::RTTI));

	if (this != rhs.get())
	{
		this->lightTransform = rhs.cast<LightParams>()->lightTransform;
		this->lightColor = rhs.cast<LightParams>()->lightColor;
		this->lightOppositeColor = rhs.cast<LightParams>()->lightOppositeColor;
		this->lightAmbientColor = rhs.cast<LightParams>()->lightAmbientColor;
		this->backLightOffset = rhs.cast<LightParams>()->backLightOffset;
		this->lightShadowBias = rhs.cast<LightParams>()->lightShadowBias;
		this->lightShadowIntensity = rhs.cast<LightParams>()->lightShadowIntensity;
		this->lightIntensity = rhs.cast<LightParams>()->lightIntensity;
		this->lightCastsShadows = rhs.cast<LightParams>()->lightCastsShadows;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
void
LightParams::BlendTo(const Ptr<ParamBase>& p, float l)
{
    n_assert(p->IsA(LightParams::RTTI));
    const Ptr<LightParams> temp = p.cast<LightParams>();

    this->lightColor            = float4::lerp(this->lightColor, temp->lightColor, l);
    this->lightOppositeColor    = float4::lerp(this->lightOppositeColor, temp->lightOppositeColor, l);
    this->lightAmbientColor     = float4::lerp(this->lightAmbientColor, temp->lightAmbientColor, l);
    this->backLightOffset       = n_lerp(this->backLightOffset, temp->backLightOffset, l);
	this->lightShadowBias		= n_lerp(this->lightShadowBias, temp->lightShadowBias, l);
    this->lightShadowIntensity  = n_lerp(this->lightShadowIntensity, temp->lightShadowIntensity, l);
    this->lightIntensity        = n_lerp(this->lightIntensity, temp->lightIntensity, l);
	this->lightCastsShadows		= temp->lightCastsShadows;

    // perform light direction interpolation
    matrix44 m0 = this->lightTransform;
    m0.set_xaxis(float4::normalize(m0.get_xaxis()));
    m0.set_yaxis(float4::normalize(m0.get_yaxis()));
    m0.set_zaxis(float4::normalize(m0.get_zaxis()));
    m0.set_position(point(0.0f, 0.0f, 0.0f));
    matrix44 m1 = temp->GetLightTransform();
    m1.set_xaxis(float4::normalize(m1.get_xaxis()));
    m1.set_yaxis(float4::normalize(m1.get_yaxis()));
    m1.set_zaxis(float4::normalize(m1.get_zaxis()));
    m1.set_position(point(0.0f, 0.0f, 0.0f));
    quaternion q0 = matrix44::rotationmatrix(m0);
    quaternion q1 = matrix44::rotationmatrix(m1);
    quaternion q = q.slerp(q0, q1, l);
    this->lightTransform = matrix44::rotationquaternion(q);
}

//------------------------------------------------------------------------------
/**
*/
bool
LightParams::Equals(const Ptr<ParamBase>& v2, float threshold) const
{
    n_assert(v2->IsA(LightParams::RTTI));
    const Ptr<LightParams> temp = v2.cast<LightParams>();

    Math::float4 e(threshold, threshold, threshold, threshold);
    if( Math::float4::nearequal4(temp->GetLightColor(), this->GetLightColor(), e) &&
        Math::float4::nearequal4(temp->GetLightOppositeColor(), this->GetLightOppositeColor(), e) &&
        Math::float4::nearequal4(temp->GetLightAmbientColor(), this->GetLightAmbientColor(), e) &&        
        Math::n_abs(temp->GetLightIntensity() - this->GetLightIntensity()) < threshold &&
        Math::n_abs(temp->GetBackLightFactor() - this->GetBackLightFactor()) < threshold &&
        Math::n_abs(temp->GetLightShadowIntensity() - this->GetLightShadowIntensity()) < threshold &&
		Math::n_abs(temp->GetLightShadowBias() - this->GetLightShadowBias()) < threshold &&
		temp->GetLightCastsShadows() == this->lightCastsShadows)
    {
        return true;
    }
    return false;
}

} // namespace PostEffects
