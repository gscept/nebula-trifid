#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::FxServer

    The FxServer manages standalone visual effects which are created and 
    can remove themselves after they have played. The system works 
    similar to the audio subsystem, but its a part of the renderthread.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/singleton.h"
#include "fx/rt/effects/effect.h"
#include "fx/rt/mixer/effectmixer.h"
#include "fx/rt/animeventregistry.h"
#include "math/vector.h"
#include "timing/time.h"
#include "vibration/vibrationplayer.h"


//------------------------------------------------------------------------------
namespace FX
{
class FxServer : public Core::RefCounted
{
	__DeclareClass(FxServer);
    __DeclareSingleton(FxServer);
public:
    /// constructor
    FxServer();
    /// destructor
    virtual ~FxServer();

    /// open the fx server
    virtual bool Open();
    /// close the fx server
    virtual void Close();

    /// get the current time
    Timing::Time GetTime() const;

    /// attach a generic effect
    virtual void AttachEffect(const Ptr<Effect>& effect);
    /// remove an effect
    virtual void RemoveEffect(const Ptr<Effect>& effect);
    /// get number of currently active effects
    int GetNumActiveEffects() const;
    /// get active effect at index
    const Ptr<Effect>& GetActiveEffectAt(int index) const;

    /// attach a generic effect mixer
    virtual void AttachEffectMixer(const Ptr<EffectMixer>& mixer);
    /// remove a mixer
    virtual void RemoveEffectMixer(const Ptr<EffectMixer>& mixer);
    
    /// call before rendering happens
    virtual void OnFrame(const Timing::Time& time);

    /// flush all effects
    void FlushAll();

private:
    bool isOpen;
    Timing::Time curTime;
	Ptr<AnimEventRegistry> animEventRegistry;
    Util::Array<Ptr<Effect> > activeEffects;
    Util::Array<Ptr<EffectMixer> > activeEffectMixer;

    Ptr<Vibration::VibrationPlayer> vibrationPlayer;
};

//------------------------------------------------------------------------------
/**
*/
inline
Timing::Time
FxServer::GetTime() const
{
    return this->curTime;
}

//------------------------------------------------------------------------------
/**
    Returns the number of currently active effects.
*/
inline
int
FxServer::GetNumActiveEffects() const
{
    return this->activeEffects.Size();
}

//------------------------------------------------------------------------------
/**
    Returns pointer to active effect at index. Note that the number of
    active effects may have change after Trigger() has been called.
*/
inline
const Ptr<Effect>&
FxServer::GetActiveEffectAt(int index) const
{
    return this->activeEffects[index];
}

}; // namespace FX
    