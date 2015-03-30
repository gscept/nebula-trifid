#pragma once
//------------------------------------------------------------------------------
/**
    @class Sequencer::TrackSequencer
    
    (C) 2008 Radon Labs GmbH
*/

#include "core/refcounted.h"
#include "io/xmlreader.h"
#include "track.h"
#include "math/matrix44.h"

namespace Sequencer
{
class TrackBar : public Core::RefCounted
{
    __DeclareClass(TrackBar);

public:
    /// constructor
    TrackBar();
    /// destructor
    virtual ~TrackBar();
	/// called by sequencer when end frame is reached
    virtual void OnExit();
    /// called by sequencer when start frame is reached
    virtual void OnEnter();
    /// called by sequencer per frame if track bar is in time line
    virtual void OnSequenceFrame(Timing::Time localFrameTime);
    /// called every frame, when the sequencer is not playing
    virtual void OnIdle(Timing::Time localFrameTime);
    /// called every frame after OnIdle(), when the sequencer is not playing
    virtual void OnIdleAfter(Timing::Time localFrameTime);
    /// called by sequencer after all tracks are loaded
    virtual void OnLoadAfter();
    /// called when deattached from its track. Will unload all subsystems.
    virtual void Discard();
    /// called by seqeuencer when the sequence is aborted
    virtual void OnAbort();
	/// parse xml element and set attributes
    virtual void SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml);
    
    /// set start frame
    void SetStartFrame(int frame);
    /// get start frame
    int GetStartFrame() const;
    
    /// set end frame
    void SetEndFrame(int frame);
    /// get end frame
    int GetEndFrame() const;
    
	/// set playing state
    void SetIsPlaying(bool isPlaying);
    /// get playing state
    bool GetIsPlaying() const;
    
    /// set mute state
    void SetMuted(bool mute);
    /// get mute state
    bool GetMuted() const;
    
    /// get type
    const Util::String& GetType();
    /// get name
    const Util::String& GetName() const;
    
    /// get executing status
    bool HasExecuted() const;
    /// factory class for creating derrived track bars, returns null if its a unknown trackbar.
    static Ptr<TrackBar> CreateFromXmlReader(const Ptr<IO::XmlReader>& xml);
    /// set the track in which this trackbar is located in
    void SetTrack(const Ptr<Sequencer::Track>& value);
    
    /// get graphics stage pointer
    const Ptr<Graphics::Stage>& GetStage() const;

protected:
    /// called after created and all parameters are set to initialize
    virtual void Load();
    
    Ptr<Graphics::Stage> stage;
	int startFrame;
    int endFrame;
    Ptr<Track> track;
    bool isMuted;
    bool isPlaying;
    bool isLoaded;
    bool isSetup;
    bool hasExecuted;
    Math::matrix44 parentTransform;
    Util::String type;
    Util::String name;
};

//------------------------------------------------------------------------------
/**
*/
inline 
bool
TrackBar::GetMuted() const
{
    return this->isMuted;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
TrackBar::SetStartFrame(int frame)
{
    this->startFrame = frame;
}

//------------------------------------------------------------------------------
/**
*/
inline 
int
TrackBar::GetStartFrame() const
{
    return this->startFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
TrackBar::SetEndFrame(int frame)
{
    this->endFrame = frame;
}

//------------------------------------------------------------------------------
/**
*/
inline 
int
TrackBar::GetEndFrame() const
{
    return this->endFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
TrackBar::SetIsPlaying(bool isPlaying)
{
    this->isPlaying = isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
TrackBar::GetIsPlaying() const
{
    return this->isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
TrackBar::HasExecuted() const
{
    return this->hasExecuted;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Util::String&
TrackBar::GetType()
{
    return type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String& 
TrackBar::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Ptr<Graphics::Stage>&
TrackBar::GetStage() const
{
    return this->stage;
}

}//End Sequencer Namespace