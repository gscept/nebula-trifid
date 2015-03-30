#pragma once
#ifndef SCRIPTING_ARG_H
#define SCRIPTING_ARG_H
//------------------------------------------------------------------------------
/**
    @class Scripting::Arg
  
    An argument for scripting, this is just a typedef from Util::Variant.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/variant.h"

//------------------------------------------------------------------------------
namespace Scripting
{
typedef Util::Variant Arg;
};
//------------------------------------------------------------------------------
#endif
