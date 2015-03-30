#pragma once
//------------------------------------------------------------------------------
/**
    @class Sequencer::TrackSequencer
    
    (C) 2008 Radon Labs GmbH
*/

#include "core/refcounted.h"
#include "timing/time.h"
#include "math/matrix44.h"
#include "util/string.h"
#include "io/xmlreader.h"
#include "graphics/stage.h"

namespace Sequencer
{
class Track;
class TrackBar;

class TrackSequencer : public Core::RefCounted
{
    __DeclareClass(TrackSequencer);

public:
	/// the sequencer states
	enum TrackSequencerState
	{
	    PLAYING,
	    PAUSED,
	    STOPPED
	};

	/// constructor
	TrackSequencer();
	/// destructor
	virtual ~TrackSequencer();
	/// play sequencer
	void Play();
	/// pause sequencer
	void Pause();
	/// stop the sequencer
	void Stop();
	/// abort sequencer
	void Abort();
	/// reset sequencer to start position
	void ResetPosition();
	/// must be used to manually end a
	/// sequence(cutscene) which waits on the last frame 
	void EndWaitingSequence();
	/// goto cinematic frame position
	void GotoPosition(float frame);
	/// get all trackbars handled by this track sequencer
	Util::Array<Ptr<TrackBar> > GetTrackBarsArray();
	/// get the track array
	Util::Array<Ptr<Track> > GetTrackArray() const;
	/// is sequence playing
	bool IsPlaying();
	/// get is setup
	bool IsSetup() const;
    /// is a sequence a cutscene
    bool IsACutscene() const;
    /// is a repeating sequence
    bool IsRepeat() const;
    
	/// get the transformation matrix
	Math::matrix44 GetTransformationMatrix() const;
	/// get the transformation matrix
	void SetTransformationMatrix(const Math::matrix44& value);
	
	/// get the current frame position
    float GetFramePosition() const;
    /// get frames per second
    int GetFramesPerSecond() const;
    
    /// call per-frame from track sequencer manager
    void OnFrame();
    /// called when becomes deactive
    void Discard();
    /// setup sequencer using a filename
    void SetupFromFile(const Ptr<Graphics::Stage>& stage, const Util::String& fileName);
    /// setup sequencer using a xml-reader
    void SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml);
    
    /// get graphics stage pointer
    const Ptr<Graphics::Stage>& GetStage() const;
    
    /// Get selection start
    int GetSelectionStart() const;
    /// Set selection end
    void SetSelectionStart(int selectionStart);
    
    /// get selection end
    int GetSelectionEnd() const;
    /// set selection end
    void SetSelectionEnd(int selectionEnd);
	
private:
	/// calculates the current frame position
	float CalcFramePosition();
	/// add a new track
	void AddTrack(const Ptr<Track>& track);
    /// Parse the global parameters of a sequence
    void ParseGlobalParameter(const Ptr<IO::XmlReader>& xml);

	Ptr<Graphics::Stage> stage;
	Timing::Time startTime;
	Timing::Time elapsedTime;
	TrackSequencerState currentState;
	int selectionStart;
	int selectionEnd;
	int numOfRepeats;
	int repeatCount;
	bool repeat;
	bool isSetup;
	bool isCutscene;
	float framePosition;
	int framesPerSecond;
	int playLength;
	Util::Array<Ptr<Track> > trackArray;
	Math::matrix44 matrix;
};

//------------------------------------------------------------------------------
/**
*/
inline
Util::Array<Ptr<Sequencer::Track> > 
TrackSequencer::GetTrackArray() const
{
    return this->trackArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
TrackSequencer::IsPlaying()
{
    return (this->currentState == PLAYING);
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::matrix44 
TrackSequencer::GetTransformationMatrix() const
{
    return this->matrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
TrackSequencer::SetTransformationMatrix(const Math::matrix44& value)
{
    this->matrix = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
TrackSequencer::IsACutscene() const
{
    return this->isCutscene;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
TrackSequencer::IsSetup() const
{
    return this->isSetup;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
TrackSequencer::GetFramePosition() const
{
    return this->framePosition;
}

//------------------------------------------------------------------------------
/**
*/
inline 
int
TrackSequencer::GetFramesPerSecond() const
{
    return this->framesPerSecond;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Ptr<Graphics::Stage>&
TrackSequencer::GetStage() const
{
    return this->stage;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
TrackSequencer::IsRepeat() const
{
    return this->repeat;
}

//------------------------------------------------------------------------------
/**
    get selection end
*/
inline
int
TrackSequencer::GetSelectionEnd() const
{
    return this->selectionEnd;
}

//------------------------------------------------------------------------------
/**
    set selection end
*/
inline
void
TrackSequencer::SetSelectionEnd(int selectionEnd)
{
    this->selectionEnd = selectionEnd;
}

//------------------------------------------------------------------------------
/**
    Get selection start
*/
inline
int
TrackSequencer::GetSelectionStart() const
{
    return this->selectionStart;
}

//------------------------------------------------------------------------------
/**
    Set selection end
*/
inline
void
TrackSequencer::SetSelectionStart(int selectionStart)
{
    this->selectionStart = selectionStart;
}

}//End Sequencer Namespace