//------------------------------------------------------------------------------
//  networkfactorymanager.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkfactorymanager.h"
#include "networkentity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace MultiplayerFeature
{
	__ImplementClass(MultiplayerFeature::NetworkFactoryManager, 'NWFM', BaseGameFeature::FactoryManager);
	__ImplementSingleton(NetworkFactoryManager);

//------------------------------------------------------------------------------
/**
*/
NetworkFactoryManager::NetworkFactoryManager():
	useNetworked(false)
{
	__ConstructSingleton;
	
}
 
//------------------------------------------------------------------------------
/**
*/
NetworkFactoryManager::~NetworkFactoryManager()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
.
*/
Ptr<Game::Entity>
NetworkFactoryManager::CreateEntityByClassName(const Util::String& cppClassName) const
{
	if (this->useNetworked)
	{
		if ("Entity" == cppClassName)
		{
			return NetworkEntity::Create();
		}
		else
		{
			n_error("MultiplayerFeature::NetworkFactoryManager::CreateEntityByClassName(): unknown entity class name '%s'!", cppClassName.AsCharPtr());
			return 0;
		}
	}
	else
	{
		return BaseGameFeature::FactoryManager::CreateEntityByClassName(cppClassName);
	}
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity>
NetworkFactoryManager::CreateEmptyEntity(const Util::String & category)
{
	Util::Array<Attr::Attribute> arr;
	Ptr<Game::Entity> entity = this->CreateEntityByAttrs(category, arr,false);
	IndexT row = entity->GetAttrTableRowIndex();
	const Ptr<Db::ValueTable>& table = entity->GetAttrTable();
	if (!table->HasColumn(Attr::IsMaster))
	{
		table->AddColumn(Attr::IsMaster);
		// this function is only called for entities created on some other client, we can never own it
		entity->SetBool(Attr::IsMaster, false);
	}	
	return entity;
}

}
