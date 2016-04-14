//------------------------------------------------------------------------------
//  inputtimesource.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "basegametiming/inputtimesource.h"

namespace BaseGameFeature
{
__ImplementClass(InputTimeSource, 'TSIN', TimeSource);
__ImplementSingleton(InputTimeSource);

//------------------------------------------------------------------------------
/**
*/
InputTimeSource::InputTimeSource()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
InputTimeSource::~InputTimeSource()
{
    __DestructSingleton;
}

}; // namespace Timing
