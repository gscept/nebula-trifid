#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShaderFilter
    
    Shader filters are used to filter shapes and polygons by assigned
    shader type.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShaderFilter
{
public:
    /// shader filter enum
    enum Code
    {
        Standard = 0,
        Particle,

        NumShaderFilters,
        InvalidShaderFilter,
    };

    /// convert from string
    static Code FromString(const char* str);
    /// convert to string
    static const char* ToString(Code c);
};

//------------------------------------------------------------------------------
/**
*/
inline MayaShaderFilter::Code
MayaShaderFilter::FromString(const char* str)
{
    if (0 == Util::String::StrCmp(str, "Standard")) return Standard;
    else if (0 == Util::String::StrCmp(str, "Particle")) return Particle;
    else
    {
        n_error("MayaShaderFilter::FromString(): invalid shader filter '%s'!\n", str);
        return InvalidShaderFilter;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const char*
MayaShaderFilter::ToString(Code c)
{
    switch (c)
    {
        case Standard: return "Standard";
        case Particle: return "Particle";
        default:
            n_error("MayaShaderFilter::ToString(): invalid shader filter code '%d'!\n", c);
            return "INVALID";
    }
}

} // namespace Maya
//------------------------------------------------------------------------------
