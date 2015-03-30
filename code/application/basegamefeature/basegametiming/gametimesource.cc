//------------------------------------------------------------------------------
//  gametimesource.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "basegametiming/gametimesource.h"

namespace BaseGameFeature
{
__ImplementClass(GameTimeSource, 'TSRC', TimeSource);
__ImplementSingleton(GameTimeSource);

//------------------------------------------------------------------------------
/**
*/
GameTimeSource::GameTimeSource()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GameTimeSource::~GameTimeSource()
{
    __DestructSingleton;
}

}; // namespace Timing
