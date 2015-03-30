//------------------------------------------------------------------------------
//  sequenzermanager.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "trackbar.h"

#include "core/ptr.h"
#include "core/factory.h"

namespace Sequencer
{
__ImplementClass(Sequencer::TrackBar, 'STBA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
TrackBar::TrackBar():
		startFrame(0),
		endFrame(0),
		isMuted(false),
		isPlaying(false),
		isSetup(false),
		isLoaded(false),
		hasExecuted(false),
		name(""),
		stage(0)
{

}

//------------------------------------------------------------------------------
/**
*/
TrackBar::~TrackBar()
{

}

//------------------------------------------------------------------------------
/** This method is called when deattached from its track.
    Will unload all subsystems.
*/
void 
TrackBar::Discard()
{
    this->track = 0;
	this->isSetup = false;
    //this->isLoad = false;
}

//------------------------------------------------------------------------------
/** This method is called when deattached from its track.
    Will unload all subsystems.
*/
void 
TrackBar::Load()
{
    n_assert(!this->isLoaded)
    this->isLoaded = true;
    // the root transform is only affected by the sequencers total position
    this->parentTransform = this->track->GetTrackSequencer()->GetTransformationMatrix();

}

//------------------------------------------------------------------------------
/** This method is called when the play position reaches the
    beginning of the track bar.
*/
void
TrackBar::OnEnter()
{

}

//------------------------------------------------------------------------------
/** This method is called when the play position reaches the
    end of the track bar.
*/
void
TrackBar::OnExit()
{
    this->hasExecuted = true;
}

//------------------------------------------------------------------------------
/**
*/
void
TrackBar::OnAbort()
{
    this->hasExecuted = true;
}

//------------------------------------------------------------------------------
/** This method should be called per frame. It handles the TrackBars
    according to the sequencer frame time position.
*/
void
TrackBar::OnSequenceFrame(Timing::Time localFrameTime)
{
	//empty
}

//------------------------------------------------------------------------------
/** called every frame, when the sequencer is not playing
*/
void
TrackBar::OnIdle(Timing::Time localFrameTime)
{
	//empty
}

//------------------------------------------------------------------------------
/** called every frame after OnIdle(), when the sequencer is not playing
*/
void
TrackBar::OnIdleAfter(Timing::Time localFrameTime)
{
	//empty
}

//------------------------------------------------------------------------------
/**
*/
void
TrackBar::OnLoadAfter()
{
	//empty
}

//------------------------------------------------------------------------------
/**
    parse xml element and set attributes common for all track bars
*/
void 
TrackBar::SetupFromXmlReader(const Ptr<Graphics::Stage>& stage, const Ptr<IO::XmlReader>& xml)
{
	//Set Stage
	this->stage = stage;
	
    n_assert(!this->isSetup);
    this->isSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<TrackBar> 
TrackBar::CreateFromXmlReader(const Ptr<IO::XmlReader>& xml)
{
    Util::String valStr = "Sequencer::" + xml->GetString("type");
    n_assert(!valStr.IsEmpty());
    //check if its a project specified trackbar
    if(false == Core::Factory::Instance()->ClassExists(valStr))
    {
        return 0;
    }
    Ptr<TrackBar> trackbar = (TrackBar*)Core::Factory::Instance()->Create(valStr);
    n_assert(trackbar->IsA(TrackBar::RTTI));
    
    trackbar->type = valStr;
    // read the standard attributes for all trackbars
    if (xml->HasAttr("startFrame"))
    {
        trackbar->startFrame = xml->GetInt(Util::String("startFrame"));
    }

    if (xml->HasAttr("endFrame"))
    {
        trackbar->endFrame = xml->GetInt(Util::String("endFrame"));
    }

    if (xml->HasAttr("name"))
    {   
        trackbar->name = xml->GetString(Util::String("name"));
    }

    if (xml->HasAttr("mute"))
    {   
        trackbar->isMuted = xml->GetBool(Util::String("mute"));
    }
    
    return trackbar;
}

//------------------------------------------------------------------------------
/**
*/
void 
TrackBar::SetTrack(const Ptr<Sequencer::Track>& value)
{
    n_assert(value.isvalid());
    this->track = value;
}

}//End namespace Sequencer