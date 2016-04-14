//------------------------------------------------------------------------------
//  fogparams.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fogparams.h"

namespace PostEffect
{
__ImplementClass(PostEffect::FogParams, 'PEFP', PostEffect::ParamBase);

using namespace Math;
//------------------------------------------------------------------------------
/**
*/
FogParams::FogParams() :
    fogNearDist(0.0f),
    fogFarDist(5000.0f),
    fogHeight(400.0f),
    fogColor(0.5f, 0.5f, 0.5f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FogParams::~FogParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FogParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(FogParams::RTTI));

	if (this != rhs.get())
	{
		this->fogNearDist = rhs.cast<FogParams>()->fogNearDist;
		this->fogFarDist = rhs.cast<FogParams>()->fogFarDist;
		this->fogHeight = rhs.cast<FogParams>()->fogHeight;
		this->fogColor = rhs.cast<FogParams>()->fogColor;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
void
FogParams::BlendTo(const Ptr<ParamBase>& p, float l)
{
    n_assert(p->IsA(FogParams::RTTI));

    this->fogNearDist           = n_lerp(this->fogNearDist, p.cast<FogParams>()->fogNearDist, l);
    this->fogFarDist            = n_lerp(this->fogFarDist, p.cast<FogParams>()->fogFarDist, l);
    this->fogHeight             = n_lerp(this->fogHeight, p.cast<FogParams>()->fogHeight, l);
    this->fogColor              = float4::lerp(this->fogColor, p.cast<FogParams>()->fogColor, l);
}

//------------------------------------------------------------------------------
/**
*/
bool
FogParams::Equals(const Ptr<ParamBase>& v2, float threshold) const
{
    n_assert(v2->IsA(FogParams::RTTI));
    const Ptr<FogParams> temp = v2.cast<FogParams>();

    if( Math::float4::nearequal4(temp->GetFogColorAndIntensity(), this->GetFogColorAndIntensity(), Math::float4(threshold, threshold, threshold, threshold)) &&
        Math::n_abs(temp->GetFogFarDistance() - this->GetFogFarDistance()) < threshold &&
        Math::n_abs(temp->GetFogHeight() - this->GetFogHeight()) < threshold &&
        Math::n_abs(temp->GetFogNearDistance() - this->GetFogNearDistance()) < threshold)
    {
        return true;
    }
    return false;
}
} // namespace PostEffects
