#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::SystemTimeSource
    
    Provides the time for the various system features.
    These features has to get the time from this timesource.

    Access the SystemTimeSource object as Singleton:

    Time sysTime = SystemTimeSource::Instance()->GetTime();

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "basegametiming/timesource.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class SystemTimeSource : public TimeSource
{
    __DeclareClass(SystemTimeSource);
    __DeclareSingleton(SystemTimeSource);

public:
    /// constructor
    SystemTimeSource();
    /// destructor
    virtual ~SystemTimeSource();
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
