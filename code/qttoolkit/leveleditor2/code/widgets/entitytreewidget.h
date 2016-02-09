#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::EntityTreeWidget
    
    Overrides the base QTreeWidget in order to implement extra functionality.
	This widget views the contents of a EntityStructureModel.

    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QTreeWidget>
#include <QMimeData>
#include "entityguid.h"
#include "core/ptr.h"
#include "entityutils/selectionutil.h"
#include "leveleditorstate.h"

namespace LevelEditor2
{
class EntityTreeItem;
class SelectionUtil;
class LevelEditorState;

class EntityTreeWidget : public QTreeWidget
{
public:
	/// constructor
	EntityTreeWidget(QWidget* parent);
	/// destructor
	~EntityTreeWidget();

	/// add an entity-treeitem
	void AddEntityTreeItem(EntityTreeItem* item);
	/// remove an entity-treeitem, via its id
	void RemoveEntityTreeItem(EntityGuid id);

	/// append entity to selection
	void AppendToSelection(EntityGuid id);
	/// remove entity from selection
	void RemoveFromSelection(EntityGuid id);	

	/// set the leveleditorstate class
	void SetEditorState(const Ptr<LevelEditorState>& state);

	/// get entity tree item from a guid
	EntityTreeItem* GetEntityTreeItem(const Util::Guid& guid) const;

	/// get direct children 
	Util::Array<Ptr<Game::Entity>> GetDirectChildren(const EntityGuid &id);
    /// get all children 
    Util::Array<Ptr<Game::Entity>> GetSelectionRecursive();

	/// get all currently selected items
	Util::Array<Ptr<Game::Entity>> GetSelection() const;

	/// select all children, used with delete
	void SelectAllChildren();

	/// find parent
	Util::Guid GetParent(const Util::Guid & guid);

	/// update all parent child relations (sets parentguid to all entities)
	void SetParentGuids();

	/// rebuild tree from guid information
	void RebuildTree();


	friend class EntityTreeItem;
protected:
	/// override selection-changed method
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	/// overridden qt function
	Qt::DropActions supportedDropActions() const;	
	/// overridden qt function
	void dropEvent(QDropEvent * event);	
	/// context menu
	void contextMenuEvent(QContextMenuEvent * event);
	/// prepare mime data for drag event
	QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	/// remove an id from the dictionary
	void Deregister(EntityGuid id);
	/// select recursively
	Util::Array<EntityGuid> SelectRecursive(EntityTreeItem* item);
    /// get selected items and all children of selected items
    Util::Array<EntityGuid> CollectAllChildren();
    /// set parent guid recursively
	void SetParentGuid(const Util::Guid & guid, EntityTreeItem * child);
	
	Ptr<LevelEditorState> editorState;
	
	Util::Dictionary<EntityGuid, EntityTreeItem*> itemDictionary;	//< keep a dictionary for quick access of entities, much quicker than traversing all nodes and check their guids
	bool blockSignal;
}; 

//------------------------------------------------------------------------------
/**
	Simply a QTreeWidgetItem with an EntityGuid member
*/
class EntityTreeItem : public QTreeWidgetItem
{
public:
	virtual ~EntityTreeItem();
	/// set entity guid
	void SetEntityGuid(EntityGuid id);
	/// get entityt guid
	EntityGuid GetEntityGuid() const;
	/// set item text
	void SetText(const Util::String& text);	
	/// set icon according to type
	void SetIcon(const EntityType& newType);
private:
	EntityGuid entityGuid;	//< the guid of the entity this item represents
};



//------------------------------------------------------------------------------
/**
*/
inline void 
EntityTreeItem::SetEntityGuid(EntityGuid id)
{
	this->entityGuid = id;
}

//------------------------------------------------------------------------------
/**
*/
inline LevelEditor2::EntityGuid 
EntityTreeItem::GetEntityGuid() const
{
	return this->entityGuid;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
EntityTreeWidget::SetEditorState(const Ptr<LevelEditorState>& state)
{
	this->editorState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline EntityTreeItem* 
EntityTreeWidget::GetEntityTreeItem(const Util::Guid& guid) const
{
	n_assert(this->itemDictionary.Contains(guid));
	return this->itemDictionary[guid];
}

//------------------------------------------------------------------------------
/**
*/
inline void 
EntityTreeItem::SetText(const Util::String& text)
{
	this->setText(0, text.AsCharPtr());
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------