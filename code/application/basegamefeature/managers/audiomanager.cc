//------------------------------------------------------------------------------
//  managers/audiomanager.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/audiomanager.h"
#include "dbserver.h"

namespace BaseGameFeature
{
__ImplementClass(BaseGameFeature::AudioManager, 'AUMA', Game::Manager);

//------------------------------------------------------------------------------
/**
*/
AudioManager::AudioManager()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AudioManager::~AudioManager()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the manager is attached to the game server.
    The manager base class will register its message port with the
    message server.
*/
void
AudioManager::OnActivate()
{
	Manager::OnActivate();
	this->audioServer = FAudio::AudioServer::Create();
	this->audioServer->Open();
}

//------------------------------------------------------------------------------
/**
    This method is called when the manager is removed from the game server.
    It will unregister its message port from the message server at this point.
*/
void
AudioManager::OnDeactivate()
{
	Manager::OnDeactivate();
	this->audioServer->Close();
	this->audioServer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioManager::OnFrame()
{
	this->audioServer->OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
AudioManager::OnLoad()
{
	this->audioServer->LoadBanks(Db::DbServer::Instance()->GetStaticDatabase());
}

//------------------------------------------------------------------------------
/**
*/
void
AudioManager::OnRenderDebug()
{
    // empty
}

}; // namespace BaseGameFeature
