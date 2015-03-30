//------------------------------------------------------------------------------
//  vector.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mathlib/vector.h"
#include "math/float4.h"
#include "math/float2.h"

//------------------------------------------------------------------------------
/**
    N2 -> N3
*/
template<> Math::float4
vector4::as() const
{
    return Math::float4(this->x, this->y, this->z, this->w);
}

//------------------------------------------------------------------------------
/**
    N3 -> N2
*/
template<> vector2
Math::float2::as() const
{
    return vector2(this->X, this->Y);
}
