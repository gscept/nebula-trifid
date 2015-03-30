#pragma once
//------------------------------------------------------------------------------
/**
    @class Sequencer::TrackSequencer
    
    (C) 2008 Radon Labs GmbH
*/

#include "foundation/core/refcounted.h"
#include "foundation/timing/time.h"
#include "sequencer/tracksequencer.h"

namespace Sequencer
{
class TrackBar;

class Track : public Core::RefCounted
{
    __DeclareClass(Track);

public:
    /// constructor
    Track();
    /// destructor
    virtual ~Track();
    
	/// call per-frame
	void OnSequenceFrame(Timing::Time frameTime);
	/// called every frame, when the sequencer is not playing
    virtual void OnIdle(Timing::Time localFrameTime);
    /// called every frame after OnIdle(), when the sequencer is not playing
    virtual void OnIdleAfter(Timing::Time localFrameTime);
    /// called by sequencer after all tracks are loaded
    virtual void OnLoadAfter();

    /// called when deattached from the track sequencer
    void Discard();
    /// get muted status
    bool IsMuted() const;
	/// get track bar element at position
    Ptr<TrackBar> GetTrackBar(int index) const;
    /// get number of track bars
    int GetNumOfTrackBars() const;
    /// add a track bar
    void AddTrackBar(const Ptr<TrackBar>& trackBar);
	/// set the track sequencer in which this track is located in
    void SetTrackSequencer(const Ptr<TrackSequencer>& sequencer );
    /// get the track sequencer in which this track is located in
    Ptr<TrackSequencer> GetTrackSequencer() const;
    /// get trackbar array
    Util::Array<Ptr<TrackBar> > GetTrackBarArray();
    /// parse xml element and set attributes
    void SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml);
    /// get track id
    int GetId();
    /// get the tracks children
    Util::Array<Util::String> GetChildrens() const;
    
    /// get graphics stage pointer
    const Ptr<Graphics::Stage>& GetStage() const; 

private:

	Ptr<Graphics::Stage> stage;
    Util::Array<Ptr<TrackBar> > trackBarArray;
	Util::Array<Ptr<TrackBar> > activeTrackBars;
    Ptr<TrackBar> inactivTrackBarTransition;
    Ptr<TrackBar> activeTrackBar;
    Ptr<TrackSequencer> trackSequencer;
    bool muted;
    bool isSetup;
    int id;
    Util::Array<Util::String> childrens;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Track::IsMuted() const
{
    return this->muted;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
Track::GetId()
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
Util::Array<Util::String> 
Track::GetChildrens() const
{
    return this->childrens;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Ptr<Graphics::Stage>&
Track::GetStage() const
{
    return this->stage;
}

}//End Sequencer Namespace