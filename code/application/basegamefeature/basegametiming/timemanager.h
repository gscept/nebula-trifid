#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::TimeManager

    Singleton object which manages the current game time. These are
    the standard time source objects provided by Application layer:

    SystemTimeSource    - timing for low level Application layer subsystems
    GameTimeSource      - timing for the game logic
    CameraTimeSource    - extra time source for camera handling
    GuiTimeSource       - time source for user interface stuff

    The TimeManager offers a TimeEffect for animating the timefactor of all time sources
    AND of the graphicsthread to allow "Matrix"-like time effects.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file 
*/
#include "game/manager.h"
#include "basegamefeature/basegametiming/timesource.h"
#include "core/singleton.h"
#include "effects/animeventhandler/animeventtiminghandler.h"
#include "faudio/audiolistener.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class TimeManager : public Game::Manager
{
	__DeclareClass(TimeManager);
    __DeclareSingleton(TimeManager);

public:
    /// constructor
    TimeManager();
    /// destructor
    virtual ~TimeManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called after loading game state
    virtual void OnLoad();
    /// called before saving game state
    virtual void OnSave();
    /// on frame 
    virtual void OnFrame();
    /// reset all time sources
    void ResetAll();
    /// pause all time sources
    void PauseAll();
    /// continue all time sources
    void ContinueAll();

    /// set the time factor
    void SetTimeFactor(float timeFactor, bool resetTimeEffects = true);
    /// get the time factor
    float GetTimeFactor() const;

    /// render a debug visualization 
    virtual void OnRenderDebug();

    /// attach a time source
    void AttachTimeSource(const Ptr<TimeSource>& timeSource);
    /// remove a time source
    void RemoveTimeSource(const Ptr<TimeSource>& timeSource);
    /// get number of time source
    int GetNumTimeSources() const;
    /// get pointer to time source by index
    const Ptr<TimeSource>& GetTimeSourceByIndex(int index) const;
    /// get pointer to time source by class name
    Ptr<TimeSource> GetTimeSourceByClassName(const Util::String& n) const;

    /// start a time effect to animate time factor ("Matrix/MaxPayne" time effect)
    void StartTimeEffect(float timeFactor, Timing::Tick duration, Timing::Tick fadeIn, Timing::Tick fadeOut);
    /// stop current time effect
    void StopTimeEffect(bool immediate = false);
    /// is time effect active
    bool IsTimeEffectActive() const;

protected:
    
    /// update the time manager
    void Update();
    /// apply time effect
    void ApplyTimeEffect();

    EffectsFeature::AnimEventRegistry::TimingEvent activeTimeEffect;
    bool isTimeEffectActive;
    
    float timeFactor;
    float lastTimeFactor;
    Timing::Tick time;
    Timing::Tick frameTime;
    Timing::Tick lastRealTime;

    Util::Array<Ptr<TimeSource> > timeSourceArray;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
TimeManager::IsTimeEffectActive() const
{
    return this->isTimeEffectActive;
}

} // namespace BaseGameFeature

//------------------------------------------------------------------------------
