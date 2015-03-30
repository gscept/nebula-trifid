//------------------------------------------------------------------------------
//  sequenzermanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tracksequencer.h"

#include "track.h"
#include "trackbar.h"
#include "sequencermanager.h"
#include "framesync/framesynctimer.h"
#include "io/ioserver.h"
#include "math/transform44.h"

using namespace IO;

namespace Sequencer
{
__ImplementClass(Sequencer::TrackSequencer, 'TSEQ',Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
TrackSequencer::TrackSequencer():
		framesPerSecond(30),
		playLength(300),
		framePosition(0),
		isSetup(false),
		isCutscene(false),
		elapsedTime(0),
		startTime(0),
		currentState(STOPPED),
		stage(0)
{

}

//------------------------------------------------------------------------------
/**
*/
TrackSequencer::~TrackSequencer()
{

}

//------------------------------------------------------------------------------
/**
*/
void
TrackSequencer::Discard()
{
    int index;
    for(index = 0; index < trackArray.Size(); index++)
    {
        this->trackArray[index]->Discard();
    }

	this->trackArray.Clear();
    this->isSetup = false;
    this->currentState = STOPPED;
}

//------------------------------------------------------------------------------
/** This method should be called per frame. It handles the TrackBars
    according to the sequencer frame time position.
*/
void TrackSequencer::OnFrame()
{
    // play if the sequence is playing or when its a cutscene 
    // when standing on the last frame
    if (this->currentState == PLAYING)
    {
    //    this->runningTime += deltaTime;
		this->elapsedTime = FrameSync::FrameSyncTimer::Instance()->GetScaledTime() - this->startTime;
        this->CalcFramePosition();
        
        if (this->framePosition < this->playLength)
        {
            // play inside selection if repeat is on
            if (((	this->framePosition + 1.0f) >= this->selectionEnd)
				&&	this->repeat
                &&	(this->selectionEnd != this->selectionStart)
                &&	((this->numOfRepeats < 1) || (this->repeatCount != this->numOfRepeats)))
            {
                if (this->numOfRepeats > 0)
                {
                    this->repeatCount++;
                }
                float diff = this->framePosition - (float)this->selectionEnd;
                this->GotoPosition(((float)this->selectionStart) + diff);
            }
            else
            {
                // call all tracks
                int i;
                int numOfTracks = this->trackArray.Size();
                for (i=0; i<numOfTracks; i++)
                {
                    this->trackArray[i]->OnSequenceFrame(this->framePosition);
                }
            }
        }
        // play end is reached stop
        else if ((	this->repeat)
				&&	((this->numOfRepeats < 1) || (this->repeatCount != this->numOfRepeats)))
        {
            if (this->numOfRepeats > 0)
            {
                this->repeatCount++;
            }
            float diff = this->framePosition - (float)this->playLength;
            this->GotoPosition(diff);
        }
        else
        {
            this->currentState = STOPPED;
		}
    }
}

//------------------------------------------------------------------------------
/** This method creates a new track.
*/
void TrackSequencer::AddTrack(const Ptr<Track>& track)
{
    n_assert(!this->trackArray.Find(track));
    this->trackArray.Append( track );
}

//------------------------------------------------------------------------------
/** Get the current cinematic frame number
*/
float TrackSequencer::CalcFramePosition()
{
    this->framePosition = (float)this->elapsedTime * this->framesPerSecond;
    return this->framePosition;
}

//------------------------------------------------------------------------------
/** This method starts the sequencer play back.
*/
void TrackSequencer::Play()
{
    //recalculate start time realtive to current system time
	this->startTime = FrameSync::FrameSyncTimer::Instance()->GetScaledTime() - this->elapsedTime;
    this->currentState = PLAYING;
}

//------------------------------------------------------------------------------
/** This method pauses the sequencer.
*/
void TrackSequencer::Pause()
{
    this->currentState = PAUSED;
}

//------------------------------------------------------------------------------
/** Will stop the sequence. Unlikley to resetposition(0) it
    will not call a OnSequence frame.
*/
void TrackSequencer::Stop()
{
    //reset starttime and elapsed time
    this->startTime		= FrameSync::FrameSyncTimer::Instance()->GetScaledTime();
    this->elapsedTime	= 0;
    this->framePosition = 0;
    // reset repeat count
    this->repeatCount	= 1;

    this->currentState	= STOPPED;
}

//------------------------------------------------------------------------------
/** This method resets the sequencer to the start.
*/
void TrackSequencer::ResetPosition()
{
    this->GotoPosition(0);
}

//------------------------------------------------------------------------------
/**
*/
void TrackSequencer::EndWaitingSequence()
{
	Util::Array<Ptr<TrackBar> > trackBarArray = this->GetTrackBarsArray();
    //loop thorugh all trackbars look if they are properly executed
    IndexT i;
    for(i = 0; i < trackBarArray.Size(); i++)
    {
        if(false == trackBarArray[i]->HasExecuted())
        {
            trackBarArray[i]->OnExit();
        }
    }
}

//------------------------------------------------------------------------------
/** This method jumps to a frame.
*/
void TrackSequencer::GotoPosition(float frame)
{
    //calculate current time in the sequence
    this->elapsedTime = frame/(float)this->framesPerSecond;
    //calculate start position realtive to system time
	this->startTime = FrameSync::FrameSyncTimer::Instance()->GetScaledTime() - this->elapsedTime;
    this->framePosition = frame;
    // call all tracks
    int i;
    int numOfTracks = this->trackArray.Size();
    for (i=0; i<numOfTracks; i++)
    {
        this->trackArray[i]->OnSequenceFrame(this->framePosition);
    }
}

//------------------------------------------------------------------------------
/**
    Create sequence from a filename containing xml-form
*/
void
TrackSequencer::SetupFromFile(const Ptr<Graphics::Stage>& stage, const Util::String& fileName)
{
    Util::String file = "seq:" + fileName + ".xml";
    IO::URI uriPath = URI(file);
    n_assert(IoServer::Instance()->FileExists(uriPath));
        
    //build a stream
    Ptr<Stream> stream = IoServer::Instance()->CreateStream(uriPath); 
    
	Ptr<XmlReader> xmlReader = XmlReader::Create(); 
	xmlReader->SetStream(stream); 
    
	if(xmlReader->Open()) 
    {
        // make sure it's a Xml file
        if (xmlReader->GetCurrentNodeName() != "TrackEditor")
        {
            n_error("SequencerProperty::SetupFromFile): not a valid track sequencer file!");
        }
    }
    else
    {
        n_error("SequencerProperty::SetupFromFile(): could not open sequencer file");
    }
    
    //setup sequencer with the stream
    this->SetupFromXmlReader(stage, xmlReader);
    IndexT i;
    for(i = 0; i < this->trackArray.Size(); i++)
    {
        this->trackArray[i]->OnLoadAfter();
    }
    
    //Close XML
    if(xmlReader->IsOpen())
    {
    	xmlReader->Close();	
    }
}

//------------------------------------------------------------------------------
/**
    Create sequence form xml node (xml element)
*/
void
TrackSequencer::SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml)
{
    n_assert(!this->isSetup);
    
    //Set Stage
	this->stage = stage;
	
    // clear all tracks
    this->trackArray.Clear();

    // parse children
    n_assert(Util::String("TrackEditor") == xml->GetCurrentNodeName());
    if (xml->SetToFirstChild()) do
    {
        if (Util::String("GlobalParameter") == xml->GetCurrentNodeName())
        {
            this->ParseGlobalParameter(xml);
        }
        else if (Util::String("TrackList") == xml->GetCurrentNodeName())
        {
            if (xml->SetToFirstChild("Track")) do
            {
                Ptr<Track> track = Track::Create();
                track->SetTrackSequencer(this);
                track->SetupFromXmlReader(this->stage, xml);

                this->AddTrack(track);
            }
            while (xml->SetToNextChild("Track"));
        }
    }
    while (xml->SetToNextChild());
    // reset repeat count
    this->repeatCount = 1;
    this->isSetup = true;
    IndexT i;
    for(i = 0; i < this->trackArray.Size(); i++)
    {
        this->trackArray[i]->OnLoadAfter();
    }
}

//------------------------------------------------------------------------------
/**
    This method parses the global parameters of a sequence from an xml node.
*/
void
TrackSequencer::ParseGlobalParameter(const Ptr<IO::XmlReader>& xml)
{
    Math::transform44 transform;

    if (xml->HasAttr("playLength"))
    {
        playLength = xml->GetInt(Util::String("playLength"));
    }
    if (xml->HasAttr("playbackPosition"))
    {
        this->framePosition = (float)xml->GetInt(Util::String("playbackPosition"));
    }
    if (xml->HasAttr("framesPerSecond"))
    {
        framesPerSecond = xml->GetInt(Util::String("framesPerSecond"));
    }
    if (xml->HasAttr("selectionStart"))
    {
        selectionStart = xml->GetInt(Util::String("selectionStart"));
    }
    if (xml->HasAttr("isCutscene"))
    {
        isCutscene = xml->GetBool(Util::String("isCutscene"));
    }
    if (xml->HasAttr("selectionEnd"))
    {
        selectionEnd = xml->GetInt(Util::String("selectionEnd"));
    }
    if (xml->HasAttr("repeat"))
    {
        repeat = xml->GetBool(Util::String("repeat"));
    }
    if (xml->HasAttr("numOfRepeats"))
    {
        numOfRepeats = xml->GetInt(Util::String("numOfRepeats"));
    }
    if(xml->HasAttr("position"))
    {
        transform.setposition(xml->GetFloat4(Util::String("position")));
    }
    if(xml->HasAttr("rotation"))
    {
        transform.setrotatepivot(xml->GetFloat4(Util::String("rotation")));
    }
    if(xml->HasAttr("scale"))
    {
        transform.setscale(xml->GetFloat4(Util::String("scale")));
    }
    //if its a sequence +1 frame so onexit will be needed to gurantee 
    //all trackbars onexit will be called
    //a cutscene must be ended explicit (then all onexit will be called)
    if(!this->isCutscene)
    {
        this->playLength++;
    }

    this->SetTransformationMatrix(transform.getmatrix());
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<TrackBar> > 
TrackSequencer::GetTrackBarsArray()
{
    Util::Array<Ptr<TrackBar> > output;
    IndexT i;
    for(i = 0; i < this->trackArray.Size();i++)
    {
        IndexT u;
        for(u = 0; u < this->trackArray[i]->GetNumOfTrackBars(); u++)
        {
            output.Append(this->trackArray[i]->GetTrackBar(u));
        }
    }
    return output;
}

//------------------------------------------------------------------------------
/**
*/
void
TrackSequencer::Abort()
{
	Util::Array<Ptr<TrackBar> > trackBarArray = this->GetTrackBarsArray();
    //loop thorugh all trackbars look if they are properly executed
    int index;
    for(index = 0; index < trackBarArray.Size(); index++)
    {
        if(false == trackBarArray[index]->HasExecuted())
        {
            trackBarArray[index]->OnAbort();
        }
    }
    this->currentState = STOPPED;
}

}//End namespace Sequencer