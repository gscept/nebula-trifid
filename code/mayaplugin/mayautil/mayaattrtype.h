#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaAttrType
    
    Attribute types accepted by the N3 Maya plugin.  
    
    (C) 2009 Radon Labs GmbH
*/    
#include "core/types.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaAttrType
{
public:
    /// enums
    enum Code
    {
        Int,
        Float,
        Bool,
        Vector,
        String,
        Enum,

        IntArray,
        FloatArray,
        VectorArray,
        StringArray,
    
        InvalidAttrType,
    };

    /// convert to string
    static Util::String ToString(MayaAttrType::Code c);
    /// convert from string
    static MayaAttrType::Code FromString(const Util::String& str);
};

} // namespace Maya
//------------------------------------------------------------------------------
