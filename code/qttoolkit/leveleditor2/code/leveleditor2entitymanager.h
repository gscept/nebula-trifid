#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::LevelEditor2EntityManager
    
    Manager for easy handling of game entities. Entities are handled through
	their GUID attribute.
	This also acts as a controller for the entity structure and its treeview 
	representation. When adding or removing entities, do it through the 
	AddEntity and RemoveEntity methods, since they do the updating of the 
	structure and view.

	Important note: if you want create or remove an entity and let it be
	undoable, then do it through the ActionManager instead.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"
#include "entityguid.h"
#include "attr/attribute.h"
#include "widgets/entitytreewidget.h"
#include "attr/attributecontainer.h"
#include "level.h"

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class EntityStructureModel;
class EntityNode;

class LevelEditor2EntityManager : public Game::Manager
{
	__DeclareClass(LevelEditor2EntityManager);
    __DeclareSingleton(LevelEditor2EntityManager);
public:
	/// constructor
	LevelEditor2EntityManager();
	/// destructor
	~LevelEditor2EntityManager();

	// important note: only attach or remove entities via AddEntity and RemoveEntity, since it controls other things such as the treeview
	/// attach an entity to the world 
	void AddEntity(const Ptr<Game::Entity>& entity);
	/// remove an entity with the given id (uses EntityGuid)
	void RemoveEntity(EntityGuid id);
	/// remove an entity
	void RemoveEntity(const Ptr<Game::Entity>& entity, bool immediate = false);

    void LoadLevel(const Util::String & name, Level::LoadMode mode);
    ///
    void OnEndFrame();

	/// returns true if an entity with the id exists (uses EntityGuid)
	bool EntityExists(EntityGuid id) const;
	/// returns the entity with the given id (uses EntityGuid)
	Ptr<Game::Entity> GetEntityById(EntityGuid id) const;

	/// create an environment entity and attach it to the game world, then return its id
	EntityGuid CreateEnvironmentEntity(const Util::String& _template);
	/// create a light entity, attach it to the world and return its id
	EntityGuid CreateLightEntity(const Util::String& lightType);
	/// create a game entity, attach it to the world and return its id
	EntityGuid CreateGameEntity(const Util::String& category, const Util::String &_template);
	/// create a group transform node, attach it and return its id
	EntityGuid CreateTransformEntity(const Util::String & name);
	/// create a new navmesh entity for storing navmesh properties
	EntityGuid CreateNavMeshEntity();
	/// create a new light probe entity
	EntityGuid CreateLightProbeEntity();
    /// create nav area
    EntityGuid CreateNavArea();

	/// create an entity, by some attributes
	Ptr<Game::Entity> CreateEntityByAttrs(const Util::Array<Attr::Attribute>& attributes, const Util::String & entityclass = "EditorEntity", const Util::String & guidStr = "");

	/// duplicate an entity
	EntityGuid DuplicateEntity(const Ptr<Game::Entity>& entity);
	/// duplicate an entity list
	Util::Array<EntityGuid> DuplicateEntities(const Util::Array<Ptr<Game::Entity>> & entities);

	/// creates an entity from the values defined in the attributecontainer, filling up missing attributes	
	void CreateEntityFromAttrContainer(const Util::String & levelName, const Util::String & category, Attr::AttributeContainer attrs);

	/// clear all entities
	void RemoveAllEntities(bool immediate = false);
		
	/// get the global light entity, can be null
	Ptr<Game::Entity> GetGlobalLight();

	/// set entity tree widget
	void SetEntityTreeWidget(EntityTreeWidget* widget);
	/// update category entities
	void UpdateCategoryEntities(const Util::String & category);
	
	/// morph an entity into a different category
	void MorphEntity(Ptr<Game::Entity> entity, const Util::String & target);

private:
	/// calculate transform for new entities
	Math::matrix44 GetPlacementTransform();
	/// update the treeview with a new entity
	void AddTreeviewNode(const Ptr<Game::Entity>& node);
	/// remove a entity node from the treeview
	void RemoveTreeviewNode(const Ptr<Game::Entity>& node);
   
    Util::String delayedLevel;
    Level::LoadMode delayedMode;

	EntityTreeWidget* entityTreeWidget;

};

//------------------------------------------------------------------------------
/**
*/
inline void 
LevelEditor2EntityManager::SetEntityTreeWidget(EntityTreeWidget* widget)
{
	this->entityTreeWidget = widget;
}

} // namespace LevelEditor2
