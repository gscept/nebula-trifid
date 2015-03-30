//------------------------------------------------------------------------------
//  audioserver.cc
//  (C) 2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audioserver.h"
#include "audioprotocol.h"
#include "faudio/audiodevice.h"
#include "audiohandler.h"

using namespace FAudio;

__ImplementClass(FAudio::AudioServer, 'AUSV', Core::RefCounted);
__ImplementSingleton(FAudio::AudioServer);

using namespace Util;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
AudioServer::AudioServer() :
    isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AudioServer::~AudioServer()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioServer::Open()
{
    n_assert(!this->IsOpen());

	this->device = AudioDevice::Create();    
	this->handler = AudioHandler::Create();
    
	this->device->Open();
	this->handler->Open();
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioServer::Close()
{
    n_assert(this->IsOpen());
	this->handler->Close();
	this->device->Close();	
    this->device = 0;
	this->handler = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioServer::OnFrame()
{
    n_assert(this->IsOpen());
       
	this->device->OnFrame();    
}

//------------------------------------------------------------------------------
/**
*/
void
AudioServer::LoadBanks()
{
	Util::Dictionary<Util::String,bool> autoloads = AudioDevice::ParseAutoload("data:tables/audio.xml");
	for (int i = 0; i < autoloads.Size(); i++)
	{
		if (autoloads.ValueAtIndex(i))
		{
			Util::String bank = autoloads.KeyAtIndex(i);
			this->device->LoadBank(bank);
		}
	}
}