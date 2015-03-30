#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::TimeSource
    
    A generic time source object which is attached to the TimeManager.
    Each time source tracks its own time independently from the other
    time sources, they can also be paused and unpaused independentlty 
    from each other, and they may also run faster or slower then
    realtime. 
    
    To create new time sources, derive a subclass from time source. This
    is necessary because time source objects are identified by their
    class id. A positive side effect of this is that time sources
    are created as singletons, so access is very simple, e.g.:

    Time gameTime = GameTime::Instance()->GetTime();

    Time source classes are responsible for loading/saving their
    state into the globals attribute table.

    All timesources have to be passiv, everyone who needs time has to 
    get it on its own from one of the different timsources. 
    No time source should set the time in a subsystem activly.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "addons/db/reader.h"
#include "addons/db/writer.h"
#include "timing/time.h"


//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class TimeManager;

class TimeSource : public Core::RefCounted
{
    __DeclareClass(TimeSource);

public:
    /// constructor
    TimeSource();
    /// destructor
    virtual ~TimeSource();

    /// called when time source is attached to the time manager
    virtual void OnActivate();
    /// called when time source is removed from the time manager
    virtual void OnDeactivate();
    /// read state from database reader
    virtual void OnLoad(const Ptr<Db::Reader>& dbReader);
    /// write state to database writer
    virtual void OnSave(const Ptr<Db::Writer>& dbWriter);

    /// reset the time source to 0.0
    virtual void Reset();
    /// pause the time source (increments pause counter)
    virtual void Pause();
    /// unpause the time source (decrements pause counter)
    virtual void Continue();
    /// return true if currently paused
    bool IsPaused() const;

    /// set acceleration/deceleration factor
    virtual void SetFactor(float f);
    /// get acceleration/deceleration factor
    float GetFactor() const;
    /// get the current time
    Timing::Time GetTime() const;
    /// get the current frame time
    Timing::Time GetFrameTime() const;
    /// get a unique frame id
    uint GetFrameId() const;
    /// get current ticks
    Timing::Tick GetTicks() const;

protected:
    friend class TimeManager;

    /// update current time (called by time manager)
    virtual void UpdateTime(Timing::Time frameTime, Timing::Tick t);

    Timing::Time frameTime; 
    Timing::Time time;
    Timing::Tick ticks;
    int pauseCounter;
    float timeFactor;
};

//------------------------------------------------------------------------------
/**
*/
inline Timing::Time
TimeSource::GetFrameTime() const
{
    if (this->IsPaused())
    {
        return 0.0f;
    }
    else
    {
        return this->frameTime;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline Timing::Time
TimeSource::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Timing::Tick 
TimeSource::GetTicks() const
{
    return this->ticks;
}

//------------------------------------------------------------------------------
/**
*/
inline void
TimeSource::SetFactor(float f)
{
    n_assert(f > 0.0f);
    this->timeFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float
TimeSource::GetFactor() const
{
    return this->timeFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TimeSource::IsPaused() const
{
    return (this->pauseCounter > 0);
}

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
