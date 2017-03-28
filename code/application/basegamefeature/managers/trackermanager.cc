//------------------------------------------------------------------------------
//  trackermanager.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/trackermanager.h"
#include "basegametiming/gametimesource.h"
#include "io/ioserver.h"
#include "basegamefeature/basegameprotocol.h"

using namespace Util;

namespace Tracking
{
__ImplementClass(Tracking::TrackerManager, 'TRMA', Game::Manager);
__ImplementSingleton(Tracking::TrackerManager);


TrackerManager::TrackerManager() :nextSample(0), mode(IDLE), sampleStart(0)
{
    // hacky default so that we can add replay objects before loading
    this->targets.SetSize(100);
    this->attrSlots.SetSize(100);
    __ConstructSingleton
}

TrackerManager::~TrackerManager()
{
    __DestructSingleton
}

void TrackerManager::OnBeginFrame()
{
    Timing::Tick tick = BaseGameFeature::GameTimeSource::Instance()->GetTicks() - this->sampleStart;
    switch (this->mode)
    {
    case RECORD:
    {
        this->writer->WriteFormatted("%d\n", tick);
        for (int i = 0; i < this->attributes.Size(); i++)
        {
            const KeyValuePair<Ptr<Game::Entity>, Attr::AttrId> val = this->attributes[i];
            this->writer->WriteLine(val.Key()->GetAttr(val.Value()).ValueAsString());
        }
    }
    break;
    case PLAYBACK:        
        while(this->replay[this->nextSample].tick < tick)
        {
            for (int i = 0; i < this->targets.Size(); i++)
            {
                const Util::Array<Util::KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>> & entries = this->targets[i];
                for (int j = 0; j < entries.Size(); j++)
                {
                    Attr::Attribute at(entries[j].Value());
                    at.SetValueFromString(this->replay[this->nextSample].vals[i]);
                    entries[j].Key()->SetAttr(at);
                }
            }
            this->nextSample = (this->nextSample + 1) % this->replay.Size();
            if (this->nextSample == 0)
            {
                this->sampleStart = BaseGameFeature::GameTimeSource::Instance()->GetTicks();
                this->SetMode(IDLE);
                break;
            }
        }        
        break;
    default:
        break;
    }    
}

void TrackerManager::AddTracking(const Ptr<Game::Entity> & entity, const Attr::AttrId& id)
{
    this->attributes.Append(KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>(entity, id));    
}

void TrackerManager::SetFile(const IO::URI& stream)
{
    this->uri = stream;
}

void TrackerManager::OnActivate()
{
    Manager::OnActivate();
}

void TrackerManager::OnDeactivate()
{
    Manager::OnDeactivate();   
    if (this->writer.isvalid() && this->writer->IsOpen())
    {
        this->writer->Close();
    }
}

void TrackerManager::SetMode(TrackingMode mode)
{
    if (this->callback)
    {
        this->callback(mode);
    }
    if (this->mode == RECORD)
    {
        this->writer->Close();
    }    
    if (mode == IDLE)
    {
        Ptr<BaseGameFeature::SetTrackingMode> msg = BaseGameFeature::SetTrackingMode::Create();
        msg->SetMode(Tracking::IDLE);
        if (this->mode == RECORD)
        {
            for (int i = 0; i < this->attributes.Size(); i++)
            {
                __SendSync(this->attributes[i].Key(), msg);
            }
        }
        else if (this->mode == PLAYBACK)
        {
            for (int i = 0; i < this->targets.Size(); i++)
            {
                const Util::Array<Util::KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>> & entries = this->targets[i];
                for (int j = 0; j < entries.Size(); j++)
                {
                    __SendSync(entries[j].Key(), msg);
                }
            }
        }
        this->mode = mode;
        return;
    }
    this->mode = mode;
    n_assert(this->uri.IsValid());
    this->stream = IO::IoServer::Instance()->CreateStream(this->uri);
    this->sampleStart = BaseGameFeature::GameTimeSource::Instance()->GetTicks();

    if(this->mode == RECORD)
    {
        this->writer = IO::TextWriter::Create();
        this->writer->SetStream(this->stream);
        this->writer->Open();
        this->writer->WriteFormatted("%d\n", this->attributes.Size());
        for (int i = 0; i < this->attributes.Size(); i++)
        {
            const KeyValuePair<Ptr<Game::Entity>, Attr::AttrId> val = this->attributes[i];
            this->writer->WriteFormatted("%s:%d:%s\n", val.Key()->GetCategory().AsCharPtr(), val.Key()->GetUniqueId(), val.Value().GetName().AsCharPtr());
        }
        Ptr<BaseGameFeature::SetTrackingMode> msg = BaseGameFeature::SetTrackingMode::Create();
        msg->SetMode(Tracking::RECORD);
        for (int i = 0; i < this->attributes.Size(); i++)
        {
            __SendSync(this->attributes[i].Key(), msg);
        }
    }
    else
    {
        this->nextSample = 0;
        this->reader = IO::TextReader::Create();
        this->reader->SetStream(this->stream);
        this->reader->Open();
        this->LoadTrackingData();
        this->reader->Close();
        Ptr<BaseGameFeature::SetTrackingMode> msg = BaseGameFeature::SetTrackingMode::Create();
        msg->SetMode(Tracking::PLAYBACK);
        for (int i = 0; i < this->targets.Size(); i++)
        {
            const Util::Array<Util::KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>> & entries = this->targets[i];
            for (int j = 0; j < entries.Size(); j++)
            {
                __SendSync(entries[j].Key(), msg);
            }
        }
    }
    
}

void TrackerManager::LoadTrackingData()
{
    this->replay.Clear();
    Util::String line = this->reader->ReadLine();
    int attrs = line.AsInt();    
    n_assert(attrs < 100);    
    for (int i = 0; i < attrs; i++)
    {
        line = this->reader->ReadLine();
        Array<String> toks = line.Tokenize(":");
        this->attrSlots[i] = Attr::AttrId(toks[2]);
    }
    while (!this->reader->Eof())
    {
        TrackingEntry e;
        line = this->reader->ReadLine();
        e.tick = line.AsInt();
        for (int i = 0; i < attrs; i++)
        {            
            e.vals.Append(this->reader->ReadLine());
        }
        this->replay.Append(e);
    }
}

void TrackerManager::AddReplay(unsigned int spot, const Ptr<Game::Entity>& entity, const Attr::AttrId& id)
{
    this->targets[spot].Append(KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>(entity, id));
}

}