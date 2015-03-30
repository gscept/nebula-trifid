#pragma once
//------------------------------------------------------------------------------
/**
    @class RakNet::RakNetPlayer
        
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "network/base/playerbase.h"          
#include "network/packetpriority.h"
#include "util/ringbuffer.h"
#include "framesync/framesynctimer.h"

//------------------------------------------------------------------------------
namespace RakNet
{
class RakNetMultiplayerServer;

class RakNetPlayer : public Base::PlayerBase
{
    __DeclareClass(RakNetPlayer);
public:
    /// constructor
    RakNetPlayer();
    /// destructor
    virtual ~RakNetPlayer();

    /// set sign in state, updates out unique id on change
    void SetSignInState(Base::PlayerBase::SignInState val);
    /// get Started	
    bool GetStarted() const;
    /// set Started
    void SetStarted(const bool val);                                               
    /// on join session 
    virtual void OnJoinSession(Multiplayer::Session* sessionPtr);

 #if NEBULA3_ENABLE_PROFILING     
    class StatisticValue
    {
    public:
        // constructor
        StatisticValue();
        // constrcutor with description
        StatisticValue(const Util::String& desc);
        // add value with timestamp
        void Add(int value);
        // values as array
        Util::Array<int> ValuesAsArray() const;
        // timestamps as array
        Util::Array<Timing::Tick> TimeStampsAsArray() const;
        // get description
        const Util::String& GetDescription() const;
    private:
        friend class RakNetPlayer;
        Util::String description;
        Util::RingBuffer<Util::KeyValuePair<Timing::Tick, int> > values;
    };        
    Util::Dictionary<Util::StringAtom, StatisticValue> debugCounterSet;

    /// get counter set to fill by RakNetMultiplayerServer
    Util::Dictionary<Util::StringAtom, StatisticValue>& GetDebugCounterSet();
#endif   

private:  

    friend class RakNetMultiplayerServer;
    bool started;
};

#if NEBULA3_ENABLE_PROFILING  
//------------------------------------------------------------------------------
/**
*/
inline 
RakNetPlayer::StatisticValue::StatisticValue()
{
    values.SetCapacity(50);
}     
//------------------------------------------------------------------------------
/**
*/
inline
RakNetPlayer::StatisticValue::StatisticValue(const Util::String& desc)
{
    this->description = desc;
    values.SetCapacity(50);
}
//------------------------------------------------------------------------------
/**
*/
inline const Util::String& 
RakNetPlayer::StatisticValue::GetDescription() const
{
    return this->description;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RakNetPlayer::StatisticValue::Add(int value)
{
    Timing::Tick curTime = FrameSync::FrameSyncTimer::Instance()->GetTicks();
    Util::KeyValuePair<Timing::Tick, int> newEntry(curTime, value);
    this->values.Add(newEntry);
}    

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<int> 
RakNetPlayer::StatisticValue::ValuesAsArray() const
{
    Util::Array<int> intValues;
    IndexT i;
    for (i = 0; i < this->values.Size(); ++i)
    {
        intValues.Append(this->values[i].Value());    	
    }
    return intValues;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Timing::Tick> 
RakNetPlayer::StatisticValue::TimeStampsAsArray() const
{       
    Util::Array<Timing::Tick> timeStamps;
    IndexT i;
    for (i = 0; i < this->values.Size(); ++i)
    {
        timeStamps.Append(this->values[i].Key());    	
    }
    return timeStamps;

}      

//------------------------------------------------------------------------------
/**
*/
inline Util::Dictionary<Util::StringAtom, RakNetPlayer::StatisticValue>& 
RakNetPlayer::GetDebugCounterSet()
{
    return this->debugCounterSet;
} 
#endif

//------------------------------------------------------------------------------
/**
*/
inline void 
RakNetPlayer::SetSignInState(Base::PlayerBase::SignInState val)
{
    this->signInState = val;
}
//------------------------------------------------------------------------------
/**
*/
inline bool 
RakNetPlayer::GetStarted() const
{
    return this->started;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void RakNetPlayer::SetStarted(const bool val)
{
    this->started = val;
}  
} // namespace RakNet
//------------------------------------------------------------------------------
