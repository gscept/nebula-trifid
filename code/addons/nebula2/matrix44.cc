//------------------------------------------------------------------------------
//  matrix44.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mathlib/matrix.h"
#include "mathlib/matrixdefs.h"
#include "math/matrix44.h"

//------------------------------------------------------------------------------
/**
    N2 -> N3
*/
template<> Math::matrix44
matrix44::as() const
{
    return Math::matrix44(Math::float4(this->M11, this->M12, this->M13, this->M14),
                          Math::float4(this->M21, this->M22, this->M23, this->M24),
                          Math::float4(this->M31, this->M32, this->M33, this->M34),
                          Math::float4(this->M41, this->M42, this->M43, this->M44));
}

//------------------------------------------------------------------------------
/**
    N3 -> N2
*/
template<> matrix44
Math::matrix44::as() const
{
    #if __WIN32__
        // HACK
        return ::matrix44(*(::matrix44*)this);
    #elif __PS3__
        // HACK
        return ::matrix44(*(::matrix44*)this);
    #else
    #error "Conversion from N3 matrix type to N2 matrix type not supported.   
    #endif
}

