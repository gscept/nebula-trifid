#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::TimingTimeSource
    
    Provides timing information for the Timing logic.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "basegametiming/timesource.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class GameTimeSource : public TimeSource
{
    __DeclareClass(GameTimeSource);
    __DeclareSingleton(GameTimeSource);

public:
    /// constructor
    GameTimeSource();
    /// destructor
    virtual ~GameTimeSource();
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------

