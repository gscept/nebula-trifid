//------------------------------------------------------------------------------
//  attributewidgetmanager.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attributewidgetmanager.h"
#include "db/valuetable.h"
#include "game/entity.h"
#include "attributecontrollerwidget.h"
#include "baseattributecontroller.h"
#include "widgets/entitytreewidget.h"
#include "actions/transformaction.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "editorfeatures/editorblueprintmanager.h"
#include "matrix44controller.h"
#include "entityutils/placementutil.h"
#include "basegamefeature/basegameprotocol.h"
#include "idldocument/idlattribute.h"
#include "qlabel.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "qgroupbox.h"
#include <QCheckBox>
#include "leveleditor2app.h"

using namespace Util;
using namespace Attr;
using namespace Game;
using namespace QtAttributeControllerAddon;
using namespace Math;

namespace LevelEditor2
{
__ImplementClass(AttributeWidgetManager, 'AWIM', Game::Manager);
__ImplementSingleton(AttributeWidgetManager);

//------------------------------------------------------------------------------
/**
*/
AttributeWidgetManager::AttributeWidgetManager():
	currentEntity(0),
	transformController(0)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AttributeWidgetManager::~AttributeWidgetManager()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeWidgetManager::OnActivate()
{
	n_assert(0 != this->container);

	Manager::OnActivate();	
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeWidgetManager::OnDeactivate()
{
	if (this->multiEntity.isvalid())
	{		
		this->multiEntity = 0;
	}
	this->container = 0;

	Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeWidgetManager::ViewEntityAttributes(const Ptr<Game::Entity>& entity)
{
	n_assert(!this->currentEntity.isvalid());
	this->currentEntity = entity;

	Ptr<Toolkit::EditorBlueprintManager> blueprintManager = Toolkit::EditorBlueprintManager::Instance();

	// must have a transform controller for each entity (an assertion is done at the end of this method)
	bool retrievedTransformController = false;
	int entityType = entity->GetInt(Attr::EntityType);
	if (entityType == Game || entityType == MultiSelection)
	{
		Util::String cat = entity->GetString(Attr::EntityCategory);
		QFont font;
		font.setPointSize(10);
		font.setBold(true);
		QLabel * entityLabel = new QLabel(cat.AsCharPtr());
		entityLabel->setFont(font);
		this->container->addWidget(entityLabel);
		Ptr<GetEntityValues> g = GetEntityValues::Create();
		entity->SendSync(g.cast<Messaging::Message>());		
		Util::Array<Attr::Attribute> attrs = g->GetAttrs().GetAttrs().ValuesAsArray();

		Util::Array<Util::String> propOrder;
		Util::Dictionary<Util::String, Util::Dictionary<Util::String,AttributeControllerWidget*>> propertyAttrs;
		Util::Dictionary<Util::String, AttributeControllerWidget*> sharedarr;
		propertyAttrs.Add("shared", sharedarr);
		propOrder.Append("shared");

		Array<String> entProps;
		if (blueprintManager->HasCategory(entity->GetString(Attr::EntityCategory)))
		{
			entProps = blueprintManager->GetCategoryProperties(entity->GetString(Attr::EntityCategory));
		}

		IndexT i;
		for(i = 0;i<attrs.Size();i++)
		{
			if (!blueprintManager->IsSystemAttribute(attrs[i].GetFourCC()))
			{				
				AttributeControllerWidget* attributeController;
												
				attributeController = new AttributeControllerWidget(entity, attrs[i].GetAttrId(),(Variant::Type)attrs[i].GetValueType());	

				if (attrs[i].GetAttrId() == Attr::Transform && entityType == MultiSelection)
				{
					attributeController->LockWidgets();
					this->transformController = dynamic_cast<Matrix44Controller*>(attributeController->GetControllerWidget());;
					retrievedTransformController = true;
				}
				else if (attrs[i].GetAttrId() == Attr::Graphics && entityType == MultiSelection)
				{
					attributeController->LockWidgets();
				}


				// if the attribute is read only, only view their values
				if (ReadOnly == attrs[i].GetAccessMode())
				{
					attributeController->LockWidgets();
				}
				String propID = "shared";

				Array<Util::String> props = blueprintManager->GetAttributeProperties(attrs[i].GetAttrId());
				int propCount = 0;
				String firstProp;
				for(IndexT j = 0 ; j < props.Size() ; j++)
				{
					for(IndexT k = 0 ; k < entProps.Size() ; k++)
					{
						if(props[j] == entProps[k])
						{
							propCount++;
							firstProp = props[j];
						}
					}
				}

				if(propCount == 1)
				{
					propID = firstProp;
				}
				
				if (!propertyAttrs.Contains(propID))
				{
					Util::Dictionary<Util::String,AttributeControllerWidget*> arr;
					propertyAttrs.Add(propID, arr);
					propOrder.Append(propID);
				}
				propertyAttrs[propID].Add(attrs[i].GetName(), attributeController);				
			}
		}

		// reorder property attributes according to nidl file order				
		// a bit of magic for the shared ones 
		QVBoxLayout * sharedAttrLayout = NULL;
		for (IndexT idx = 0; idx < propOrder.Size(); idx++)
		{
			const Util::String & propID(propOrder[idx]);
			Util::Dictionary<Util::String, AttributeControllerWidget*>& attrdict(propertyAttrs[propID]);

			QGroupBox * frame = new QGroupBox();
			frame->setCheckable(true);
			frame->setChecked(true);
			QVBoxLayout * layout = new QVBoxLayout(frame);
			if (propID != "shared")
			{
				frame->setTitle(propID.AsCharPtr());
			}
			else
			{
				frame->setTitle("Shared Attributes");
				sharedAttrLayout = layout;
			}
			QGroupBox * advanced = NULL;
			
						
			layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
			container->addWidget(frame);
			QObject::connect(frame, SIGNAL(toggled(bool)), LevelEditor2App::Instance()->GetWindow(), SLOT(OnCollapseProperty(bool)));
			
			if (blueprintManager->HasProperty(propID))
			{
				const Ptr<Tools::IDLProperty> & idl(blueprintManager->GetProperty(propID));
				const Util::Array<Util::String> & pattrs(idl->GetAttributes());
				for (int i = 0; i < pattrs.Size() ; i++)
				{
					const Util::String & str(pattrs[i]);
					if (attrdict.Contains(str))
					{
						const Ptr<Tools::IDLAttribute> & idlattr(blueprintManager->GetAttributeByName(str));
						if (idlattr->HasAttribute("advanced") && idlattr->GetAttribute("advanced").AsBool())
						{
							if (advanced == NULL)
							{
								advanced = new QGroupBox();
								advanced->setCheckable(true);
								advanced->setCheckable(true);
								QVBoxLayout * layoutAdvanced = new QVBoxLayout(advanced);
								advanced->setTitle("Advanced Attributes");								
								advanced->setAlignment(Qt::AlignLeft | Qt::AlignTop);
								QObject::connect(advanced, SIGNAL(toggled(bool)), LevelEditor2App::Instance()->GetWindow(), SLOT(OnCollapseProperty(bool)));
							}
							advanced->layout()->addWidget(attrdict[str]);
						}
						else
						{
							layout->addWidget(attrdict[str]);
						}						
						attrdict.Erase(str);
					}
				}
			}
			for (int i = 0; i < attrdict.Size(); i++)
			{
				layout->addWidget(attrdict.ValueAtIndex(i));
			}
			if (entityType == Game && LevelEditor2App::Instance()->HasPropertyCallbacks(propID))
			{
				const Util::Array<LevelEditor2App::PropertyCallbackEntry>& callbacks = LevelEditor2App::Instance()->GetPropertyCallbacks(propID);
				for (int i = 0; i < callbacks.Size(); i++)
				{
					QPushButton * button = new QPushButton(callbacks[i].displayName.AsCharPtr());
					button->setProperty("script", callbacks[i].scriptFunction.AsCharPtr());
					button->setProperty("entity", entity->GetUniqueId());
					QObject::connect(button, SIGNAL(clicked()), LevelEditor2App::Instance(), SLOT(PropertCallback()));
					layout->addWidget(button);
				}				
			}
			
			if (advanced != NULL)
			{
				layout->addWidget(advanced);
				advanced->setChecked(false);
			}
		}
		// insert the transform at the very beginning
		if (entityType != MultiSelection)
		{
			AttributeControllerWidget* attributeController = new AttributeControllerWidget(entity, Attr::Transform, Variant::Matrix44);
			Matrix44Controller* controller = dynamic_cast<Matrix44Controller*>(attributeController->GetControllerWidget());
			n_assert(0 != controller);
			this->transformController = controller;
			retrievedTransformController = true;			
			sharedAttrLayout->insertWidget(0, attributeController);
		}
	}
	else
	{
		Util::String cat;
		if(entity->GetInt(Attr::EntityType) == Light)
		{
			int lightType = entity->GetInt(Attr::LightType);
			switch(lightType)
			{
			case 0:
				cat = "Global Light";
				break;
			case 1:
				cat = "Spotlight";
				break;
			case 2:
				cat = "Pointlight";
				break;
			}			
		}else if(entity->GetInt(Attr::EntityType) == Group)
		{
			cat = "Group Node";
		}
        else if(entity->GetInt(Attr::EntityType) == NavMesh)
        {
            cat = "Navigation Mesh";
        }
		else
		{
			cat = "Environment Entity";
		}
		QFont font;
		font.setPointSize(10);
		font.setBold(true);
		QLabel * entityLabel = new QLabel(cat.AsCharPtr());
		entityLabel->setFont(font);
		this->container->addWidget(entityLabel);


		Ptr<Db::ValueTable> attrTable = entity->GetAttrTable();
		IndexT rowIndex = entity->GetAttrTableRowIndex();

		// hide everything for global light
		// FIXME ugly this way
		if(cat == "Global Light")
		{
			AttributeControllerWidget* attributeController = new AttributeControllerWidget(entity, Attr::Transform, Util::Variant::Matrix44);
			this->container->addWidget(attributeController);
			this->container->setAlignment(Qt::AlignLeft | Qt::AlignTop);
			retrievedTransformController = true;
			Matrix44Controller* controller = dynamic_cast<Matrix44Controller*>(attributeController->GetControllerWidget());
			n_assert(0 != controller);
			this->transformController = controller;
		}
		else
		{
			QGroupBox * advanced = NULL;

			IndexT i;
			for (i = 0; i < attrTable->GetNumColumns(); i++)
			{
				Variant::Type type = (Variant::Type)attrTable->GetColumnValueType(i);
				AttrId attrId = attrTable->GetColumnId(i);

				// if the attribute is not a system attribute, create a controller for it
				if (!blueprintManager->IsSystemAttribute(attrId.GetFourCC()))
				{
					AttributeControllerWidget* attributeController = new AttributeControllerWidget(entity, attrId, type);

					if (Attr::Transform.GetFourCC() == attrId.GetFourCC())
					{
						retrievedTransformController = true;
						Matrix44Controller* controller = dynamic_cast<Matrix44Controller*>(attributeController->GetControllerWidget());
						n_assert(0 != controller);
						this->transformController = controller;
					}

					// if the attribute is read only, only view their values
					if (ReadOnly == attrId.GetAccessMode())
					{
						attributeController->LockWidgets();
					}
					bool added = false;
					if (blueprintManager->HasAttributeByName(attrId.GetName()))
					{
						const Ptr<Tools::IDLAttribute> & idlattr(blueprintManager->GetAttributeByName(attrId.GetName()));
						if (idlattr->HasAttribute("advanced") && idlattr->GetAttribute("advanced").AsBool())
						{
							if (advanced == NULL)
							{
								advanced = new QGroupBox();
								advanced->setCheckable(true);
								advanced->setCheckable(true);
								QVBoxLayout * layoutAdvanced = new QVBoxLayout(advanced);
								advanced->setTitle("Advanced Attributes");
								advanced->setAlignment(Qt::AlignLeft | Qt::AlignTop);
								QObject::connect(advanced, SIGNAL(toggled(bool)), LevelEditor2App::Instance()->GetWindow(), SLOT(OnCollapseProperty(bool)));
							}
							advanced->layout()->addWidget(attributeController);
							added = true;
						}						
					}
					if (!added)
					{
						this->container->addWidget(attributeController);
					}					
					this->container->setAlignment(Qt::AlignLeft | Qt::AlignTop);
				}
			}
			if (advanced != NULL)
			{
				QVBoxLayout * layout = dynamic_cast<QVBoxLayout*>(this->container);
				layout->addWidget(advanced);
				advanced->setChecked(false);
			}
			if (LevelEditor2App::Instance()->HasPropertyCallbacks(cat))
			{
				const Util::Array<LevelEditor2App::PropertyCallbackEntry>& callbacks = LevelEditor2App::Instance()->GetPropertyCallbacks(cat);
				for (int i = 0; i < callbacks.Size(); i++)
				{
					QPushButton * button = new QPushButton(callbacks[i].displayName.AsCharPtr());
					button->setProperty("script", callbacks[i].scriptFunction.AsCharPtr());
					button->setProperty("entity", entity->GetUniqueId());
					QObject::connect(button, SIGNAL(clicked()), LevelEditor2App::Instance(), SLOT(PropertCallback()));
					this->container->addWidget(button);
				}
			}
		}
		
	}	
	QVBoxLayout * layout = dynamic_cast<QVBoxLayout*>(this->container);
	layout->addStretch();
	n_assert(retrievedTransformController);
}

//------------------------------------------------------------------------------
/**
*/
void
AttributeWidgetManager::ViewEntityAttributes(const Util::Array<Ptr<Game::Entity>>& entities)
{
	if (entities.Size() == 1)
	{
		this->ViewEntityAttributes(entities[0]);
	}
	else
	{
		if (!this->multiEntity.isvalid())
		{
			Util::Array<Attr::Attribute> attributes;
			attributes.Append(Attr::Attribute(Attr::EntityType, MultiSelection));
			attributes.Append(Attr::Attribute(Attr::EntityCategory, "Multiple Items"));
			this->multiEntity = BaseGameFeature::FactoryManager::Instance()->CreateEntityByAttrs("EditorMultiselect", attributes);
			BaseGameFeature::EntityManager::Instance()->AttachEntity(this->multiEntity);
		}
		Ptr<SetMultiSelection> smsg = SetMultiSelection::Create();
		smsg->SetEntities(SelectionUtil::Instance()->GetSelectedEntities());
		__SendSync(this->multiEntity, smsg);
		this->ViewEntityAttributes(this->multiEntity);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeWidgetManager::ClearAttributeControllers()
{
	this->currentEntity = 0;
	this->transformController = 0;

	QLayoutItem* item;

	while ( ( item = this->container->takeAt(0) ) != NULL )
	{
		delete item->widget();
		delete item;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AttributeWidgetManager::OnValueChanged(QtAttributeControllerAddon::BaseAttributeController* controller)
{
	bool callParentMethod = true;

	const FourCC attrFourCC = controller->GetAttributeId().GetFourCC();

	if (Attr::Transform.GetFourCC() == attrFourCC)
	{
		// handle this here, need to handle it through an undoable action
		callParentMethod = false;

		// create the new transform: get the position and scale from the controller, and the rotation from the previous matrix
		matrix44 posscaleMatrix = controller->GetValue().GetMatrix44();
		float4 position = posscaleMatrix.getrow0();
		float4 scale = posscaleMatrix.getrow1();
		float4 rot = posscaleMatrix.getrow2();
		quaternion qrot(rot);

		matrix44 prevTransform = controller->GetGameEntity()->GetMatrix44(Attr::Transform);
		
		matrix44 newTransform = matrix44::transformation(float4::zerovector(), quaternion::identity(), scale, float4::zerovector(), qrot, position);

		// create and perform the transformation action
		Util::Array<Ptr<Game::Entity>> entities;
		entities.Append(controller->GetGameEntity());
		Util::Array<matrix44> prevTransforms, newTransforms;
		prevTransforms.Append(prevTransform);
		newTransforms.Append(newTransform);

		Ptr<TransformAction> transformAction = TransformAction::Create();
		transformAction->SetEntitiesStart(entities, prevTransforms);
		transformAction->SetEntitiesEnd(newTransforms);
		ActionManager::Instance()->PerformAction(transformAction.upcast<Action>());

		// if the entity is currently selected, update the selection and placement utility
		IndexT selectionIndex = SelectionUtil::Instance()->GetIndexOfEntity(controller->GetGameEntity());
		if (InvalidIndex != selectionIndex)
		{
			//FIXME: update the placement utility
			PlacementUtil::Instance()->SetSelectionMatrix(selectionIndex, newTransform);
			PlacementUtil::Instance()->UpdateCurrentFeature();			
			//PlacementUtil::Instance()->ApplyTransformFeatureToMatrices();
		}
	}
	
	if (callParentMethod)
	{
		// call parent method: sets the value to the entity
		CallbackManager::OnValueChanged(controller);
	}
	
	if (Attr::Id.GetFourCC() == attrFourCC)
	{
		Ptr<BaseGameFeature::GetAttribute> msg = BaseGameFeature::GetAttribute::Create();
		msg->SetAttributeId(Attr::Id);
		controller->GetGameEntity()->SendSync(msg.cast<Messaging::Message>());
		Util::String newId = msg->GetAttr().GetString();

		controller->GetGameEntity()->SetString(Attr::Id,newId);
		Util::Guid guid = controller->GetGameEntity()->GetGuid(Attr::EntityGuid);
		EntityTreeItem* treeItem = this->treeWidget->GetEntityTreeItem(guid);
		treeItem->SetText(newId);
	}

	if (Attr::Graphics.GetFourCC() == attrFourCC)
	{
		// if the graphics have changed we have to trigger lots of refreshed (silhouette, attributes, etc)
		// set same selection again to trigger an update
		Util::Array<EntityGuid> ents = SelectionUtil::Instance()->GetSelectedEntityIds();
		SelectionUtil::Instance()->SetSelection(ents);

		this->ClearAttributeControllers();
		Util::Array<Ptr<Game::Entity>> realents = SelectionUtil::Instance()->GetSelectedEntities();
		this->ViewEntityAttributes(realents);
	}
}

}