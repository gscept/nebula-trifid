//------------------------------------------------------------------------------
//  audioserver.cc
//  (C) 2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudio/audioserver.h"
#include "faudio/audioprotocol.h"
#include "faudio/audiodevice.h"
#include "audiohandler.h"
#include "reader.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

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
FAudio::AudioServer::LoadBanks(const Ptr<Db::Database> & staticDb)
{
    if(staticDb->HasTable("_Template__AudioBanks"))
    {    
        Ptr<Db::Reader> reader = Db::Reader::Create();
        reader->SetDatabase(staticDb);
        reader->SetTableName("_Template__AudioBanks");
        reader->Open();
        for(IndexT i = 0 ; i < reader->GetNumRows(); i++)
        {
            reader->SetToRow(i);
            if(reader->GetBool(Attr::AutoLoad))
            {
                this->device->LoadBank(reader->GetString(Attr::Id));
            }
        }
        reader->Close();
    }
}