//------------------------------------------------------------------------------
//  audiodevice.cc
//  (C) 2014-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audiodevice.h"
#include "faudio/fmoderror.h"
#include "faudio/eventinstance.h"
#include "faudio/busid.h"
#include "faudio/eventid.h"
#include "io/assignregistry.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "io/stream.h"
#include "fmod_studio.hpp"

using namespace Math;
using namespace FAudio;

__ImplementClass(FAudio::AudioDevice, 'AUDF', Core::RefCounted);
__ImplementSingleton(FAudio::AudioDevice);

const int AudioDevice::MAX_CHANNELS = 64;

#define FMOD_VERBOSE_DEBUG (0)

//------------------------------------------------------------------------------
/**
*/
FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type, void *commanddata1, void *commanddata2, void * userdata)
{
    switch (type)
    {
		case FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED:
            n_error("FMOD ERROR : FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED occured: %s.\n %d bytes.\n",commanddata1, (int)commanddata2);
            break;
		case FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION:
			{
				FMOD::DSP *source = (FMOD::DSP *)commanddata1;
				FMOD::DSP *dest = (FMOD::DSP *)commanddata2;

				Util::String msg;
				char name1[256];
				name1[0] = 0;
				char name2[256];
				name2[0] = 0;
				if (source)
				{
					source->getInfo(name1, 0, 0, 0, 0);
				}
				if (dest)
				{
					dest->getInfo(name2, 0, 0, 0, 0);
				}
				msg.Format("FMOD ERROR : FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION occured\nSOURCE = %s\nDEST = %s\n", name1, name2);
				n_error(msg.AsCharPtr());
			}
            break;
		case FMOD_SYSTEM_CALLBACK_ERROR:
			{
				if (FMOD_VERBOSE_DEBUG)
				{
					FMOD_ERRORCALLBACK_INFO* errorInfo = (FMOD_ERRORCALLBACK_INFO*)commanddata1;
					// FIXME: fmod spams these errors when using the studio api
					// http://www.fmod.org/questions/question/forum-40457
					// will try to filter these out
					if (errorInfo->instancetype != FMOD_ERRORCALLBACK_INSTANCETYPE_DSP || errorInfo->instancetype != FMOD_ERRORCALLBACK_INSTANCETYPE_CHANNEL)
					{
						Util::String msg;
						msg.Format("FMOD ERROR : FMOD_SYSTEM_CALLBACK_ERROR occured.\n %s(%s)[%d]\n", errorInfo->functionname, errorInfo->functionparams, errorInfo->result);
						n_warning(msg.AsCharPtr());
					}
				}
			}
            break;
        default:
            break;
    }
	
    return FMOD_OK;
}

namespace FAudio
{

//------------------------------------------------------------------------------
/**
*/
AudioDevice::AudioDevice() :    
    system(NULL),	
	lowlevelSystem(NULL),
    masterGroup(NULL),    
	auditioningEnabled(false),
	hasSoundDevice(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AudioDevice::~AudioDevice()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
AudioDevice::Open()
{
    n_assert(!this->IsOpen());    
    n_assert(!this->system);
	n_assert(!this->lowlevelSystem);
	n_assert(!this->masterGroup);

	this->listener = AudioListener::Create();
	this->audiointerface = AudioInterface::Create();
        
    FMOD_RESULT result = FMOD::Studio::System::create(&this->system);
    FMOD_CHECK_ERROR(result);

	result = this->system->getLowLevelSystem(&this->lowlevelSystem);
	FMOD_CHECK_ERROR(result);

	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_ERROR);
    
    // check fmod version
    unsigned int version;
	result = this->lowlevelSystem->getVersion(&version);
    FMOD_CHECK_ERROR(result);
    if (version < FMOD_VERSION)
    {
        n_error("You are using an old version of FMOD %08x.  This program requires %08x\nFILE: %s\nLine:%d\n", version, FMOD_VERSION, __FILE__, __LINE__);
    }

    int numdrivers;
    result = this->lowlevelSystem->getNumDrivers(&numdrivers);
    FMOD_CHECK_ERROR(result);
    if (!numdrivers)
    {
        n_warning("No Sound-Device found\nFILE: %s\nLine:%d\n", __FILE__, __LINE__);
		this->hasSoundDevice = false;
		return false;
    }
	else
	{
		this->hasSoundDevice = true;
	}
	
	result = this->lowlevelSystem->setCallback(systemcallback, FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED | FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION | FMOD_SYSTEM_CALLBACK_ERROR);
	FMOD_CHECK_ERROR(result);

    char driverName[256] = {'\0'};
	FMOD_SPEAKERMODE speakermode;
	int speakerchannels;
	result = this->lowlevelSystem->getDriverInfo(0, driverName, 256, 0, 0, &speakermode, &speakerchannels);
    FMOD_CHECK_ERROR(result); 
    driverName[255] ='\0';
    FMOD_COREAUDIO_VERBOSE("audio device %s\n", driverName);

	this->lowlevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);

	void * pExtraDriverData = NULL;

#if NEBULA3_FMOD_ENABLE_PROFILING
	result = this->system->initialize(AudioDevice::MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_ENABLE_PROFILE | FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData);    
#else
    result = this->system->initialize(AudioDevice::MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData);
#endif
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)        
    { 
        // Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... 
		this->lowlevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);        
        FMOD_CHECK_ERROR(result); 
        
        // reinit
		result = this->system->initialize(AudioDevice::MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, pExtraDriverData);        
        FMOD_CHECK_ERROR(result); 
    }
    else
    {
        FMOD_CHECK_ERROR(result);
    }

	// load fmod plugins in the binary folder
	Util::Array<Util::String> fmod_plugins = IO::IoServer::Instance()->ListFiles("bin:", "fmod_*", true);
	for (int i = 0; i < fmod_plugins.Size(); i++)
	{
		this->lowlevelSystem->loadPlugin(fmod_plugins[i].AsCharPtr(), NULL);
	}

	result = this->lowlevelSystem->getMasterChannelGroup(&this->masterGroup);
    FMOD_CHECK_ERROR(result);

	// load master bank files
	if (IO::IoServer::Instance()->FileExists("audio:Master Bank.bank") && IO::IoServer::Instance()->FileExists("audio:Master Bank.strings.bank"))
	{
		this->LoadBank("Master Bank");
		this->LoadBank("Master Bank.strings");
	}
  
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::Close()
{
    n_assert(this->IsOpen());
    n_assert(this->system);

    this->DiscardAllEvents();
    n_assert(!this->events.Size());
	this->UnloadAll();
  
    // reset callback
    FMOD_RESULT result = this->system->setCallback(NULL);
    FMOD_CHECK_ERROR(result);

    // close and release fmod
    result = this->system->release();
    FMOD_CHECK_ERROR(result);
    this->lowlevelSystem = 0;
    this->system = 0;
    this->masterGroup = 0;
	this->listener = 0;
	this->audiointerface = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::OnFrame()
{    
    n_assert(this->system);

	if (!this->hasSoundDevice) return;

    this->listener->OnFrame();

	FMOD_RESULT result = this->system->update();
    FMOD_CHECK_ERROR(result);
	
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::LoadBank(const Util::String & bankFile, bool nonblocking)
{
	if (!this->hasSoundDevice) return;

	if (!this->BankLoaded(bankFile))
	{
		// open must be called before loading an event project
		n_assert(this->system);
		n_assert(bankFile.IsValid());
		Util::String path = "audio:";
		path += bankFile;
		path += ".bank";
		IO::URI resPath = IO::AssignRegistry::Instance()->ResolveAssigns(path);
		n_assert(IO::IoServer::Instance()->FileExists(resPath));

		FMOD::Studio::Bank * bank;
		FMOD_RESULT result = this->system->loadBankFile(resPath.LocalPath().AsCharPtr(), nonblocking ? FMOD_STUDIO_LOAD_BANK_NONBLOCKING : FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
		FMOD_CHECK_ERROR_EXT(result, "failed to load \"audio:%s\"", bankFile.AsCharPtr());

		this->banks.Add(bankFile, bank);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::UnloadBank(const Util::String & bankFile)
{    
	if (!this->hasSoundDevice) return;

	n_assert(bankFile.IsValid());
	n_assert(this->banks.Contains(bankFile));

	FMOD::Studio::Bank * bank = this->banks[bankFile];
	this->banks.Erase(bankFile);
	FMOD_STUDIO_LOADING_STATE state;
	FMOD_RESULT result;
	if (bank->getSampleLoadingState(&state) == FMOD_OK)
	{
		if (state == FMOD_STUDIO_LOADING_STATE_LOADED || state == FMOD_STUDIO_LOADING_STATE_LOADING)
		{
			result = bank->unloadSampleData();
			FMOD_CHECK_ERROR(result);
		}
	}
	if (bank->getLoadingState(&state) == FMOD_OK)
	{
		if (state == FMOD_STUDIO_LOADING_STATE_LOADED || state == FMOD_STUDIO_LOADING_STATE_LOADING)
		{
			result = bank->unload();
			FMOD_CHECK_ERROR(result);
		}
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::UnloadAll()
{	
	if (!this->hasSoundDevice) return;

	FMOD_RESULT result = this->system->unloadAll();
	FMOD_CHECK_ERROR(result);
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String> 
AudioDevice::FindBankFiles()
{	
	Util::Array<Util::String> rawbanks = IO::IoServer::Instance()->ListFiles(IO::URI("audio:"), "*.bank");
	Util::Array<Util::String> realBanks;
	for (int i = 0; i < rawbanks.Size(); i++)
	{
		Util::String basename = rawbanks[i].ExtractFileName();
		basename.StripFileExtension();
		realBanks.Append(basename);
	}
	return realBanks;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<FAudio::EventInstance> 
AudioDevice::CreateEvent(const FAudio::EventId &eventId)
{    
	n_assert(this->system);
    n_assert(eventId.IsValid());

	Ptr<EventInstance> newevent = EventInstance::Create();
	newevent->Setup(eventId);
	this->events.Add(newevent, newevent);
	return newevent;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::DiscardEvent(const Ptr<FAudio::EventInstance> &wrapper)
{    	
	n_assert(this->system);
    n_assert(wrapper.isvalid());

    IndexT index = this->events.FindIndex(wrapper);
    // nothing to do, if it was already removed, might happen if the client
    // calls UnloadProject and after that he discards the audioemitter
    if(index == InvalidIndex) return;

    wrapper->Discard();
    this->events.EraseAtIndex(index);
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::DiscardAllEvents()
{  
    int index;
    for(index = 0; index < this->events.Size(); ++index)
    {
        this->events.ValueAtIndex(index)->Discard();
    }
    this->events.Clear();
}


//------------------------------------------------------------------------------
/**
*/
FMOD::Studio::Bus* 
AudioDevice::GetBus(const FAudio::BusId & busId)
{
	if (!this->hasSoundDevice) return 0;

    n_assert(this->system);
	n_assert(busId.IsValid());

	FMOD::Studio::Bus* bus;
	FMOD_RESULT result = this->system->getBus(busId.GetBus().AsCharPtr(), &bus);
	FMOD_CHECK_ERROR_EXT(result, "failed to get bus [bus: \"%s\"]", busId.GetBus().AsCharPtr());
    return bus;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::SetListener(const Math::matrix44& transform, const Math::vector& velocity)
{    
	if (!this->hasSoundDevice) return;

    this->listener->Set(transform, velocity);
}



#define Type2String(type) case type: return ((#type)+13); break;


//------------------------------------------------------------------------------
/**
*/
void 
AudioDevice::DumpMemoryInfo(const char *where) const
{

	// FIXME
#if 0
	ASSERT_COREAUDIO_THREAD;
    unsigned int memoryused;
    unsigned int memoryused_array[FMOD_MEMTYPE_MAX];
    this->eventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &memoryused, memoryused_array);
    n_printf("\n\n===================================================\n");
    n_printf("FMOD Memory Usage at %s:\n", where);
    n_printf("  used overall: %9d bytes  %7.3f MB\n", memoryused, (float)memoryused / (1024.0f*1024.0f));
    int i;
    for(i = 0; i < FMOD_MEMTYPE_MAX; ++i)
    {
        n_printf("    [%-25s]     %9d bytes  %7.3f MB\n", FmodMemtypeToString((FMOD_MEMTYPE)i), memoryused_array[i], (float)memoryused_array[i] / (1024.0f*1024.0f));
    }
    n_printf("\n\n===================================================\n");
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
AudioDevice::EventPlayFireAndForget(const FAudio::EventId &eventId, float volume)
{
	if (!this->hasSoundDevice) return;

	n_assert(this->system);

	FMOD::Studio::EventDescription* eventDesc = NULL;

	FMOD_RESULT result = this->system->getEvent(eventId.GetEventPath().AsCharPtr(), &eventDesc);
	FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event description",	eventId.GetEventPath().AsCharPtr());

	FMOD_STUDIO_LOADING_STATE state;
	result = eventDesc->getSampleLoadingState(&state);
	FMOD_CHECK_ERROR(result);
	if (state == FMOD_STUDIO_LOADING_STATE_UNLOADED || state == FMOD_STUDIO_LOADING_STATE_UNLOADING)
	{
		result = eventDesc->loadSampleData();
		FMOD_CHECK_ERROR(result);
	}


	FMOD::Studio::EventInstance * eventInst = NULL;
	result = eventDesc->createInstance(&eventInst);
	FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event instance", eventId.GetEventPath().AsCharPtr());
	
	bool is3d;
	eventDesc->is3D(&is3d);
	if (is3d)
	{
		// we have a 3d event but no transform, play it at listener position
		FMOD_3D_ATTRIBUTES attrs;		
#if (FMOD_VERSION >= 0x00010600)
		this->system->getListenerAttributes(0, &attrs);
#else
		this->system->getListenerAttributes(&attrs);
#endif
		eventInst->set3DAttributes(&attrs);
	}

	result = eventInst->setVolume(volume);
	FMOD_CHECK_ERROR(result);

	result = eventInst->start();
	FMOD_CHECK_ERROR(result);

	result = eventInst->release();
	FMOD_CHECK_ERROR(result);

    FMOD_COREAUDIO_VERBOSE("[project: event: \"%s\"  instance: 0x%p] EventPlayFireAndForget()",                            
                            eventId.GetEventPath().AsCharPtr(),
                            event);
}


//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::EventPlayFireAndForget3D(const FAudio::EventId &eventId, const matrix44 &transform, const Math::vector &velocity, float volume)
{
	this->EventPlayFireAndForget3D(eventId, transform.get_position(), transform.get_zaxis(), transform.get_yaxis(), velocity, volume);
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::EventPlayFireAndForget3D(const FAudio::EventId &eventId, const Math::point &position, const Math::vector & forward, const Math::vector &up, const Math::vector &velocity, float volume)
{
	if (!this->hasSoundDevice) return;

	n_assert(this->system);
	FMOD_3D_ATTRIBUTES newattrs;

	newattrs.position.x = position.x();
	newattrs.position.y = position.y();
	newattrs.position.z = position.z();
	newattrs.forward.x = forward.x();
	newattrs.forward.y = forward.y();
	newattrs.forward.z = forward.z();
	newattrs.up.x = up.x();
	newattrs.up.y = up.y();
	newattrs.up.z = up.z();
	newattrs.velocity.x = velocity.x();
	newattrs.velocity.y = velocity.y();
	newattrs.velocity.z = velocity.z();

	FMOD::Studio::EventDescription* eventDesc = NULL;

	FMOD_RESULT result = this->system->getEvent(eventId.GetEventPath().AsCharPtr(), &eventDesc);
	FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event description", eventId.GetEventPath().AsCharPtr());

	FMOD_STUDIO_LOADING_STATE state;
	result = eventDesc->getSampleLoadingState(&state);
	FMOD_CHECK_ERROR(result);
	if (state == FMOD_STUDIO_LOADING_STATE_UNLOADED || state == FMOD_STUDIO_LOADING_STATE_UNLOADING)
	{
		result = eventDesc->loadSampleData();
		FMOD_CHECK_ERROR(result);
	}


	FMOD::Studio::EventInstance * eventInst = NULL;
	result = eventDesc->createInstance(&eventInst);
	FMOD_CHECK_ERROR_EXT(result, "[event: \"%s\"] failed to get event instance", eventId.GetEventPath().AsCharPtr());

	result = eventInst->set3DAttributes(&newattrs);
	FMOD_CHECK_ERROR(result);

	result = eventInst->setVolume(volume);
	FMOD_CHECK_ERROR(result);

	result = eventInst->start();
	FMOD_CHECK_ERROR(result);

	result = eventInst->release();
	FMOD_CHECK_ERROR(result);

	FMOD_COREAUDIO_VERBOSE("[project: event: \"%s\"  instance: 0x%p] EventPlayFireAndForget3D()",
		eventId.GetEventPath().AsCharPtr(),
		event);
}

//------------------------------------------------------------------------------
/**
*/
Util::Dictionary<Util::String, bool>
AudioDevice::ParseAutoload(const IO::URI & path)
{
	Util::Dictionary<Util::String, bool> autoloader;
	if (IO::IoServer::Instance()->FileExists(path))
	{
		Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
		stream->SetAccessMode(IO::Stream::ReadAccess);
		Ptr<IO::XmlReader> reader = IO::XmlReader::Create();
		{
			reader->SetStream(stream);
			if (reader->Open())
			{
				if (reader->HasNode("/_AudioBanks"))
				{
					reader->SetToFirstChild();
					{
						do
						{
							Util::String filename = reader->GetString("Id");
							bool autoload = reader->GetOptBool("AutoLoad", false);
							autoloader.Add(filename, autoload);
						} while (reader->SetToNextChild());
					}
				}
				reader->Close();
			}
		}
	}
	return autoloader;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::SetMasterVolume(float volume)
{
	if (!this->hasSoundDevice) return;

	n_assert(this->lowlevelSystem);

	FMOD::ChannelGroup * group;
	FMOD_RESULT res = this->lowlevelSystem->getMasterChannelGroup(&group);
	FMOD_CHECK_ERROR(res);
	res = group->setVolume(volume);
	FMOD_CHECK_ERROR(res);
}

//------------------------------------------------------------------------------
/**
*/
float
AudioDevice::GetMasterVolume()
{
	if (!this->hasSoundDevice) return 0.0f;

	n_assert(this->lowlevelSystem);

	FMOD::ChannelGroup * group;
	FMOD_RESULT res = this->lowlevelSystem->getMasterChannelGroup(&group);
	FMOD_CHECK_ERROR(res);
	float volume;
	res = group->getVolume(&volume);
	FMOD_CHECK_ERROR(res);
	return volume;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::SetMasterPitch(float pitch)
{
	if (!this->hasSoundDevice) return;

	n_assert(this->lowlevelSystem);

	FMOD::ChannelGroup * group;
	FMOD_RESULT res = this->lowlevelSystem->getMasterChannelGroup(&group);
	FMOD_CHECK_ERROR(res);
	res = group->setPitch(pitch);
	FMOD_CHECK_ERROR(res);
}

//------------------------------------------------------------------------------
/**
*/
float
AudioDevice::GetMasterPitch()
{
	if (!this->hasSoundDevice) return 0.0f;

	n_assert(this->lowlevelSystem);

	FMOD::ChannelGroup * group;
	FMOD_RESULT res = this->lowlevelSystem->getMasterChannelGroup(&group);
	FMOD_CHECK_ERROR(res);
	float pitch;
	res = group->getPitch(&pitch);
	FMOD_CHECK_ERROR(res);
	return pitch;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::SetMasterMute(bool mute)
{
	if (!this->hasSoundDevice) return;

	n_assert(this->lowlevelSystem);

	FMOD::ChannelGroup * group;
	FMOD_RESULT res = this->lowlevelSystem->getMasterChannelGroup(&group);
	FMOD_CHECK_ERROR(res);
	res = group->setMute(mute);
	FMOD_CHECK_ERROR(res);
}

//------------------------------------------------------------------------------
/**
*/
void
FAudio::AudioDevice::SetVCAVolume(float volume, const Util::String & id)
{
	if (!this->hasSoundDevice) return;

	FMOD::Studio::VCA *vca;
	FMOD_RESULT res = this->system->getVCA(id.AsCharPtr(), &vca);
	FMOD_CHECK_ERROR(res);
	res = vca->setFaderLevel(volume);
	FMOD_CHECK_ERROR(res);
}

//------------------------------------------------------------------------------
/**
*/
float
FAudio::AudioDevice::GetVCAVolume(const Util::String & id)
{
	if (!this->hasSoundDevice) return 0.0f;

	FMOD::Studio::VCA *vca;
	FMOD_RESULT res = this->system->getVCA(id.AsCharPtr(), &vca);
	FMOD_CHECK_ERROR(res);
	float level;
	res = vca->getFaderLevel(&level);
	FMOD_CHECK_ERROR(res);
	return level;
}

//------------------------------------------------------------------------------
/**
*/
float
FAudio::AudioDevice::GetBusVolume(const FAudio::BusId & id)
{
	if (!this->hasSoundDevice) return 0.0f;

	FMOD::Studio::Bus *bus;
	FMOD_RESULT res = this->system->getBus(id.GetBus().AsCharPtr(), &bus);
	FMOD_CHECK_ERROR(res);
	float level;
	res = bus->getFaderLevel(&level);
	FMOD_CHECK_ERROR(res);
	return level;
}

//------------------------------------------------------------------------------
/**
*/
void
FAudio::AudioDevice::SetBusVolume(float volume, const FAudio::BusId & id)
{
	if (!this->hasSoundDevice) return;

	FMOD::Studio::Bus *bus;
	FMOD_RESULT res = this->system->getBus(id.GetBus().AsCharPtr(), &bus);
	FMOD_CHECK_ERROR(res);
	res = bus->setFaderLevel(volume);
	FMOD_CHECK_ERROR(res);
}

//------------------------------------------------------------------------------
/**
*/
void
AudioDevice::SetBusPaused(bool paused, const FAudio::BusId & id)
{
	if (!this->hasSoundDevice) return;

	FMOD::Studio::Bus *bus;
	FMOD_RESULT res = this->system->getBus(id.GetBus().AsCharPtr(), &bus);
	FMOD_CHECK_ERROR(res);
	res = bus->setPaused(paused);
	FMOD_CHECK_ERROR(res);
}

}