#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::AttributeWidgetManager
    
    Manages the container for viewing game entity attributes.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "core/singleton.h"
#include "game/manager.h"
#include "callbackmanager.h"
#include <QLayout>

namespace Game
{
	class Entity;
}

namespace QtAttributeControllerAddon
{
	class BaseAttributeController;
	class Matrix44Controller;
}

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EntityTreeWidget;

class AttributeWidgetManager: public QtAttributeControllerAddon::CallbackManager
{
	__DeclareClass(AttributeWidgetManager);
	__DeclareSingleton(AttributeWidgetManager);
public:

	/// constructor
	AttributeWidgetManager();
	/// destructor
	~AttributeWidgetManager();

	/// called when attached to game server
	void OnActivate();
	/// called when removed from game server
	void OnDeactivate();
	
	/// create and view widgets for editing the entitys attributes
	void ViewEntityAttributes(const Ptr<Game::Entity>& entity);
	/// create and view widgets for editing the entitys attributes
	void ViewEntityAttributes(const Util::Array<Ptr<Game::Entity>>& entities);
	/// clear all attribute controllers
	void ClearAttributeControllers();
	/// returns if the entity is the currently viewed entity
	bool IsCurrentlyViewed(const Ptr<Game::Entity>& entity) const;

	/// set the attribute widget container
	void SetAttributeWidgetContainer(QLayout* cont);
	/// set the entity tree widget
	void SetEntityTreeWidget(EntityTreeWidget* widget);
	
	/// get the transform controller of the currently viewed entity
	QtAttributeControllerAddon::Matrix44Controller* GetCurrentTransformController() const;

protected:
	/// updates the attribute value with the controllers value
	void OnValueChanged(QtAttributeControllerAddon::BaseAttributeController* controller);

private:
	QLayout* container;
	EntityTreeWidget* treeWidget;

	Ptr<Game::Entity> currentEntity;	//< the entity which attributes are currently viewed (can be a null pointer)
	QtAttributeControllerAddon::Matrix44Controller* transformController;
	Ptr<Game::Entity> multiEntity;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
AttributeWidgetManager::SetAttributeWidgetContainer(QLayout* cont)
{
	this->container = cont;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
AttributeWidgetManager::IsCurrentlyViewed(const Ptr<Game::Entity>& entity) const
{
	return this->currentEntity == entity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttributeWidgetManager::SetEntityTreeWidget(EntityTreeWidget* widget)
{
	this->treeWidget = widget;
}

//------------------------------------------------------------------------------
/**
*/
inline QtAttributeControllerAddon::Matrix44Controller* 
AttributeWidgetManager::GetCurrentTransformController() const
{
	n_assert(this->currentEntity.isvalid());
	return this->transformController;
}

}