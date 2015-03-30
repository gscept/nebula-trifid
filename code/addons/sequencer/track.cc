//------------------------------------------------------------------------------
//  sequenzermanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "track.h"

#include "trackbar.h"

namespace Sequencer
{
__ImplementClass(Sequencer::Track, 'SETR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Track::Track():
		muted(false),
		isSetup(false),
	    id(-1),
		stage(0)
{
	//empty
}

//------------------------------------------------------------------------------
/**
*/
Track::~Track()
{

}

//------------------------------------------------------------------------------
/**
    This method should be called per frame. It handles the TrackBars
    according to the sequencer frame time position.
    Frame time is number of the cinematic frame.
    Make sure that exit is called befor enter.
*/
void
Track::OnSequenceFrame(Timing::Time frameTime)
{
    //skip if the track is muted
    if (true == this->muted)
    {
        return;
    }

    int i;
    int numOfBars = this->trackBarArray.Size();
    this->activeTrackBars.Clear();
    this->inactivTrackBarTransition = 0;
    this->activeTrackBar = 0;

    for (i=0; i<numOfBars; i++)
    {
        Ptr<TrackBar> trackBar = this->trackBarArray[i];
        if (!trackBar->GetMuted())
        {
            if ((trackBar->GetStartFrame() <= frameTime)
                && (trackBar->GetEndFrame() >= frameTime))
            {
                //add to active list
                this->activeTrackBars.Append(trackBar);
            }
            else
            {
                if(trackBar->GetIsPlaying())
                {
                    this->inactivTrackBarTransition = trackBar;
                }
            }
        }
    }

    //there can only be 2 active trackbars per track
    n_assert(this->activeTrackBars.Size() <= 2);

    // if there are trackbars at our position concatenated
    if(this->activeTrackBars.Size() == 2)
    {
        //from a left concatenated trackbar between 2 trackbars
        if(this->activeTrackBars[0]->GetIsPlaying())
        {
            this->activeTrackBars[0]->OnExit();
            this->activeTrackBars[0]->SetIsPlaying(false);
            this->activeTrackBars[1]->OnEnter();
            this->activeTrackBars[1]->SetIsPlaying(true);
            activeTrackBar = this->activeTrackBars[1];
        }
        //from empty space between 2 trackbars
        else if(!this->activeTrackBars[0]->GetIsPlaying() && 
                !this->activeTrackBars[1]->GetIsPlaying() &&
                !this->inactivTrackBarTransition.isvalid())
        {
            this->activeTrackBars[1]->OnEnter();
            this->activeTrackBars[1]->SetIsPlaying(true);
            activeTrackBar = this->activeTrackBars[1];
        }
        //from right concatenated trackbar between 2 trackbars
        else
        {
            this->activeTrackBar = this->activeTrackBars[1];
        }
    }
    // if there are trackbars at our position concatenated
    else if(this->activeTrackBars.Size() == 1)
    {
        //from a trackbar to another
        if(!this->activeTrackBars[0]->GetIsPlaying() && this->inactivTrackBarTransition.isvalid())
        {
            this->inactivTrackBarTransition->OnExit();
            this->inactivTrackBarTransition->SetIsPlaying(false);
            this->activeTrackBars[0]->OnEnter();
            this->activeTrackBars[0]->SetIsPlaying(true);

            activeTrackBar = this->activeTrackBars[0];
        }
        //from empty space to a trackbar
        else if(!this->activeTrackBars[0]->GetIsPlaying() && !this->inactivTrackBarTransition.isvalid())
        {
            this->activeTrackBars[0]->OnEnter();
            this->activeTrackBars[0]->SetIsPlaying(true);
            this->activeTrackBar = this->activeTrackBars[0];
        }
        //moving on the same trackbar
        else
        {
            this->activeTrackBar = this->activeTrackBars[0];
        }
    }
    //from trackbar to empty space
    else if(this->activeTrackBars.Size() == 0 && this->inactivTrackBarTransition.isvalid())
    {
        this->inactivTrackBarTransition->OnExit();
        this->inactivTrackBarTransition->SetIsPlaying(false);
    }

    //call a sequence on frame on the current active trackbar
    if(this->activeTrackBar.isvalid())
    {
        this->activeTrackBar->OnSequenceFrame(frameTime);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Track::Discard()
{
    int index;
    for(index = 0; index < this->trackBarArray.Size(); index ++)
    {
        this->trackBarArray[index]->Discard();
    }
    this->trackBarArray.Clear();
    
    this->isSetup = false;
}

//------------------------------------------------------------------------------
/**
    called every frame, when the sequencer is not playing
*/
void
Track::OnIdle(Timing::Time frameTime)
{
    int i;
    int numOfBars = this->trackBarArray.Size();
    for (i=0; i<numOfBars; i++)
    {
        Ptr<TrackBar> trackBar = this->trackBarArray[i];
        if (!trackBar->GetMuted())
        {
            if ((trackBar->GetStartFrame() <= frameTime)
                && (trackBar->GetEndFrame() >= frameTime))
            {
                trackBar->OnIdle(frameTime);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    called every frame after OnIdle(), when the sequencer is not playing
*/
void
Track::OnIdleAfter(Timing::Time frameTime)
{
    int i;
    int numOfBars = this->trackBarArray.Size();
    for (i=0; i<numOfBars; i++)
    {
        Ptr<TrackBar> trackBar = this->trackBarArray[i];
        if (!trackBar->GetMuted())
        {
            if ((trackBar->GetStartFrame() <= frameTime)
                && (trackBar->GetEndFrame() >= frameTime))
            {
                trackBar->OnIdleAfter(frameTime);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Ptr<TrackBar>
Track::GetTrackBar(int index) const
{
    return trackBarArray[index];
}

//------------------------------------------------------------------------------
/**
*/
int
Track::GetNumOfTrackBars() const
{
     return trackBarArray.Size();
}

//------------------------------------------------------------------------------
/** This method adds a track bar to the track.
*/
void
Track::AddTrackBar(const Ptr<TrackBar>& trackBar)
{
    n_assert(!this->trackBarArray.Find(trackBar));
    this->trackBarArray.Append( trackBar );
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Sequencer::TrackSequencer>
Track::GetTrackSequencer() const
{
    return this->trackSequencer;
}

//------------------------------------------------------------------------------
/**
*/
void
Track::SetTrackSequencer(const Ptr<Sequencer::TrackSequencer>& sequencer)
{
    n_assert(sequencer.isvalid());
    this->trackSequencer = sequencer;
}

//------------------------------------------------------------------------------
/**
  
*/
Util::Array<Ptr<TrackBar> > 
Track::GetTrackBarArray()
{
    return this->trackBarArray;
}

//------------------------------------------------------------------------------
/**
*/
void
Track::OnLoadAfter()
{
    IndexT i;
    for (i = 0; i < this->trackBarArray.Size(); i++)
    {
        this->trackBarArray[i]->OnLoadAfter();
    }
}

//------------------------------------------------------------------------------
/**
    parse xml element, set attributes and create track bars
*/
void
Track::SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml)
{
    n_assert(!this->isSetup);
    n_assert(xml->IsOpen())
    
	//Set Stage
	this->stage = stage;
	
    // read track attributes
    if (xml->HasAttr("id"))
    {
        this->id = xml->GetInt(Util::String("id"));
    }
    if (xml->HasAttr("mute"))
    {
        this->muted = xml->GetBool(Util::String("mute"));
    }
    if (xml->HasAttr("childs"))
    {
        Util::String childrenString = xml->GetString("childs");
        if(true == childrenString.IsValid())
        {
            this->childrens = childrenString.Tokenize(";");
        }
    }
    
    //a contract: a track must have a valid id
    n_assert2(this->id >= 0, "Id not valid.");    

    // create track bar elements
    // parse children
    if (xml->SetToFirstChild("TrackBar")) do
    {
        //Create initiliaze, with the needed start states, and setup a new track bar
        Ptr<TrackBar> trackbar = TrackBar::CreateFromXmlReader(xml);
        //check if the track has been created(will not be created if it was a unknown trackbar type)
        if(false == trackbar.isvalid())
        {
            continue;
        }

        trackbar->SetTrack(this);
        trackbar->SetupFromXmlReader(this->stage, xml);
        n_assert(trackbar.isvalid());
        this->AddTrackBar(trackbar);
    }
    while (xml->SetToNextChild("TrackBar"));
    this->isSetup = true;
}

}//End namespace Sequencer