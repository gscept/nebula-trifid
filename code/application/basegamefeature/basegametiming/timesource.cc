//------------------------------------------------------------------------------
//  timesource.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "basegametiming/timesource.h"


namespace BaseGameFeature
{
__ImplementClass(TimeSource, 'TTSC', Core::RefCounted);

using namespace Timing;
//------------------------------------------------------------------------------
/**
*/
TimeSource::TimeSource() :
    frameTime(0.001f),       
    time(0.0),               
    ticks(0),                
    pauseCounter(0),         
    timeFactor(1.0f)         
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TimeSource::~TimeSource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnActivate()
{
    // empty
    this->time = 0.0;
    this->pauseCounter = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnDeactivate()
{
    // empty, override in subclass as needed
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnLoad(const Ptr<Db::Reader>& dbReader)
{
    n_assert(dbReader.isvalid());
    this->time         = dbReader->GetFloat(Attr::TimeSourceTime);
    this->timeFactor   = dbReader->GetFloat(Attr::TimeSourceFactor);
    this->pauseCounter = 0;
    this->frameTime    = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnSave(const Ptr<Db::Writer>& dbWriter)
{
    n_assert(dbWriter.isvalid());
    dbWriter->BeginRow();
    dbWriter->SetString(Attr::TimeSourceId, this->GetClassName());
    dbWriter->SetFloat(Attr::TimeSourceTime, float(this->time));
    dbWriter->SetFloat(Attr::TimeSourceFactor, this->timeFactor);
    dbWriter->EndRow();
}

//------------------------------------------------------------------------------
/**
    This method is called when the internal time should be updated. It will
    be called early in the frame by the TimeManager. Think of this method
    as the time source's OnFrame() method.
*/
void
TimeSource::UpdateTime(Time frameTime, Timing::Tick t)
{
    this->frameTime = frameTime;

    if (!this->IsPaused())
    {
        this->time += frameTime * this->timeFactor;
        this->ticks = t;
    }
}

//------------------------------------------------------------------------------
/**
*/
uint
TimeSource::GetFrameId() const
{
    return 0;//TODO: Graphics::Server::Instance()->GetFrameId();
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::Reset()
{
    this->time = 0.0;
    this->pauseCounter = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::Pause()
{
    this->pauseCounter++;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::Continue()
{
    if (this->pauseCounter > 0)
    {
        this->pauseCounter--;
    }
}


} // namespace Timing