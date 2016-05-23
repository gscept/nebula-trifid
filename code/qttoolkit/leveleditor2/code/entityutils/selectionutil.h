#pragma once
//------------------------------------------------------------------------------
/**
	@class LevelEditor2::SelectionUtil

    Manages the selection of entities in a loaded level.
    Analyzes the user input and stores selected entities in
    a list.
*/
#include "core/refcounted.h"
#include "game/entity.h"
#include "input/key.h"
#include "math/bbox.h"
#include "core/singleton.h"
#include "actions/selectaction.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class AttributeWidgetManager;

class SelectionUtil : public Core::RefCounted
{
	__DeclareClass(SelectionUtil);
	__DeclareSingleton(SelectionUtil);
public:
    /// Constructor
    SelectionUtil();
    /// Destructor
    ~SelectionUtil();

    /// Checks the input of mouse and keyboard and updates selection list.
    bool HandleInput();
    /// Returns true if the last handle input returns a new selection.
    bool HasSelectionChanged();
    /// Returns true if there are entities selected
    const bool HasSelection() const;
    /// Returns a list of selected game entities.
    Util::Array<Ptr<Game::Entity>> GetSelectedEntities(bool withChildren = false);
    /// Returns a list of selected game entity ids
    const Util::Array<EntityGuid>& GetSelectedEntityIds();
    /// Returns a game entity from the array of entities
    Ptr<Game::Entity> GetSelectedEntityAtIndex(IndexT index);
    /// Appends an entity array to the selection
    void AppendToSelection(const Util::Array<EntityGuid>& entities);
    /// Sets the selected entities
    void SetSelection(const Util::Array<EntityGuid>& entities);
    /// Removes an array of entities
    void RemoveFromSelection(const Util::Array<EntityGuid>& entities);
    /// Clears the selection
    void ClearSelection();

    /// Renders a simple representation of the selection into 3D space
    void Render();
	/// currently performing a drag operation
	const bool IsInDrag() const;	
   
    /// Calculate bounding box of a list of entities
    static Math::bbox CalculateGroupBox(const Util::Array<EntityGuid>& entities);
      
    /// returns if the entity is selected
    bool IsSelected(const Ptr<Game::Entity>& entity);

private:
    friend class AttributeWidgetManager;
    friend class SelectAction;

    /// get the selection index of the entity
    IndexT GetIndexOfEntity(const Ptr<Game::Entity> entity);
    /// Returns the game entity under the mouse
    void GetEntityUnderMouse();
    /// called when an entity ID have been retrieved
    void OnEntityClicked(const Ptr<Messaging::Message>& msg);
	/// called when marquee selection has been retrieved
	void OnEntitiesClicked(const Ptr<Messaging::Message>& msg);

    /// clear any previous attribute controllers, and create new for the entity
    void SetEntityForAttributeView(const Ptr<Game::Entity> entity);

    /// Renders a bounding box representation
    void RenderBBox(const Math::bbox & box);

    ///
    void AppendToSelection(const EntityGuid& entity);
    ///
    void RemoveFromSelection(const EntityGuid& entity);
	///
	void UpdateModels();


    Ptr<AttributeWidgetManager> attributeWidgetManager;
    Util::Array<EntityGuid> selectedEntities;
			
    bool hasSelectionChanged;
    bool isActive;
    bool multiSelect;	
    bool mouseDrag;
	bool marqueeSelecting;
	bool selectInside;
	Math::float2 clickPos;
	Math::float2 clickPosWindow;
	        
    Input::Key::Code keyMultiSelection;
	Input::Key::Code keyMultiSelectionRemove;
};

//------------------------------------------------------------------------------
/**
*/
inline const bool 
SelectionUtil::HasSelection() const
{
	return this->selectedEntities.Size() > 0;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
SelectionUtil::IsInDrag() const
{
	return this->mouseDrag;
}
//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<EntityGuid>&
SelectionUtil::GetSelectedEntityIds()
{
	return this->selectedEntities;
}

}