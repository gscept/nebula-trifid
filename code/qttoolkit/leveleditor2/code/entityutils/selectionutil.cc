//------------------------------------------------------------------------------
//  entityseletionutil.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "selectionutil.h"
#include "managers/envquerymanager.h"
#include "input/inputserver.h"
#include "graphicsfeatureunit.h"
#include "physics/contact.h"
#include "physics/physicsserver.h"
#include "physics/physicsobject.h"
#include "game/entity.h"
#include "managers/entitymanager.h"
#include "input/mouse.h"
#include "coregraphics/shaperenderer.h"
#include "input/keyboard.h"
#include "messaging/message.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "debugrender/debugrender.h"
#include "debugrender/debugshaperenderer.h"
#include "coregraphics/primitivetopology.h"
#include "physicsfeature/physicsprotocol.h"
#include "physicsfeatureunit.h"
#include "coregraphics/rendershape.h"
#include "leveleditor2protocol.h"
#include "leveleditor2app.h"
#include "managers/attributewidgetmanager.h"
#include "graphics/graphicsprotocol.h"
#include "leveleditor2entitymanager.h"
#include "messaging/staticmessagehandler.h"
#include "messaging/messagecallbackhandler.h"
#include "properties/editorproperty.h"


using namespace Util;
using namespace BaseGameFeature;
using namespace Input;
using namespace Math;

//------------------------------------------------------------------------------
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::SelectionUtil,'LESU', Core::RefCounted);
__ImplementSingleton(LevelEditor2::SelectionUtil);

//------------------------------------------------------------------------------
/**
    Constructor
*/
SelectionUtil::SelectionUtil() :
    hasSelectionChanged(false),
	groupMode(false)
{
	__ConstructSingleton;

    this->keyMultiSelection = Input::Key::Shift;
	this->groupMatrix.identity();		
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
SelectionUtil::~SelectionUtil()
{
	__DestructSingleton;
	
}

//------------------------------------------------------------------------------
/**
    Checks the given input handler for relevant inputs.
*/
bool
SelectionUtil::HandleInput()
{
	const Ptr<Input::Mouse>& mouse = InputServer::Instance()->GetDefaultMouse();
	const Ptr<Input::Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();

    // for default selection has not changed
    this->hasSelectionChanged = false;
    
    // becomes true, when multi select button is pressed
    this->multiSelect = false;

    // Handle keyboard input, if a valid handler is available
    if (keyboard.isvalid())
    {
        if (keyboard->KeyPressed(this->keyMultiSelection))
        {
            this->multiSelect = true;
        }
    }

    // Handle mouse input, if a valid handler is available
    if (mouse.isvalid())
    {
        if (mouse->ButtonDown(Input::MouseButton::LeftButton))
        {
            this->GetEntityUnderMouse();
			return true;
        }
    }
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectionUtil::OnEntityClicked(const Ptr<Messaging::Message>& msg)
{
	const Ptr<Graphics::ItemAtPosition>& rMsg = msg.downcast<Graphics::ItemAtPosition>();
	const Ptr<Game::Entity>& entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(rMsg->GetItem());
	if(entity.isvalid())
	{
		if (LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->IsEntityInLockedLayer(entity))
		{
			return;
		}

		int entityIndexInSelectedList = this->selectedEntities.FindIndex(entity->GetGuid(Attr::EntityGuid));
		if(entityIndexInSelectedList!=InvalidIndex)
		{	
			if(multiSelect)
			{
				Ptr<SelectAction> action = SelectAction::Create();
				action->SetSelectionMode(SelectAction::RemoveSelection);
				action->SetEntity(entity->GetGuid(Attr::EntityGuid));
				ActionManager::Instance()->PerformAction(action.cast<Action>());						
				return;
			}

			if (this->selectedEntities.Size() == 1)
			{
				return;
			}
			else
			{
				Ptr<SelectAction> action = SelectAction::Create();
				action->SetSelectionMode(SelectAction::SetSelection);
				action->SetEntity(entity->GetGuid(Attr::EntityGuid));
				ActionManager::Instance()->PerformAction(action.cast<Action>());
			}
		}
		else if(!this->multiSelect)
		{
			Ptr<SelectAction> action = SelectAction::Create();
			action->SetSelectionMode(SelectAction::SetSelection);
			action->SetEntity(entity->GetGuid(Attr::EntityGuid));
			ActionManager::Instance()->PerformAction(action.cast<Action>());
		}
		else
		{
			Ptr<SelectAction> action = SelectAction::Create();
			action->SetSelectionMode(SelectAction::AppendSelection);
			action->SetEntity(entity->GetGuid(Attr::EntityGuid));
			ActionManager::Instance()->PerformAction(action.cast<Action>());		
		}

	}
	else if (!this->multiSelect && this->selectedEntities.Size()>0)
	{
		Ptr<SelectAction> action = SelectAction::Create();
		action->SetSelectionMode(SelectAction::ClearSelection);						
		ActionManager::Instance()->PerformAction(action.cast<Action>());
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
SelectionUtil::HasSelectionChanged()
{
    return this->hasSelectionChanged;
}

//------------------------------------------------------------------------------
/**
    Returns an array of entities that were selected by the last input
    of the user.
*/
Util::Array<Ptr<Game::Entity> >
SelectionUtil::GetSelectedEntities()
{    
	Util::Array<Ptr<Game::Entity>> ents;
	for(IndexT i = 0 ; i < this->selectedEntities.Size() ; i++)
	{
		ents.Append(LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]));
	}
	return ents;
}

//------------------------------------------------------------------------------
/**
    Appends the given entity to the current selection.
*/
void
SelectionUtil::AppendToSelection(const EntityGuid& entity)
{    
	if(InvalidIndex == this->selectedEntities.FindIndex(entity))
	{
		this->selectedEntities.Append(entity);			
	}	
	this->hasSelectionChanged = true;
}

//------------------------------------------------------------------------------
/**
    Appends the given entity array to the current selection.
*/
void
SelectionUtil::AppendToSelection(const Util::Array<EntityGuid>& entities)
{			
	for(IndexT i = 0 ; i < entities.Size() ; i++)
	{
		this->AppendToSelection(entities[i]);
	}		
}

//------------------------------------------------------------------------------
/**
	Removes an entity
*/
void SelectionUtil::RemoveFromSelection(const EntityGuid& entity)
{
	this->hasSelectionChanged = true;
	IndexT id = this->selectedEntities.FindIndex(entity);
	if(InvalidIndex != id)
	{
		this->selectedEntities.EraseIndex(id);
	}
		
}

//------------------------------------------------------------------------------
/**
	Removes an array of entities
*/
void SelectionUtil::RemoveFromSelection(const Util::Array<EntityGuid>& entities)
{
	for(IndexT i = 0 ; i < entities.Size() ; i++)
	{
		RemoveFromSelection(entities[i]);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
SelectionUtil::SetSelection(const Util::Array<EntityGuid>& entities)
{
	this->selectedEntities.Clear();
	this->hasSelectionChanged = true;
	this->selectedEntities.AppendArray(entities);		
}

//------------------------------------------------------------------------------
/**
Clears the whole selection
*/
void
SelectionUtil::ClearSelection()
{    
	this->selectedEntities.Clear();
	this->hasSelectionChanged = true;
	Silhouette::SilhouetteAddon::Instance()->SetModels(Util::Array<Ptr<Graphics::ModelEntity>>());
}

//------------------------------------------------------------------------------
/**
    Returns the entity that is under the mouse on the screen.
*/
void
SelectionUtil::GetEntityUnderMouse()
{    
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();

	float2 mousePos = InputServer::Instance()->GetDefaultMouse()->GetPixelPosition();

	Ptr<Graphics::ItemAtPosition> msg = Graphics::ItemAtPosition::Create();
	msg->SetPosition(mousePos);
	__StaticSend(Graphics::GraphicsInterface, msg);	
	__SingleFireCallback(SelectionUtil, OnEntityClicked, this, msg.upcast<Messaging::Message>());

#ifdef NO_GFX_PICKING
	float length = 10000000.0f;
	line worldRay = envQueryManager->ComputeMouseWorldRay(mousePos, length, GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView());

	// do the actual picking
	Physics::FilterSet mouseExcludeSet;
	mouseExcludeSet.SetCollideBits(Physics::None);
	Ptr<Physics::Contact> contact;
	contact = Physics::PhysicsServer::Instance()->GetScene()->GetClosestContactUnderMouse(worldRay, mouseExcludeSet);
	if (contact.isvalid())
	{
		if(contact->GetCollisionObject()->GetUserData()->object->IsA(Game::Entity::RTTI))
		{
			return contact->GetCollisionObject()->GetUserData()->object.cast<Game::Entity>();
		}
	}
   
	return Ptr<Game::Entity>();
#endif
}

//------------------------------------------------------------------------------
/**
	Render a simple representation of the selection	
*/
void
SelectionUtil::Render()
{
    Util::Array<Ptr<Graphics::ModelEntity>> modelEntities;
    Ptr<GraphicsFeature::GetModelEntity> msg;

	float4 color = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
	color.set_w(1);
	Silhouette::SilhouetteAddon::Instance()->SetColor(color);
    IndexT i;

	bbox boundingBox;
	boundingBox.begin_extend();
    for(i = 0; i < this->selectedEntities.Size(); i++)
    {
        // try to get the bounding box of the entity if one exist
        // (only if it has the graphics property)
		const Ptr<Game::Entity>& selectedEntity = LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]);

        msg = GraphicsFeature::GetModelEntity::Create();
		__SendSync(selectedEntity, msg);
		const Ptr<Graphics::ModelEntity>& model = msg->GetEntity();
		if (model.isvalid())
		{
			boundingBox.extend(model->GetGlobalBoundingBox());
			modelEntities.Append(model);
		}
		else
		{
			// alternative solution, in case the entity have a model, we just make one up!
			matrix44 trans = selectedEntity->GetMatrix44(Attr::Transform);
			bbox box(trans);
			boundingBox.extend(box);
		}

		/*
		const Ptr<Game::Entity>& entity = LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]);
		if (modelEntity.isvalid())
		{
			bbox _box = modelEntity->GetLocalBoundingBox();
			_box.transform(entity->GetMatrix44(Attr::Transform));
			//this->RenderBBox(_box);
		}		
		*/
    }

	boundingBox.end_extend();
	this->groupBox = boundingBox;

	// set models to be rendered
	Silhouette::SilhouetteAddon::Instance()->SetModels(modelEntities);

	// calculate group box for entities
	//this->CalculateGroupBox();

	// Renders a single bounding box around the selected entities.
	if(this->groupMode)
	{
		// Render the box
		this->RenderBBox(this->groupBox);
	}
}
//------------------------------------------------------------------------------
/**
    Renders a bounding box	
*/
void
SelectionUtil::RenderBBox(const bbox & origBox)
{

    const int linecount = 24;
    vector lines[linecount*2];
    vector corner;
    bbox box = origBox;
    box.transform(matrix44::scaling(float4(1.01)));

	// set extents, but also ensure that no value is 0 so as to avoid Z-buffer fights
	box.set(origBox.center(), float4::maximize(box.extents(), vector(0.05f, 0.05f, 0.05f)));
    
    // The pattern describes the layout of the lines, that creates the corners
    // of the bounding box.
    static int pattern[] = 
    {
         1, 1, 1,
         1,-1, 1,
        -1,-1, 1,
        -1, 1, 1,
        -1,-1,-1, 
         1,-1,-1,                
         1, 1,-1,
        -1, 1,-1        
    };
    
    // The size of the bounding box is used,
    // to define the size of the corners.
    vector cornerSize = box.size() * 0.3f;

    // for each of the 8 corners create 3 small lines
    IndexT c;
    for (c = 0; c < 8; c++)
    {
        corner = box.corner_point(c);
        
        lines[c*6] = corner;
		lines[c*6+1] = corner+vector(1,0,0) * (float)pattern[c*3] * cornerSize.x();
        lines[c*6+2] = corner;
        lines[c*6+3] = corner+vector(0,1,0) * (float)pattern[c*3+1] * cornerSize.y();
        lines[c*6+4] = corner;
        lines[c*6+5] = corner+vector(0,0,1) * (float)pattern[c*3+2] * cornerSize.z();

    }

	float4 selColour = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
	float4 selLine = selColour;
	selLine.set_w(1.0f);
    // render lines
	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(
														matrix44::identity(),
														CoreGraphics::PrimitiveTopology::LineList,
														linecount,
														lines,
														4,
														selLine, 
														CoreGraphics::RenderShape::CheckDepth);

    const int pointcount = 8;
    vector points[pointcount];
    for (c = 0; c < 8; c++)
    {
        points[c] = box.corner_point(c);
    }

    const int indexcount = 36;
    int indices[indexcount] =
    {
        0, 1, 2, // front
        2, 3, 0, // front
        4, 5, 6, // back
        6, 7, 4, // back
        1, 2, 4, // top
        4, 5, 1, // top
        0, 3, 6, // bottom
        6, 7, 3, // bottom
        0, 1, 5, // left
        5, 6, 0, // left
        2, 3, 4, // right
        4, 7, 3  // right 
    };

    // render cube
    Debug::DebugShapeRenderer::Instance()->DrawIndexedPrimitives(
                                                matrix44::identity(), 
                                                CoreGraphics::PrimitiveTopology::TriangleList,
                                                12,
                                                points,
                                                pointcount,
                                                4,
                                                indices,
                                                CoreGraphics::IndexType::Index32,
                                                selColour, 
                                                CoreGraphics::RenderShape::CheckDepth);
    // 
    
// 	const Math::point& center = box.center();
// 	const Math::vector& extends = box.extents();    
// 	const Math::vector corners[24] = {  vector(1,1,1),
// 		vector(1,1,-1),
// 		vector(1,1,-1),
// 		vector(-1,1,-1),
// 		vector(-1,1,-1),
// 		vector(-1,1,1),
// 		vector(-1,1,1),
// 		vector(1,1,1),
// 
// 		vector(1,-1,1),
// 		vector(1,-1,-1),
// 		vector(1,-1,-1),
// 		vector(-1,-1,-1),
// 		vector(-1,-1,-1),
// 		vector(-1,-1,1),
// 		vector(-1,-1,1),
// 		vector(1,-1,1),
// 
// 		vector(1,1,1),
// 		vector(1,-1,1),
// 		vector(1,1,-1),
// 		vector(1,-1,-1),
// 		vector(-1,1,-1),
// 		vector(-1,-1,-1),
// 		vector(-1,1,1),
// 		vector(-1,-1,1)};
// 
// 	Util::Array<point> lineList;
// 	IndexT i;
// 	for (i = 0; i < 24; ++i)
// 	{
// 		lineList.Append(center + float4::multiply(extends, corners[i]));    	
// 	}
// 
// 	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(
// 		matrix44::identity(),
// 		CoreGraphics::PrimitiveTopology::LineList,
// 		lineList.Size()/2,
// 		&(lineList.Front()),
// 		4,
// 		float4(1.0f,0.5f,0.0f,0.5f));

	//Debug::DebugShapeRenderer::Instance()->DrawPrimitives(m,CoreGraphics::PrimitiveTopology::LineList,linecount,lines,3,color);

   //nGfxServer2::Instance()->DrawShapePrimitives(nGfxServer2::LineList,linecount,lines,3,m,color);
}

//------------------------------------------------------------------------------
/**
    Toggles group selection.	
*/
void
SelectionUtil::ToggleGroup()
{
	groupMode = !groupMode;
}

//------------------------------------------------------------------------------
/**
    Calculates group bounding box	
*/
void
SelectionUtil::CalculateGroupBox()
{
	// Don't calculate if nothing is selected
	if(this->selectedEntities.IsEmpty() && !this->groupMode)
		return;

	// Init
	bbox boundingBox;
	SizeT size = this->selectedEntities.Size();
	Util::Array<Ptr<Graphics::ModelEntity>> gfxEntities;
	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
    
	// Get all graphic entities
	for(int i = 0; i < size; i++)
	{
		Ptr<Game::Entity> entity = LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]);
		__SendSync(entity, msg);		
        const Ptr<Graphics::ModelEntity>& modelEntity = msg->GetEntity();
        if (modelEntity.isvalid())
        {
            gfxEntities.Append(msg->GetEntity());
        }        
	}

    // begin bounding box extend
    boundingBox.begin_extend();

	// Check if we got any graphics entities
	if( !gfxEntities.IsEmpty() )
    {
        for( int i = 0; i < gfxEntities.Size(); i++ )
        {
            boundingBox.extend(gfxEntities[i]->GetGlobalBoundingBox());
        }
    }
    else
    {
        // extend bounding box by all childrens position
        // since we have no bounding boxes (due to us having no models) we just make a bounding box with center at the object and extents (1,1,1)
        for(int i = 0; i < size; i++)
        {
            const Ptr<Game::Entity>& entity = LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]);            
            bbox entityBox;
            entityBox.set(entity->GetMatrix44(Attr::Transform).get_position(), vector(1,1,1));
            boundingBox.extend(entityBox);
        }     
    }

    // finish extending
    boundingBox.end_extend();   

	// Create group matrix
	Math::matrix44 newGroupMatrix;
	newGroupMatrix.identity();
	newGroupMatrix.set_position(boundingBox.center());

	// Set calculated values
	this->groupBox = boundingBox;
	this->groupMatrix = newGroupMatrix;
	this->entityGroup = this->selectedEntities;
}

//------------------------------------------------------------------------------
/**
    Get group box	
*/
Math::bbox&
SelectionUtil::GetGroupBox()
{
	return groupBox;
}

//------------------------------------------------------------------------------
/**
    Get group matrix	
*/
Math::matrix44& 
SelectionUtil::GetGroupMatrix()
{
	return groupMatrix;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Game::Entity> 
SelectionUtil::GetSelectedEntityAtIndex(IndexT index)
{
	n_assert(index >= 0);
	n_assert(index < this->selectedEntities.Size());

	return LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[index]);
}

//------------------------------------------------------------------------------
/**
*/
bool 
SelectionUtil::IsSelected(const Ptr<Game::Entity>& entity)
{
	return InvalidIndex != this->selectedEntities.FindIndex(entity->GetGuid(Attr::EntityGuid));
}


//------------------------------------------------------------------------------
/**
*/
IndexT 
SelectionUtil::GetIndexOfEntity(const Ptr<Game::Entity> entity)
{
	return this->selectedEntities.FindIndex(entity->GetGuid(Attr::EntityGuid));
}

}// namespace LevelEditor

