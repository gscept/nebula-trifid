//------------------------------------------------------------------------------
//  aoparams.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "aoparams.h"

namespace PostEffect
{
__ImplementClass(PostEffect::AoParams, 'AOPA', PostEffect::ParamBase);

//------------------------------------------------------------------------------
/**
*/
AoParams::AoParams() :
    strength(2.0f),
    angleBias(10.0f),
    power(1.0f),
    radius(12.0f)    
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AoParams::~AoParams()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AoParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(AoParams::RTTI));

	if (this != rhs.get())
	{
		const Ptr<AoParams> temp = rhs.cast<AoParams>();

		this->strength = temp->strength;
		this->angleBias = temp->angleBias;
		this->power = temp->power;
		this->radius = temp->radius;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
void 
AoParams::BlendTo( const Ptr<ParamBase>& p, float l )
{
    n_assert(p->IsA(AoParams::RTTI));

    const Ptr<AoParams> temp = p.cast<AoParams>();

    this->strength      = Math::n_lerp(this->strength, temp->strength, l);
    this->angleBias     = Math::n_lerp(this->angleBias, temp->angleBias, l);
    this->power		    = Math::n_lerp(this->power, temp->power, l);
    this->radius		= Math::n_lerp(this->radius, temp->radius, l);
}

//------------------------------------------------------------------------------
/**
*/
bool 
AoParams::Equals( const Ptr<ParamBase>& v2, float threshold ) const
{
    n_assert(v2->IsA(AoParams::RTTI));
    const Ptr<AoParams> temp = v2.cast<AoParams>();

    if (Math::n_abs(temp->GetStrength() - this->GetStrength()) < threshold &&
        Math::n_abs(temp->GetAngleBias() - this->GetAngleBias()) < threshold &&
        Math::n_abs(temp->GetPower() - this->GetPower()) < threshold &&
        Math::n_abs(temp->GetRadius() - this->GetRadius()) < threshold)
    {
        return true;
    }
    return false;
}
} // namespace PostEffect