//------------------------------------------------------------------------------
//  entitytreewidget.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "entitytreewidget.h"
#include "leveleditor2entitymanager.h"
#include "properties/editorproperty.h"
#include "qevent.h"
#include "qstandarditemmodel.h"
#include "managers/entitymanager.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "leveleditor2app.h"
#include "QInputDialog"
#include "editorfeatures/editorblueprintmanager.h"

namespace LevelEditor2
{

//------------------------------------------------------------------------------
/**
*/

EntityTreeItem::EntityTreeItem():
    locked(false),
    visible(true)
{

}

//------------------------------------------------------------------------------
/**
*/
EntityTreeItem::~EntityTreeItem()
{
	if (this->entityGuid.IsValid())
	{
		LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->Deregister(this->entityGuid);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeItem::SetIcon(const EntityType& type)
{	
	this->type = type;
	switch (type)
	{

		case Environment:
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8(":/icons/icons/Environment.png"), QSize(), QIcon::Normal, QIcon::Off);
			this->setIcon(0, icon);
		}
		break;
		case Game:
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8(":/icons/icons/Game.png"), QSize(), QIcon::Normal, QIcon::Off);
			this->setIcon(0, icon);
		}	
		break;
		case Light:
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8(":/icons/icons/Light.png"), QSize(), QIcon::Normal, QIcon::Off);
			this->setIcon(0, icon);
		}
		break;
		case Group:
		{
			QStyle* style = QApplication::style();
			this->setIcon(0, style->standardIcon(QStyle::SP_DirIcon));
		}
		break;
		default:
		{
			QStyle* style = QApplication::style();
			this->setIcon(0, style->standardIcon(QStyle::SP_FileIcon));
		}
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeItem::SetLocked(bool lock)
{
    this->locked = lock;
}

//------------------------------------------------------------------------------
/**
*/
EntityTreeWidget::EntityTreeWidget(QWidget* parent) :
	QTreeWidget(parent),
	blockSignal(false),
	sortingEnabled(false)
{
	this->setDragEnabled(true);
	this->setAcceptDrops(true);	
	this->setDragDropMode(QAbstractItemView::DragDrop );
	this->setDefaultDropAction(Qt::MoveAction);
	this->viewport()->setAcceptDrops(true);
	this->setDropIndicatorShown(true);	
	this->header()->setResizeMode(QHeaderView::ResizeToContents);
	this->header()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->header(), SIGNAL(customContextMenuRequested(const QPoint&)),	this, SLOT(HeaderContextMenu(const QPoint&)));
}

//------------------------------------------------------------------------------
/**
*/
EntityTreeWidget::~EntityTreeWidget()
{	
	this->editorState = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::AddEntityTreeItem(EntityTreeItem* item)
{
	n_assert(!this->itemDictionary.Contains(item->GetEntityGuid()));
	this->itemDictionary.Add(item->GetEntityGuid(), item);	

	// find group node
	Ptr<Game::Entity> ent = LevelEditor2EntityManager::Instance()->GetEntityById(item->GetEntityGuid());
	EntityType type = (EntityType) ent->GetInt(Attr::EntityType);
	item->SetIcon(type);

	// add item last
	int index = this->topLevelItemCount();
	this->insertTopLevelItem(index, item);
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::RemoveEntityTreeItem(EntityGuid id)
{
	if(this->itemDictionary.Contains(id))
	{
		EntityTreeItem* item = this->itemDictionary[id];
		this->blockSignal = true;
		delete item;	
		this->blockSignal = false;
	}		
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::Deregister(EntityGuid id)
{
	if(this->itemDictionary.Contains(id))
	{
		this->itemDictionary.Erase(id);			
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::AppendToSelection(EntityGuid id)
{
	n_assert(this->itemDictionary.Contains(id));

	// doesnt work. weird
	this->blockSignals(true);
	this->blockSignal = true;
	this->itemDictionary[id]->setSelected(true);
	this->blockSignal = false;
	this->blockSignals(false);	
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::RemoveFromSelection(EntityGuid id)
{
	n_assert(this->itemDictionary.Contains(id));
	this->blockSignals(true);
	this->blockSignal = true;
	this->itemDictionary[id]->setSelected(false);
	this->blockSignal = false;
	this->blockSignals(false);
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{

	QTreeView::selectionChanged(selected, deselected);
		
	if(this->blockSignal)
	{
		return;
	}
	// get selected items from tree
	QList<QTreeWidgetItem *> items = this->selectedItems();

	// add items to a new array
	Util::Array<EntityGuid> entities;
	for(int idx = 0;idx<items.size();idx++)
	{
		entities.Append(dynamic_cast<EntityTreeItem*>(items[idx])->GetEntityGuid());
	}
	// add entities in a big chunk
	if(entities.Size())
	{
		Ptr<SelectAction> action = SelectAction::Create();
		action->SetSelectionMode(SelectAction::SetSelection);
		action->SetEntities(entities);
		ActionManager::Instance()->PerformAction(action.cast<Action>());
	}		
	else
	{
		Ptr<SelectAction> action = SelectAction::Create();
		action->SetSelectionMode(SelectAction::ClearSelection);						
		ActionManager::Instance()->PerformAction(action.cast<Action>());
	}	
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<EntityGuid>
EntityTreeWidget::SelectRecursive(EntityTreeItem* item)
{
	Util::Array<EntityGuid> guids;
	guids.Append(dynamic_cast<EntityTreeItem*>(item)->GetEntityGuid());
	for(int i=0;i<item->childCount();i++)
	{
		Util::Array<EntityGuid> childGuids = SelectRecursive(dynamic_cast<EntityTreeItem*>(item->child(i)));
		for (int j = 0; j < childGuids.Size(); j++)
		{
			if (InvalidIndex == guids.BinarySearchIndex(childGuids[j]))
			{
				guids.InsertSorted(childGuids[j]);
			}
		}		
	}
	return guids;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<EntityGuid>
EntityTreeWidget::CollectAllChildren()
{
    QList<QTreeWidgetItem *> items = this->selectedItems();
    Util::Array<EntityGuid> guids;
    for (int i = 0; i < items.size(); i++)
    {
        Util::Array<EntityGuid> childGuids = SelectRecursive(dynamic_cast<EntityTreeItem*>(items[i]));
        for (int j = 0; j < childGuids.Size(); j++)
        {
            if (InvalidIndex == guids.BinarySearchIndex(childGuids[j]))
            {
                guids.InsertSorted(childGuids[j]);
            }
        }        
    }
    return guids;    
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::SelectAllChildren()
{	
    Util::Array<EntityGuid> guids = this->CollectAllChildren();
	Ptr<SelectAction> action = SelectAction::Create();
	action->SetSelectionMode(SelectAction::SetSelection);
	action->SetEntities(guids);
	ActionManager::Instance()->PerformAction(action.cast<Action>());
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Game::Entity>>  
EntityTreeWidget::GetDirectChildren(const EntityGuid &id)
{
	EntityTreeItem * item = this->GetEntityTreeItem(id);
	Util::Array<Ptr<Game::Entity>> children;
	for(int i=0 ; i < item->childCount();i++)
	{
		children.Append(LevelEditor2EntityManager::Instance()->GetEntityById(dynamic_cast<EntityTreeItem*>(item->child(i))->GetEntityGuid()));
	}
	return children;

}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Game::Entity>>
EntityTreeWidget::GetSelectionRecursive()
{    
    Util::Array<Ptr<Game::Entity>> entities;
    Util::Array<Util::Guid> items = this->CollectAllChildren();
    for (int i = 0; i < items.Size(); i++)
    {
        entities.Append(LevelEditor2EntityManager::Instance()->GetEntityById(items[i]));
    }
    return entities;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Game::Entity>>
EntityTreeWidget::GetSelection() const
{
	QList<QTreeWidgetItem *> items = this->selectedItems();
	Util::Array<Ptr<Game::Entity>> ents;
	for (int i = 0; i < items.size(); i++)
	{
		EntityGuid guid = dynamic_cast<EntityTreeItem*>(items[i])->GetEntityGuid();
		ents.Append(LevelEditor2EntityManager::Instance()->GetEntityById(guid));
	}
	return ents;
}

//------------------------------------------------------------------------------
/**
*/
Qt::DropActions 
EntityTreeWidget::supportedDropActions() const
{	
	return Qt::MoveAction | Qt::CopyAction;
}


//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::dropEvent ( QDropEvent * event )
{
	QTreeWidget::dropEvent(event);
	SetParentGuids();
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::contextMenuEvent(QContextMenuEvent * event)
{
	EntityTreeItem * myitem = dynamic_cast<EntityTreeItem*>(this->itemAt(event->pos()));
	if (myitem == NULL)
	{				
		return;
	}
	if (myitem->type == LevelReference)
	{
        QMenu menu;
        QAction * remove = menu.addAction("Remove reference");
        QAction * lock = menu.addAction(myitem->locked ? "Unlock" : "Lock");
        QAction * visible = menu.addAction(myitem->visible ? "Hide" : "Show");
        QAction * sel = menu.exec(event->globalPos());
        if (sel == remove)
        {
            Level::Instance()->RemoveReference(myitem->level);
            IndexT idx = this->referenceItems.FindIndex(myitem->level);
            this->referenceItems.EraseAtIndex(idx);
            delete myitem;
        }
        else if (sel == lock)
        {
            myitem->locked = !myitem->locked;
            Util::Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityLevel, myitem->level));
            for (int i = 0; i < ents.Size(); i++)
            {
                ents[i]->SetBool(Attr::IsLocked, myitem->locked);
            }
        }
        else if (sel == visible)
        {
            myitem->visible = !myitem->visible;
            Util::Array<Ptr<Game::Entity>> ents = BaseGameFeature::EntityManager::Instance()->GetEntitiesByAttr(Attr::Attribute(Attr::EntityLevel, myitem->level));
            Ptr<GraphicsFeature::SetGraphicsVisible> msg = GraphicsFeature::SetGraphicsVisible::Create();
            msg->SetVisible(myitem->visible);
            for (int i = 0; i < ents.Size(); i++)
            {
                __SendSync(ents[i], msg);
            }            
        }
		return;
	}
	if (this->selectedItems().count() > 1)
	{
		QMenu menu;
		QAction* morphAction = menu.addAction("Morph entity");
		QAction* morphToEnvironment = menu.addAction("Morph to model");
		QAction* sel = menu.exec(event->globalPos());
		if (sel == morphAction)
		{
			Ptr<Toolkit::EditorBlueprintManager> manager = Toolkit::EditorBlueprintManager::Instance();
			QStringList items;
			int selected = 0;
			for (int i = 0; i < manager->GetNumCategories(); i++)
			{
				Util::String cat = manager->GetCategoryByIndex(i);				
				items.append(cat.AsCharPtr());
			}
			bool ok;
			QString target = QInputDialog::getItem(this, "Select new blueprint", "Blueprint", items, selected, false, &ok);
			if (ok)
			{
				Util::Array<Ptr<Game::Entity>> selection = this->GetSelection();
				for (int i = 0; i < selection.Size(); i++)
				{
					Util::String cat = selection[i]->GetString(Attr::EntityCategory);
					if (cat != "Transform" && cat != "Light" && cat != "NavMeshData")
					{
						LevelEditor2EntityManager::Instance()->MorphEntity(selection[i], target.toLatin1().constData());
						EntityTreeItem * item = GetEntityTreeItem(selection[i]->GetGuid(Attr::EntityGuid));
						item->SetIcon((EntityType)selection[i]->GetInt(Attr::EntityType));
					}
				}				
			}
		}
		else if (sel == morphToEnvironment)
		{
			Util::Array<Ptr<Game::Entity>> selection = this->GetSelection();
			for (int i = 0; i < selection.Size(); i++)
			{
				Util::String cat = selection[i]->GetString(Attr::EntityCategory);
				if (cat != "Transform" && cat != "Light" && cat != "NavMeshData" && cat != "_Environment")
				{
					LevelEditor2EntityManager::Instance()->MorphEntity(selection[i], "_Environment");
					EntityTreeItem * item = GetEntityTreeItem(selection[i]->GetGuid(Attr::EntityGuid));
					item->SetIcon((EntityType)selection[i]->GetInt(Attr::EntityType));
				}
			}
		}
	}
	else
	{
		Ptr<Game::Entity> entity = LevelEditor2EntityManager::Instance()->GetEntityById(myitem->GetEntityGuid());
		Util::String oldcategory = entity->GetString(Attr::EntityCategory);
		// we dont support morphing light sources
		if (oldcategory == "Light")
		{
			return;
		}

		if (oldcategory == "Transform")
		{
			QMenu menu;
			QAction* selectAction = menu.addAction("Select all children");
			QAction* selected = menu.exec(event->globalPos());
			if (selected == selectAction)
			{
				this->SelectAllChildren();
			}
			return;
		}

		QMenu menu;
		QAction* morphAction = menu.addAction("Morph entity");
		QAction* morphToEnvironment = menu.addAction("Morph to model");
		if (entity->GetInt(Attr::EntityType) != Game)
		{
			morphToEnvironment->setDisabled(true);
		}
		QAction* selected = menu.exec(event->globalPos());
		if (selected == morphAction)
		{
			Ptr<Toolkit::EditorBlueprintManager> manager = Toolkit::EditorBlueprintManager::Instance();
			QStringList items;
			int selected = 0;
			for (int i = 0; i < manager->GetNumCategories(); i++)
			{
				Util::String cat = manager->GetCategoryByIndex(i);
				if (cat == oldcategory)
				{
					selected = i;
				}
				items.append(cat.AsCharPtr());
			}
			bool ok;
			QString target = QInputDialog::getItem(this, "Select new blueprint", "Blueprint", items, selected, false, &ok);
			if (ok)
			{
				LevelEditor2EntityManager::Instance()->MorphEntity(entity, target.toLatin1().constData());
			}
		}
		else if (selected == morphToEnvironment)
		{
			LevelEditor2EntityManager::Instance()->MorphEntity(entity, "_Environment");
		}
		myitem->SetIcon((EntityType)entity->GetInt(Attr::EntityType));
	}
	
}

//------------------------------------------------------------------------------
/**
*/
QMimeData*
EntityTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	// we always use single select, so the amount of items we get can be at most 1
	QTreeWidgetItem* item = items[0];

	// it has to be a base item
	EntityTreeItem* baseItem = dynamic_cast<EntityTreeItem*>(item);
	// create mimedata
	QMimeData* mimeData = QTreeWidget::mimeData(items);
	if (baseItem)
	{	
		const Ptr<Game::Entity>& entity = LevelEditor2EntityManager::Instance()->GetEntityById(baseItem->GetEntityGuid());
		if (entity->HasAttr(Attr::Graphics))
		{
			Util::String res = entity->GetString(Attr::Graphics);
			if (res.FindStringIndex("system/") == InvalidIndex)
			{
				mimeData->setData("nebula/resourceid", res.AsCharPtr());								
			}
		}				
	}
	// if that fails, return 0
	return mimeData;
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::HeaderContextMenu(const QPoint & pos)
{
	QMenu menu;
	QAction * sortAction = menu.addAction(this->isSortingEnabled() ? "Disable Sorting" : " Enable Sorting");	
	QAction *sel = menu.exec(this->header()->mapToGlobal(pos));
	if (sel == sortAction)
	{
		if (this->isSortingEnabled())
		{
			this->header()->setSortIndicatorShown(false);
			this->setSortingEnabled(false);
		}
		else
		{
			this->header()->setSortIndicatorShown(true);
			this->setSortingEnabled(true);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Util::Guid 
EntityTreeWidget::GetParent(const Util::Guid & guid)
{
	Util::Guid parent;
	if(this->itemDictionary.Contains(guid))
	{
		EntityTreeItem * item = this->itemDictionary[guid];
		EntityTreeItem * parentItem = dynamic_cast<EntityTreeItem*>(item->parent());
		if(parentItem != NULL)
		{				
			parent = parentItem->GetEntityGuid();
		}
	}	
	return parent;
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::SetParentGuids()
{
	Util::Guid invalid;
	for(int i = 0 ; i<this->topLevelItemCount(); i++)
	{
		EntityTreeItem* item = dynamic_cast<EntityTreeItem*>(this->topLevelItem(i));
		if (item->type == LevelReference)
		{
			for (int i = 0; i < item->childCount(); i++)
			{
				this->SetParentGuid(item->level, invalid, dynamic_cast<EntityTreeItem*>(item->child(i)));
			}			
		}
		else
		{
			this->SetParentGuid(Level::Instance()->GetName(), invalid, dynamic_cast<EntityTreeItem*>(this->topLevelItem(i)));

		}
		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::SetParentGuid(const Util::String & level, const Util::Guid & guid, EntityTreeItem * child)
{
    if (child)
    {
        Ptr<Game::Entity> entity = LevelEditor2EntityManager::Instance()->GetEntityById(child->GetEntityGuid());
        entity->SetGuid(Attr::ParentGuid, guid);
		entity->SetString(Attr::EntityLevel, level);
        Util::Guid itemGuid = entity->GetGuid(Attr::EntityGuid);
        for (int i = 0; i < child->childCount(); i++)
        {
            this->SetParentGuid(level, itemGuid, dynamic_cast<EntityTreeItem*>(child->child(i)));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
EntityTreeWidget::RebuildTree()
{	
	const Util::Array<Ptr<Game::Entity>> entities = BaseGameFeature::EntityManager::Instance()->GetEntities();	
	for(int i = 0 ; i < entities.Size() ; i++)
	{						
		if(entities[i].isvalid() && entities[i]->HasAttr(Attr::EntityType) && !BaseGameFeature::EntityManager::Instance()->IsEntityInDelayedJobs(entities[i]) && entities[i]->HasAttr(Attr::ParentGuid))
		{
			Util::Guid parentGuid = entities[i]->GetGuid(Attr::ParentGuid);
			if(parentGuid.IsValid())
			{
				Ptr<Game::Entity> parent = BaseGameFeature::EntityManager::Instance()->GetEntityByAttr(Attr::Attribute(Attr::EntityGuid,parentGuid));
				if(parent.isvalid())
				{
					EntityTreeItem* treeItem = dynamic_cast<EntityTreeItem*>(this->GetEntityTreeItem(entities[i]->GetGuid(Attr::EntityGuid)));				
					EntityTreeItem* parentTreeItem = dynamic_cast<EntityTreeItem*>(this->GetEntityTreeItem(parent->GetGuid(Attr::EntityGuid)));								
					int idx = this->indexOfTopLevelItem (treeItem);
					if (idx != -1)
					{
						this->takeTopLevelItem(idx);
					}
					else
					{
						idx = treeItem->parent()->indexOfChild(treeItem);
						treeItem->parent()->takeChild(idx);
					}					
					parentTreeItem->addChild(treeItem);
				}
			}
            else
            {
                Util::String entLevel = entities[i]->GetString(Attr::EntityLevel);
                if (entLevel != Level::Instance()->GetName() && !entLevel.IsEmpty())
                {
                    EntityTreeItem* treeItem = dynamic_cast<EntityTreeItem*>(this->GetEntityTreeItem(entities[i]->GetGuid(Attr::EntityGuid)));
                    EntityTreeItem * parentTreeItem = this->referenceItems[entLevel];
                    int idx = this->indexOfTopLevelItem(treeItem);
                    if (idx != -1)
                    {
                        this->takeTopLevelItem(idx);
                    }
                    else
                    {
                        idx = treeItem->parent()->indexOfChild(treeItem);
                        treeItem->parent()->takeChild(idx);
                    }
                    parentTreeItem->addChild(treeItem);
                }
            }
		}		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::OnBeginLoad()
{
	if (this->isSortingEnabled())
	{
		this->sortingEnabled = true;
		this->setSortingEnabled(false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::OnEndLoad()
{
	this->setSortingEnabled(this->sortingEnabled);
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::AddReference(const Util::String & name)
{
    if (!this->referenceItems.Contains(name))
    {
        EntityTreeItem * ent = new EntityTreeItem();
        ent->SetText(name);		
		ent->SetLevel(name);	
		ent->SetCategory("Level Reference");
        this->referenceItems.Add(name, ent);                
        ent->setFlags(Qt::ItemIsEnabled| Qt::ItemIsDropEnabled);
        this->insertTopLevelItem(0, ent);
		ent->SetIcon(LevelReference);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EntityTreeWidget::ClearReferences()
{
	for (int i = 0;i < this->referenceItems.Size();i++)
	{
		delete this->referenceItems.ValueAtIndex(i);
	}
    this->referenceItems.Clear();
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------