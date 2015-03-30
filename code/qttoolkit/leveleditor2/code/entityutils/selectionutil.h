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

//------------------------------------------------------------------------------
namespace LevelEditor2
{
class AttributeWidgetManager;
class EntityTreeWidget;

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
    Util::Array<Ptr<Game::Entity>> GetSelectedEntities();
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

    /// Toggle group selection
    void ToggleGroup();
    /// Calculate group bbox
    void CalculateGroupBox();
    /// Update group box
    //void UpdateGroupBox(const Math::matrix44& matrix);
    /// Get group box
    Math::bbox& GetGroupBox();
    /// Get group matrix
    Math::matrix44& GetGroupMatrix();

    /// Returns true if this is in group mode
    bool InGroupMode();

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

    /// clear any previous attribute controllers, and create new for the entity
    void SetEntityForAttributeView(const Ptr<Game::Entity> entity);

    /// Renders a bounding box representation
    void RenderBBox(const Math::bbox & box);

    ///
    void AppendToSelection(const EntityGuid& entity);
    ///
    void RemoveFromSelection(const EntityGuid& entity);


    Ptr<AttributeWidgetManager> attributeWidgetManager;
    Util::Array<EntityGuid> selectedEntities;
    Util::Array<Math::bbox> boundingBoxes;

    bool hasSelectionChanged;
    bool isActive;
    bool multiSelect;
    // 	bool wasActiveBefore;

    bool groupMode;
    Math::bbox groupBox;
    Math::matrix44 groupMatrix;
    Util::Array<EntityGuid> entityGroup;

    EntityTreeWidget* treeWidget;	//< the entity treeview selection should be synced with the 3dview selection

    Input::Key::Code keyMultiSelection;

};

//------------------------------------------------------------------------------
/**
*/
inline
const bool 
SelectionUtil::HasSelection() const
{
	return this->selectedEntities.Size() > 0;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
SelectionUtil::InGroupMode()
{
	return this->groupMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<EntityGuid> &
SelectionUtil::GetSelectedEntityIds()
{
	return this->selectedEntities;
}

}