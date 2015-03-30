//------------------------------------------------------------------------------
//  bbox.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

// nebula3 includes
#include "math/bbox.h"

// nebula2 includes
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
/**
*/
template<> Math::bbox
bbox3::as() const
{
    Math::bbox n3Box;
    n3Box.pmin.set(this->vmin.x, this->vmin.y, this->vmin.z);
    n3Box.pmax.set(this->vmax.x, this->vmax.y, this->vmax.z);
    return n3Box;
}

//------------------------------------------------------------------------------
/**
*/
template<> bbox3
Math::bbox::as() const
{
    bbox3 n2Box;
    n2Box.vmin.set(this->pmin.x(), this->pmin.y(), this->pmin.z());
    n2Box.vmax.set(this->pmax.x(), this->pmax.y(), this->pmax.z());
    return n2Box;
}
