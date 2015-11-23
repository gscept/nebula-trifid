//------------------------------------------------------------------------------
//  lightingmode.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/lightingmode.h"

namespace Frame
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
LightingMode::Code
LightingMode::FromString(const String& str)
{
    if (str == "None")              return None;
    else if (str == "Forward")   return Forward;
    else if (str == "Deferred")    return Deferred;
    else
    {
        n_error("LightingMode::FromString(): invalid string '%s'!", str.AsCharPtr());
        return None;
    }
}

//------------------------------------------------------------------------------
/**
*/
String
LightingMode::ToString(Code c)
{
    switch (c)
    {
        case None:          return "None";
        case Forward:       return "Forward";
        case Deferred:      return "Deferred";
        default:
            n_error("LightingMode::ToString(): invalid code!");
            return "";
    }
}

} // namespace Frame
