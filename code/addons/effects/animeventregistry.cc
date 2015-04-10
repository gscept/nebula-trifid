//------------------------------------------------------------------------------
//  animeventregistry.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animeventregistry.h"
#include "effectsattributes.h"
#include "db/reader.h"
#include "db/dbserver.h"
#include "animeventhandler/animeventtiminghandler.h"
#include "animeventhandler/animeventvibrationhandler.h"
#include "animeventhandler/animeventsoundhandler.h"
#include "animeventhandler/animeventshakehandler.h"
#include "animeventhandler/animeventattachmenthandler.h"
#include "animation/animeventserver.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AnimEventRegistry, 'AERE', Core::RefCounted);
__ImplementInterfaceSingleton(EffectsFeature::AnimEventRegistry);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
AnimEventRegistry::AnimEventRegistry() :
    isValid(false)
{
    __ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AnimEventRegistry::~AnimEventRegistry()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::Setup()
{
    n_assert(!this->IsValid());

    this->LoadCamShakeEvents(Util::StringAtom("_Template_AnimEventCamShake"));
    this->LoadSoundEvents(Util::StringAtom("_Template_AnimEventSound"));
    this->LoadTimingEvents(Util::StringAtom("_Template_AnimEventTiming"));
    this->LoadVibrationEvents(Util::StringAtom("_Template_AnimEventVibration"));
    this->LoadAttachmentEvents(Util::StringAtom("_Template_AnimEventEffects"));

    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::Discard()
{
    n_assert(this->IsValid());

    // TODO CLEANUP HERE
	if (this->timingHandler.isvalid())
	{
		Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->timingHandler);
		this->timingHandler = 0;
	}
	if (this->vibrationHandler.isvalid())
	{
		Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->vibrationHandler);
		this->vibrationHandler = 0;
	}
	if (this->soundHandler.isvalid())
	{
		Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->soundHandler);
		this->soundHandler = 0;
	}
	if (this->shakeHandler.isvalid())
	{
		Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->shakeHandler);
		this->shakeHandler = 0;
	}
	if (this->attachmentHandler.isvalid())
	{
		Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->attachmentHandler);
		this->attachmentHandler = 0;
	}
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventRegistry::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::LoadAttachmentEvents(const Util::StringAtom &tableName)
{
    // open database
    Ptr<Db::Reader> reader = Db::Reader::Create();
    reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    reader->SetTableName(tableName.Value());    
    if (!reader->Open())
    {
        n_error("AnimEventRegistry::LoadAttachmentEvents: failed to load table '%s' in static database", tableName.Value());        
    }

    int numRows = reader->GetNumRows();

	if (numRows)
	{
		// only add an eventhandler if we actually have events defined in the database
		Ptr<EffectsFeature::AnimEventAttachmentHandler> attach = EffectsFeature::AnimEventAttachmentHandler::Create();
		attach->SetCategoryName("attachment");
		this->attachmentHandler = attach.cast<Animation::AnimEventHandlerBase>();
		Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(this->attachmentHandler);
	}

    this->animEventAttachments.BeginBulkAdd();
    int index;
    for (index = 0; index < numRows; index++)
    {
        reader->SetToRow(index);

        Util::StringAtom id = reader->GetString(Attr::AnimEventName);
        Util::StringAtom res = reader->GetString(Attr::Resource);
        Util::StringAtom joint = reader->GetString(Attr::Joint);        
        Util::String rotation = reader->GetString(Attr::Rotation);
        Timing::Time time = reader->GetFloat(Attr::Duration);

        if(!res.IsValid() && !joint.IsValid() && 0 == time)
        {
            continue;
        }

        AttachmentEvent newEffect;
        newEffect.name = id;
        newEffect.resource = res;
        newEffect.jointName = joint;
        newEffect.duration = time;
        if ("joint" == rotation)
        {
            newEffect.rotation = GraphicsFeature::AttachmentManager::Joint;
        } 
        else if ("jointTranslate" == rotation)
        {
			newEffect.rotation = GraphicsFeature::AttachmentManager::JointTranslationOnly;
        }
        else if ("translate" == rotation)
        {
			newEffect.rotation = GraphicsFeature::AttachmentManager::TransformOnly;
        }
        else
        {
            n_error("AnimEventRegistry::LoadAttachmentEvents: table:'%s' row:'%i' column:'Rotation' unsupported value '%s'!\n", tableName.Value(), index, rotation.AsCharPtr());
        }
        this->animEventAttachments.Add(newEffect.name, newEffect);
    }
    this->animEventAttachments.EndBulkAdd();
    reader->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::LoadCamShakeEvents(const Util::StringAtom& tableName)
{
    // open database
    Ptr<Db::Reader> reader = Db::Reader::Create();
    reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    reader->SetTableName(tableName.Value());    
    if (!reader->Open())
    {
        n_error("AnimEventRegistry::LoadCamShakeEvents: failed to load table '%s' in static database", tableName.Value());        
    }

    int numRows = reader->GetNumRows();

	if (numRows)
	{
		// only add an eventhandler if we actually have events defined in the database
		Ptr<EffectsFeature::AnimEventShakeHandler> attach = EffectsFeature::AnimEventShakeHandler::Create();
		attach->SetCategoryName("shake");
		this->shakeHandler = attach.cast<Animation::AnimEventHandlerBase>();
		Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(this->shakeHandler);
	}

    this->animEventCamShakes.BeginBulkAdd();
    int index;
    for (index = 0; index < numRows; index++)
    {
        reader->SetToRow(index);

        Util::String id = reader->GetString(Attr::AnimEventName);
        Timing::Time duration = reader->GetFloat(Attr::Duration);
        Math::vector intensity(reader->GetFloat(Attr::IntensityX),
                               reader->GetFloat(Attr::IntensityY),
                               reader->GetFloat(Attr::IntensityZ));
        Math::vector rotation(reader->GetFloat(Attr::RotationX),
                              reader->GetFloat(Attr::RotationY),
                              reader->GetFloat(Attr::RotationZ));
        float range = reader->GetFloat(Attr::Range);

        if (0 == duration && 0 == intensity.x() && 0 == intensity.y() && 0 == intensity.z() && 0 == range &&
            0 == rotation.x() && 0 == rotation.y() && 0 == rotation.z())
        {
            continue;
        }

        ShakeEvent newOne;
        newOne.duration = duration;
        newOne.intensity = intensity;
        newOne.rotation = rotation;
        newOne.range = range;
        animEventCamShakes.Add(id, newOne);
    }
    animEventCamShakes.EndBulkAdd();
    reader->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::LoadSoundEvents(const Util::StringAtom& tableName)
{
    // open database
    Ptr<Db::Reader> reader = Db::Reader::Create();
    reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    reader->SetTableName(tableName.Value());    
    if (!reader->Open())
    {
        n_error("AnimEventRegistry::LoadSoundEvents: failed to load table '%s' in static database", tableName.Value());        
    }

    int numRows = reader->GetNumRows();
	
	if (numRows)
	{
		// only add an eventhandler if we actually have events defined in the database
		Ptr<EffectsFeature::AnimEventSoundHandler> attach = EffectsFeature::AnimEventSoundHandler::Create();
		attach->SetCategoryName("sound");
		this->soundHandler = attach.cast<Animation::AnimEventHandlerBase>();
		Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(this->soundHandler);
	}

    this->animEventSounds.BeginBulkAdd();
    int index;
    for (index = 0; index < numRows; index++)
    {
        reader->SetToRow(index);

        Util::String id = reader->GetString(Attr::AnimEventName);
        const Util::String eventName = reader->GetString(Attr::EventName); 		
		
        // check if string is not empty
        if (eventName.IsEmpty())
        {
            continue;
        }

        this->animEventSounds.Add(id, FAudio::EventId(eventName));
    }
    this->animEventSounds.EndBulkAdd();
    reader->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::LoadTimingEvents(const Util::StringAtom& tableName)
{
    // open database
    Ptr<Db::Reader> reader = Db::Reader::Create();
    reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    reader->SetTableName(tableName.Value());    
    if (!reader->Open())
    {
        n_error("AnimEventRegistry::LoadTimingEvents: failed to load table '%s' in static database", tableName.Value());        
    }

    int numRows = reader->GetNumRows();

	if (numRows)
	{
		// only add an eventhandler if we actually have events defined in the database
		Ptr<EffectsFeature::AnimEventTimingHandler> attach = EffectsFeature::AnimEventTimingHandler::Create();
		attach->SetCategoryName("timing");
		this->timingHandler = attach.cast<Animation::AnimEventHandlerBase>();
		Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(this->timingHandler);
	}

    this->animEventTimer.BeginBulkAdd();
    int index;
    for (index = 0; index < numRows; index++)
    {
        reader->SetToRow(index);

        Util::String id = reader->GetString(Attr::AnimEventName);
        float factor = reader->GetFloat(Attr::TimeFactor);
        Timing::Tick fadeIn = Timing::SecondsToTicks(reader->GetFloat(Attr::Attack));
        Timing::Tick duration = Timing::SecondsToTicks(reader->GetFloat(Attr::Sustain));
        Timing::Tick fadeOut = Timing::SecondsToTicks(reader->GetFloat(Attr::Release));

        if (1 == factor && 0 == duration && fadeIn == 0 && fadeOut == 0)
        {
            continue;
        }

        TimingEvent newOne;
        newOne.factor = factor;
        newOne.sustain = duration;
        newOne.release = fadeOut;
        newOne.attack = fadeIn;
        newOne.startTime = -1;
        animEventTimer.Add(id, newOne);
    }
    animEventTimer.EndBulkAdd();
    reader->Close();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimEventRegistry::LoadVibrationEvents(const Util::StringAtom& tableName)
{
    // open database
    Ptr<Db::Reader> reader = Db::Reader::Create();
    reader->SetDatabase(Db::DbServer::Instance()->GetStaticDatabase());
    reader->SetTableName(tableName.Value());    
    if (!reader->Open())
    {
        n_error("AnimEventRegistry::LoadVibrationEvents: failed to load table '%s' in static database", tableName.Value());        
    }

    int numRows = reader->GetNumRows();
    this->animEventVibs.BeginBulkAdd();
    int index;
    for (index = 0; index < numRows; index++)
    {
        reader->SetToRow(index);

        Util::String id = reader->GetString(Attr::AnimEventName);

        Timing::Time highDuration = reader->GetFloat(Attr::HighFreqDuration);
        float highIntensity = reader->GetFloat(Attr::HighFreqIntensity);

        Timing::Time lowDuration = reader->GetFloat(Attr::LowFreqDuration);
        float lowIntensity = reader->GetFloat(Attr::LowFreqIntensity);

        IndexT playerIndex = reader->GetInt(Attr::PlayerIndex);

        if (0 == lowIntensity && 0 == lowDuration && 0 == highDuration && 0 == highIntensity)
        {
            continue;
        }

        VibrationEvent newOne;
        newOne.highFreqDuration = highDuration;
        newOne.lowFreqDuration = lowDuration;
        newOne.highFreqIntensity = highIntensity;
        newOne.lowFreqIntensity = lowIntensity;
        newOne.playerIndex = playerIndex;

        this->animEventVibs.Add(id, newOne);        
    }
    this->animEventVibs.EndBulkAdd();
    reader->Close();
}
} // namespace FX