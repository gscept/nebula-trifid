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
    else if (str == "SinglePass")   return SinglePass;
    else if (str == "MultiPass")    return MultiPass;
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
        case SinglePass:    return "SinglePass";
        case MultiPass:     return "MultiPass";
        default:
            n_error("LightingMode::ToString(): invalid code!");
            return "";
    }
}

} // namespace Frame
