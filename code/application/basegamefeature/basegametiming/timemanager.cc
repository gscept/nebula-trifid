//------------------------------------------------------------------------------
//  timing/timemanager.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "basegametiming/timemanager.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "db/writer.h"
#include "math/scalar.h"
#include "timing/time.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "render/debugrender/debugrender.h" 
#include "basegamefeature/basegametiming/gametimesource.h"

#ifdef _DEBUG
#include "io/console.h"
#endif

namespace BaseGameFeature
{
__ImplementClass(TimeManager, 'TIMA', Game::Manager);
__ImplementSingleton(TimeManager);

using namespace Timing;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
TimeManager::TimeManager() :
    isTimeEffectActive(false),
    timeFactor(1.0), 
    time(0),                                 
    frameTime(0),                              
    lastRealTime(0)  
{                                              
    __ConstructSingleton;                     
}

//------------------------------------------------------------------------------
/**
*/
TimeManager::~TimeManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Activate the time manager. This will create all the standard time
    sources for Mangalore.
*/
void
TimeManager::OnActivate()
{
    // get current global time
    this->time = 0;
    // this should be the framesync time, this however results in a cyclic setup dependency so we just go with 
    // 0 instead. 
    //this->time = FrameSync::FrameSyncTimer::Instance()->GetTicks();
    this->frameTime = 0;

    // create standard time sources...
    Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
    Deactivate the time manager. 
*/
void
TimeManager::OnDeactivate()
{
    // cleanup time sources...
    while (this->timeSourceArray.Size() > 0)
    {
        this->RemoveTimeSource(this->timeSourceArray[0]);
    }        
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Attach a time source to the time manager. This will invoke
    OnActivate() on the time source.
*/
void
TimeManager::AttachTimeSource(const Ptr<TimeSource>& timeSource)
{
    n_assert(timeSource);
    n_assert(!this->timeSourceArray.Find(timeSource));
    timeSource->OnActivate();
    this->timeSourceArray.Append(timeSource);
}

//------------------------------------------------------------------------------
/**
    Remove a time source from the time manager. This will invoke OnDeactivate()
    on the time source.
*/
void
TimeManager::RemoveTimeSource(const Ptr<TimeSource>& timeSource)
{
    n_assert(timeSource);
    IndexT index = this->timeSourceArray.FindIndex(timeSource);
    n_assert(InvalidIndex != index);
    this->timeSourceArray[index]->OnDeactivate();
    this->timeSourceArray.EraseIndex(index);
}

//------------------------------------------------------------------------------
/**
    Returns number of time sources attached to the time manager.
*/
int
TimeManager::GetNumTimeSources() const
{
    return this->timeSourceArray.Size();
}

//------------------------------------------------------------------------------
/**
    Gets pointer to time source object by index.
*/
const Ptr<TimeSource>& 
TimeManager::GetTimeSourceByIndex(int index) const
{
    return this->timeSourceArray[index];
}

//------------------------------------------------------------------------------
/**
    Get pointer to time source object by class name, returns 0 if not found.
*/
Ptr<TimeSource> 
TimeManager::GetTimeSourceByClassName(const Util::String& n) const
{
    n_assert(n.IsValid());
    IndexT i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        if (this->timeSourceArray[i]->GetClassName() == n)
        {
            return this->timeSourceArray[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Checks whether the TimeSources table exists in the database, if
    yes invokes OnLoad() on all time sources...
*/
void
TimeManager::OnLoad()
{
    const Ptr<Db::Database>& db = Db::DbServer::Instance()->GetGameDatabase();
    if (db->HasTable("TimeSources"))
    {
        // create a db reader...
        Ptr<Db::Reader> dbReader = Db::Reader::Create();
        dbReader->SetDatabase(db);
        dbReader->SetTableName("TimeSources");
        if (dbReader->Open())
        {   
            int rowIndex;
            int numRows = dbReader->GetNumRows();
            for (rowIndex = 0; rowIndex < numRows; rowIndex++)
            {
                dbReader->SetToRow(rowIndex);
                TimeSource* timeSource = this->GetTimeSourceByClassName(dbReader->GetString(Attr::TimeSourceId));
                if (timeSource)
                {
                    timeSource->OnLoad(dbReader);
                }
            }
            dbReader->Close();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Ask all time sources to save their status to the database.
*/
void
TimeManager::OnSave()
{
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbWriter->SetTableName("TimeSources");
    dbWriter->AddColumn(Db::Column(Attr::TimeSourceId, Db::Column::Primary));
    dbWriter->AddColumn(Db::Column(Attr::TimeSourceTime));
    dbWriter->AddColumn(Db::Column(Attr::TimeSourceFactor));
    
    if (dbWriter->Open())
    {
        IndexT i;
        for (i = 0; i < this->timeSourceArray.Size(); i++)
        {
            this->timeSourceArray[i]->OnSave(dbWriter);
        }
        dbWriter->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Reset all time sources. This is usually called at the beginning
    of an application state.
*/
void
TimeManager::ResetAll()
{
    IndexT i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    Pause all time sources. NOTE: there's an independent pause counter inside each
    time source, a pause just increments the counter, a continue decrements
    it, when the pause counter is != 0 it means, pause is activated.
*/
void
TimeManager::PauseAll()
{
    IndexT i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Pause();
    }
}

//------------------------------------------------------------------------------
/**
    Unpause all time sources.
*/
void
TimeManager::ContinueAll()
{
    IndexT i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Continue();
    }
}

//------------------------------------------------------------------------------
/**
Update our time sources
*/
void
TimeManager::OnFrame()
{
    this->Update();
}

//------------------------------------------------------------------------------
/**
    Update all time sources. This method is called very early in the frame
    by the current application state handler. This will get the current frame time
    and call UpdateTime() on all attached time sources.

    FIXME:
    * properly handle time exceptions!!!
*/
void
TimeManager::Update()
{
    // if time effect is active apply
    if (this->IsTimeEffectActive())
    {
        this->ApplyTimeEffect();
    }

    // compute the current frame time
    Tick curTime = FrameSync::FrameSyncTimer::Instance()->GetTicks();
    this->frameTime = curTime - this->lastRealTime;           
    this->time = this->time + this->frameTime;
    
    // apply time factor
    //this->frameTime = (Timing::Tick)((float)this->frameTime * this->GetTimeFactor());
    
    // clamp frame time
    /*if (this->frameTime > 250)
    {
        this->frameTime = 250;
        this->time = curTime - this->frameTime;
    }*/

    // update all time sources
    Timing::Time frameTimeInSecs = Timing::TicksToSeconds(this->frameTime);
    IndexT i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->SetFactor(this->GetTimeFactor());
        this->timeSourceArray[i]->UpdateTime(frameTimeInSecs, curTime);
    }
    this->lastRealTime = curTime;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeManager::SetTimeFactor(float f, bool resetTimeEffects)
{
    if (resetTimeEffects && this->IsTimeEffectActive())
    {
        this->StopTimeEffect(true);
    }
    n_assert(f > 0);
    this->timeFactor = f;
                                    
    // update time factor on gfx thread
    Ptr<Graphics::SetTimeFactor> msg = Graphics::SetTimeFactor::Create();
    msg->SetFactor(this->timeFactor);
    __StaticSend(Graphics::GraphicsInterface, msg);    
}

//------------------------------------------------------------------------------
/**
*/
float
TimeManager::GetTimeFactor() const
{
    return this->timeFactor;
}

//------------------------------------------------------------------------------
/**
    show time factor if not 1.0
*/
void
TimeManager::OnRenderDebug()
{
    #ifdef _DEBUG
    //if (this->GetTimeFactor() != 1.0)
    {
        Util::String txt = "mainThread - TimeFactor : ";
        txt.Append(Util::String::FromFloat(this->GetTimeFactor()));
        _debug_text(txt, Math::float2(0.03f, 0.1f), Math::float4(1,0,1,1));
        //IO::Console::Instance()->Print("Time Factor is %i%%.", this->GetTimeFactor()); 
    }
    #endif
    
    // call parrent
    Game::Manager::OnRenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void 
TimeManager::StartTimeEffect(float timeFactor, Tick duration, Tick fadeIn, Tick fadeOut)
{
    n_assert(!this->IsTimeEffectActive());
    
    EffectsFeature::AnimEventRegistry::TimingEvent event;
    event.attack = fadeIn;
    event.factor = timeFactor;
    event.release = fadeOut;
    event.startTime = this->lastRealTime;
    event.sustain = duration;
    this->activeTimeEffect = event;
    this->isTimeEffectActive = true;
    this->lastTimeFactor = this->timeFactor;
}

//------------------------------------------------------------------------------
/**
*/
void 
TimeManager::StopTimeEffect(bool immediate)
{
    n_assert(this->IsTimeEffectActive());

    if (immediate)
    {
        // set factor here
        this->SetTimeFactor(1.0, false);
        this->lastTimeFactor = this->timeFactor;

        // update time factor on gfx thread
        Ptr<Graphics::SetTimeFactor> msg = Graphics::SetTimeFactor::Create();
        msg->SetFactor(this->timeFactor);
        __StaticSend(Graphics::GraphicsInterface, msg);

        // clear effect
        this->isTimeEffectActive = false;
    }
    else
    {
        this->activeTimeEffect.attack = 500;
        this->activeTimeEffect.factor = 1.0f;
        this->activeTimeEffect.release = 0;
        this->activeTimeEffect.sustain = 0;
        this->activeTimeEffect.startTime = this->lastRealTime;
        this->isTimeEffectActive = true;
        this->lastTimeFactor = this->timeFactor;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
TimeManager::ApplyTimeEffect()
{
    // first set time        
    Timing::Tick absoluteSustainTime = this->activeTimeEffect.startTime + this->activeTimeEffect.attack;
    Timing::Tick absoluteRelaseTime = absoluteSustainTime + this->activeTimeEffect.sustain;
    Timing::Tick absoluteEndTime = absoluteRelaseTime + this->activeTimeEffect.release;

    // ATTACK
    if (this->lastRealTime < absoluteSustainTime)
    {
        float value = (float)(this->lastRealTime - this->activeTimeEffect.startTime) / (float)(this->activeTimeEffect.attack);
        this->timeFactor = Math::n_lerp(this->lastTimeFactor, this->activeTimeEffect.factor, value);
    } 
    // SUSTAIN
    else if (this->lastRealTime < absoluteRelaseTime || this->activeTimeEffect.sustain == -1)
    {   
        this->timeFactor = this->activeTimeEffect.factor;
        this->lastTimeFactor = this->timeFactor;
    }
    // RELEASE
    else if (this->lastRealTime < absoluteEndTime)
    {
        float value = (float)(this->lastRealTime - absoluteRelaseTime) / (float)(this->activeTimeEffect.release);
        this->timeFactor = Math::n_lerp(this->activeTimeEffect.factor, this->lastTimeFactor, value);
    }
    // FINISHED
    else
    {
        this->timeFactor = 1.0f;
        this->lastTimeFactor = this->timeFactor;
        this->isTimeEffectActive = false;
    }

    // update time factor on gfx thread
    Ptr<Graphics::SetTimeFactor> msg = Graphics::SetTimeFactor::Create();
    msg->SetFactor(this->timeFactor);
    __StaticSend(Graphics::GraphicsInterface, msg);    
}
}; // namespace Game
