//------------------------------------------------------------------------------
//  depthoffieldparams.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "depthoffieldparams.h"
#include "math/scalar.h"

namespace PostEffect
{

__ImplementClass(PostEffect::DepthOfFieldParams, 'PEDF', PostEffect::ParamBase);

//------------------------------------------------------------------------------
/**
*/
DepthOfFieldParams::DepthOfFieldParams() :
    focusDistance(0.0f),
    focusLength(100.0f),
    filterSize(0.0f)
{
    // empty
}    

//------------------------------------------------------------------------------
/**
*/
DepthOfFieldParams::~DepthOfFieldParams()
{
    // empty
}    

//------------------------------------------------------------------------------
/**
*/
void 
DepthOfFieldParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(DepthOfFieldParams::RTTI));

	if (this != rhs.get())
	{
		this->focusDistance = rhs.cast<DepthOfFieldParams>()->focusDistance;
		this->focusLength = rhs.cast<DepthOfFieldParams>()->focusLength;
		this->filterSize = rhs.cast<DepthOfFieldParams>()->filterSize;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
void
DepthOfFieldParams::BlendTo(const Ptr<ParamBase>& p, float l)
{
    n_assert(p->IsA(DepthOfFieldParams::RTTI));

    this->focusDistance        = Math::n_lerp(this->focusDistance, p.cast<DepthOfFieldParams>()->focusDistance, l);
    this->focusLength          = Math::n_lerp(this->focusLength, p.cast<DepthOfFieldParams>()->focusLength, l);
    this->filterSize           = Math::n_lerp(this->filterSize, p.cast<DepthOfFieldParams>()->filterSize, l);
}

//------------------------------------------------------------------------------
/**
*/
bool
DepthOfFieldParams::Equals(const Ptr<ParamBase>& v2, float threshold) const
{
    n_assert(v2->IsA(DepthOfFieldParams::RTTI));
    const Ptr<DepthOfFieldParams> temp = v2.cast<DepthOfFieldParams>();

    if( Math::n_abs(temp->GetFilterSize() - this->GetFilterSize()) < threshold &&
        Math::n_abs(temp->GetFocusDistance() - this->GetFocusDistance()) < threshold &&
        Math::n_abs(temp->GetFocusLength() - this->GetFocusLength()) < threshold)
    {
        return true;
    }
    return false;
}
} // namespace PostEffects
