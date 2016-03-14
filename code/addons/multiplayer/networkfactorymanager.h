#pragma once
//------------------------------------------------------------------------------
/**
@class MultiplayerFeature::NetworkFactoryManager

(C) 2015-2016 Individual contributors, see AUTHORS file
*/

#include "managers/factorymanager.h"

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class NetworkFactoryManager : public BaseGameFeature::FactoryManager
{
	__DeclareClass(NetworkFactoryManager);
	__DeclareSingleton(NetworkFactoryManager);
public:
	/// constructor
	NetworkFactoryManager();
	/// destructor
	virtual ~NetworkFactoryManager();
	/// create a new raw game entity by type name, extend this method in subclasses for new types
	virtual Ptr<Game::Entity> CreateEntityByClassName(const Util::String& cppClassName) const;
	/// create an empty entity by category for filling in attributes later
	virtual Ptr<Game::Entity> CreateEmptyEntity(const Util::String & category);
	/// enable creation of network entities, triggering entity sync
	void SetEnableNetworkEntities(bool enable);
	/// get entity sync 
	const bool GetEnableNetworkEntities() const;
private:
	bool useNetworked;
};

//------------------------------------------------------------------------------
/**
*/
inline void
NetworkFactoryManager::SetEnableNetworkEntities(bool enable)
{
	this->useNetworked = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const bool
NetworkFactoryManager::GetEnableNetworkEntities() const
{
	return this->useNetworked;
}



}