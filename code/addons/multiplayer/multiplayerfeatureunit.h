#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::MultiplayerFeatureUnit
    
    The MultiplayerFeatureUnit sets up the multiplayer thread and deliveres the
    stuff to host games, join games and all needed functionality behind the scenes

    (C) 2015-2016 Individual contributors, see AUTHORS file	
*/
#include "game/featureunit.h"
#include "networkserver.h"
#include "networkplayer.h"
#include "networkfactorymanager.h"

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class MultiplayerFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(MultiplayerFeatureUnit);
    __DeclareInterfaceSingleton(MultiplayerFeatureUnit);   

public:
    /// constructor
    MultiplayerFeatureUnit();
    /// destructor
    virtual ~MultiplayerFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
	
	/// called from within GameServer::NotifyBeforeCleanup() before shutting down a level
	virtual void OnBeforeCleanup();
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();
    /// flush all messages
    virtual void OnEndFrame();
    /// handle incoming requests from network
    virtual void OnBeginFrame();

	/// called when game debug visualization is on
	virtual void OnRenderDebug();

	/// set application port
	void SetApplicationPort(ushort val);
	/// get application port
	const ushort GetApplicationPort() const;

	/// enable entity sync via replicationmanager
	void SetEnableEntitySync(bool enable);
	/// entity sync enabled
	const bool GetEnableEntitySync() const;


	/// set local player instance
	void SetPlayer(const Ptr<MultiplayerFeature::NetworkPlayer> &player);
	/// get local player instance
	Ptr<MultiplayerFeature::NetworkPlayer> & GetPlayer();

	/// get our unique id
	const Multiplayer::UniquePlayerId & GetUniqueId() const;

	/// restart network
	void RestartNetwork();

protected:
	Ptr<MultiplayerFeature::NetworkServer> server;
	Ptr<MultiplayerFeature::NetworkPlayer> player;
	Ptr<MultiplayerFeature::NetworkFactoryManager> factory;
	ushort applicationPort;	
};


//------------------------------------------------------------------------------
/**
*/
inline const ushort
MultiplayerFeatureUnit::GetApplicationPort() const
{
	return this->applicationPort;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerFeatureUnit::SetApplicationPort(ushort port) 
{
	this->applicationPort = port;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
MultiplayerFeatureUnit::GetEnableEntitySync() const
{
	return this->factory->GetEnableNetworkEntities();
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerFeatureUnit::SetEnableEntitySync(bool enable)
{	
	this->factory->SetEnableNetworkEntities(enable);
}

//------------------------------------------------------------------------------
/**
*/
inline void
MultiplayerFeatureUnit::SetPlayer(const Ptr<MultiplayerFeature::NetworkPlayer> &player)
{
	this->player = player;
}

//------------------------------------------------------------------------------
/**
*/
inline 
Ptr<MultiplayerFeature::NetworkPlayer> &
MultiplayerFeatureUnit::GetPlayer()
{
	return this->player;
}
}; // namespace Multiplayer
//------------------------------------------------------------------------------