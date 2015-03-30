//------------------------------------------------------------------------------
//  frustum.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mathlib/frustum.h"
#include "math/frustum.h"

//------------------------------------------------------------------------------
/**
    N3 -> N2
*/
template<> frustum
Math::frustum::as() const
{
    // HACK!
    return ::frustum(*(::frustum*)this);
}