//------------------------------------------------------------------------------
//  mayaattrtype.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayautil/mayaattrtype.h"

namespace Maya
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
String
MayaAttrType::ToString(MayaAttrType::Code c)
{
    switch (c)
    {
        case Int:           return "int";
        case Float:         return "float";
        case Bool:          return "bool";
        case Vector:        return "vector";
        case String:        return "string";
        case Enum:          return "enum";
        case IntArray:      return "intarray";
        case FloatArray:    return "floatarray";
        case VectorArray:   return "vectorarray";
        case StringArray:   return "stringarray";
    
        default:
            n_error("MayaAttrType::ToString: invalid code '%d'!", c);
            return "";
    }
}

//------------------------------------------------------------------------------
/**
*/
MayaAttrType::Code 
MayaAttrType::FromString(const Util::String& str)
{
    if (str == "int") return Int;
    else if (str == "float") return Float;
    else if (str == "bool") return Bool;
    else if (str == "vector") return Vector;
    else if (str == "string") return String;
    else if (str == "enum") return Enum;
    else if (str == "intarray") return IntArray;
    else if (str == "floatarray") return FloatArray;
    else if (str == "vectorarray") return VectorArray;
    else if (str == "stringarray") return StringArray;
    else
    {
        n_error("MayaAttrType::FromString(): invalid string '%s'!", str.AsCharPtr());
        return InvalidAttrType;
    }
}

} // namespace Maya
