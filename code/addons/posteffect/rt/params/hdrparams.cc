//------------------------------------------------------------------------------
//  hdrparams.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "hdrparams.h"

namespace PostEffect
{
__ImplementClass(PostEffect::HdrParams, 'PEHP', PostEffect::ParamBase);

using namespace Math;
//------------------------------------------------------------------------------
/**
*/
HdrParams::HdrParams() :
    hdrBloomIntensity(1.0f),
    hdrBloomThreshold(1.0f),
    hdrBloomColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
HdrParams::~HdrParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HdrParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(HdrParams::RTTI));

	if (this != rhs.get())
	{
		this->hdrBloomIntensity = rhs.cast<HdrParams>()->hdrBloomIntensity;
		this->hdrBloomColor     = rhs.cast<HdrParams>()->hdrBloomColor;
		this->hdrBloomThreshold = rhs.cast<HdrParams>()->hdrBloomThreshold;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
void
HdrParams::BlendTo(const Ptr<ParamBase>& p, float l)
{
    n_assert(p->IsA(HdrParams::RTTI));

    this->hdrBloomIntensity     = n_lerp(this->hdrBloomIntensity, p.cast<HdrParams>()->hdrBloomIntensity, l);
    this->hdrBloomColor         = float4::lerp(this->hdrBloomColor, p.cast<HdrParams>()->hdrBloomColor, l);
    this->hdrBloomThreshold     = n_lerp(this->hdrBloomThreshold, p.cast<HdrParams>()->hdrBloomThreshold, l);
}

//------------------------------------------------------------------------------
/**
*/
bool
HdrParams::Equals(const Ptr<ParamBase>& v2, float threshold) const
{
    n_assert(v2->IsA(HdrParams::RTTI));
    const Ptr<HdrParams> temp = v2.cast<HdrParams>();
    
    if( Math::float4::nearequal4(temp->GetHdrBloomColor(), this->GetHdrBloomColor(), Math::float4(threshold, threshold, threshold, threshold)) &&
        Math::n_abs(temp->GetHdrBloomIntensity() - this->GetHdrBloomIntensity()) < threshold &&
        Math::n_abs(temp->GetHdrBloomThreshold() - this->GetHdrBloomThreshold()) < threshold)
    {
        return true;
    }
    return false;
}
} // namespace PostEffects
