//------------------------------------------------------------------------------
//  entityseletionutil.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
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
#include "leveleditor2/leveleditor2protocol.h"
#include "leveleditor2app.h"
#include "managers/attributewidgetmanager.h"
#include "graphics/graphicsprotocol.h"
#include "leveleditor2entitymanager.h"
#include "messaging/staticmessagehandler.h"
#include "messaging/messagecallbackhandler.h"
#include "properties/editorproperty.h"
#include "imgui.h"

static const int MarqueeSize = 16;
using namespace Util;
using namespace BaseGameFeature;
using namespace Input;
using namespace Math;
using namespace Attr;

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
	mouseDrag(false),
	marqueeSelecting(false),
	keyMultiSelection(Input::Key::Shift),
	keyMultiSelectionRemove(Input::Key::LeftControl),
	selectInside(true)
{
	__ConstructSingleton;    		
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

	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse = false;
	io.WantCaptureKeyboard = false;

    // Handle mouse input, if a valid handler is available
    if (mouse.isvalid())
    {
		Math::float2 curPos = mouse->GetPixelPosition();
        if (mouse->ButtonDown(Input::MouseButton::LeftButton))
        {
			this->mouseDrag = true;
			this->clickPos = curPos;			
        }
		if (mouse->ButtonPressed(Input::MouseButton::LeftButton))
		{
			this->marqueeSelecting = ((curPos - this->clickPos).lengthsq() > MarqueeSize);
		}
		if (mouse->ButtonUp(Input::MouseButton::LeftButton))
		{
			if (this->mouseDrag)
			{
				ImGuiIO& io = ImGui::GetIO();
				this->mouseDrag = false;
				if (!this->marqueeSelecting)
				{
					this->GetEntityUnderMouse();
				}
				else
				{
					Math::rectangle<float> selection;
					selection.top = n_min(curPos.y(), this->clickPos.y());
					selection.bottom = n_max(curPos.y(), this->clickPos.y());
					selection.left = n_min(curPos.x(), this->clickPos.x());
					selection.right = n_max(curPos.x(), this->clickPos.x());
					if (curPos.y() < this->clickPos.y())
					{
						this->selectInside = false;
					}
					else
					{
						this->selectInside = true;
					}
					Ptr<Graphics::ItemsAtPosition> msg = Graphics::ItemsAtPosition::Create();
					msg->SetRectangle(selection);
					__StaticSend(Graphics::GraphicsInterface, msg);
					__SingleFireCallback(SelectionUtil, OnEntitiesClicked, this, msg.upcast<Messaging::Message>());
				}
				this->marqueeSelecting = false;
			}
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
        if (entity->GetBool(Attr::IsLocked))
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
void
SelectionUtil::OnEntitiesClicked(const Ptr<Messaging::Message>& msg)
{
	const Ptr<Graphics::ItemsAtPosition>& rMsg = msg.downcast<Graphics::ItemsAtPosition>();
	Util::Array<int> ids = rMsg->GetItems();	
	Util::Array<int> edgeIds = rMsg->GetEdgeItems();
	edgeIds.Sort();
	if (ids.Size() > 0)
	{
		if (this->multiSelect)
		{
			Util::Array<EntityGuid> current = this->GetSelectedEntityIds();
			current.Sort();
			Util::Array<EntityGuid> entities;
			Util::Array<EntityGuid> containedEntities;
			for (int i = 0; i < ids.Size();i++)
			{
				if (BaseGameFeature::EntityManager::Instance()->ExistsEntityByUniqueId(ids[i]))
				{
					if (BaseGameFeature::EntityManager::Instance()->ExistsEntityByUniqueId(ids[i]))
					{
						if (!this->selectInside || (this->selectInside && InvalidIndex == edgeIds.BinarySearchIndex(ids[i])))
						{
                            Ptr<Game::Entity> ent = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(ids[i]);
                            if (ent->GetBool(Attr::IsLocked) || LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->IsEntityInLockedLayer(ent))
                            {
                                continue;
                            }
							EntityGuid guid = ent->GetGuid(Attr::EntityGuid);
                            
							if (current.BinarySearchIndex(guid) == InvalidIndex)
							{
								entities.Append(guid);
							}
							else
							{
								containedEntities.Append(guid);
							}
						}
					}
				}
			}
			if (InputServer::Instance()->GetDefaultKeyboard()->KeyPressed(this->keyMultiSelectionRemove))
			{
				if (!containedEntities.IsEmpty())
				{
					Ptr<SelectAction> action = SelectAction::Create();
					action->SetSelectionMode(SelectAction::RemoveSelection);
					action->SetEntities(containedEntities);
					ActionManager::Instance()->PerformAction(action.cast<Action>());
				}
			}
			else
			{
				if (!entities.IsEmpty())
				{
					Ptr<SelectAction> action = SelectAction::Create();
					action->SetSelectionMode(SelectAction::AppendSelection);
					action->SetEntities(entities);
					ActionManager::Instance()->PerformAction(action.cast<Action>());
				}
			}
		}
		else
		{
			Ptr<SelectAction> action = SelectAction::Create();
			action->SetSelectionMode(SelectAction::ClearSelection);
			ActionManager::Instance()->PerformAction(action.cast<Action>());
			Util::Array<EntityGuid> entities;
			for (int i = 0; i < ids.Size();i++)
			{
				if (BaseGameFeature::EntityManager::Instance()->ExistsEntityByUniqueId(ids[i]))
				{
                    Ptr<Game::Entity> ent = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(ids[i]);
                    if (ent->GetBool(Attr::IsLocked) || LevelEditor2App::Instance()->GetWindow()->GetLayerHandler()->IsEntityInLockedLayer(ent))
                    {
                        continue;
                    }
					if (!this->selectInside || (this->selectInside && InvalidIndex == edgeIds.BinarySearchIndex(ids[i])))
					{
						entities.Append(BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(ids[i])->GetGuid(Attr::EntityGuid));
					}					
				}
			}
			if (!entities.IsEmpty())
			{
				action->SetSelectionMode(SelectAction::SetSelection);
				action->SetEntities(entities);
				ActionManager::Instance()->PerformAction(action.cast<Action>());
			}
		}
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
Util::Array<Ptr<Game::Entity>>
SelectionUtil::GetSelectedEntities(bool withChildren)
{    
    if (withChildren)
    {
        return LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->GetSelectionRecursive();        
    }
    else
    {
        Util::Array<Ptr<Game::Entity>> ents;
        for (IndexT i = 0; i < this->selectedEntities.Size(); i++)
        {
            ents.Append(LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]));
        }
        return ents;
    }	
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
	this->UpdateModels();
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
	this->UpdateModels();
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
	this->UpdateModels();
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
	this->UpdateModels();
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
	this->UpdateModels();
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
	this->UpdateModels();
}

//------------------------------------------------------------------------------
/**
    Returns the entity that is under the mouse on the screen.
*/
void
SelectionUtil::GetEntityUnderMouse()
{    
	Ptr<Graphics::ItemAtPosition> msg = Graphics::ItemAtPosition::Create();
	msg->SetPosition(this->clickPos);
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
#define swap(x, y) { float c = x; x = y; y = c; }
	// if dragging mouse draw a rectangle
	if (this->mouseDrag)
	{
		Math::float2 curPos = InputServer::Instance()->GetDefaultMouse()->GetPixelPosition();
		float top = n_min(curPos.y(), this->clickPos.y());
		float bottom = n_max(curPos.y(), this->clickPos.y());
		float left = n_min(curPos.x(), this->clickPos.x());
		float right = n_max(curPos.x(), this->clickPos.x());
		float width = abs(left - right);
		float height = abs(bottom - top);

		Math::float4 selectionColor = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
		if (this->marqueeSelecting)
		{			
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(selectionColor.x(), selectionColor.y(), selectionColor.z(), 1.0f));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(selectionColor.x(), selectionColor.y(), selectionColor.z(), 0.5f));
			ImGui::SetNextWindowPos(ImVec2(left, top));
			ImGui::SetNextWindowSize(ImVec2(width, height));
			ImGui::Begin("_selection", NULL, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders);
			ImGui::End();
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(2);
		}						
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
    CoreGraphics::RenderShape::RenderShapeVertex lines[linecount * 2];
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
        
        lines[c*6].pos = corner;
		lines[c*6+1].pos = corner+vector(1,0,0) * (float)pattern[c*3] * cornerSize.x();
        lines[c*6+2].pos = corner;
        lines[c*6+3].pos = corner+vector(0,1,0) * (float)pattern[c*3+1] * cornerSize.y();
        lines[c*6+4].pos = corner;
        lines[c*6+5].pos = corner+vector(0,0,1) * (float)pattern[c*3+2] * cornerSize.z();

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
														selLine, 
														CoreGraphics::RenderShape::CheckDepth);

    const int pointcount = 8;
    CoreGraphics::RenderShape::RenderShapeVertex points[pointcount];
    for (c = 0; c < 8; c++)
    {
        points[c].pos = box.corner_point(c);
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
    Calculates entity bounding box
*/
Math::bbox
SelectionUtil::CalculateGroupBox(const Util::Array<EntityGuid>& entities)
{
	// Init
	bbox boundingBox;
	SizeT size = entities.Size();
	Util::Array<Ptr<Graphics::ModelEntity>> gfxEntities;
	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
    
	// Get all graphic entities
	for(int i = 0; i < size; i++)
	{
		Ptr<Game::Entity> entity = LevelEditor2EntityManager::Instance()->GetEntityById(entities[i]);
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
            const Ptr<Game::Entity>& entity = LevelEditor2EntityManager::Instance()->GetEntityById(entities[i]);
            bbox entityBox;
            entityBox.set(entity->GetMatrix44(Attr::Transform).get_position(), vector(1,1,1));
            boundingBox.extend(entityBox);
        }     
    }

    // finish extending
    boundingBox.end_extend();   
	return boundingBox;	
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

//------------------------------------------------------------------------------
/**
*/
void
SelectionUtil::UpdateModels()
{
	Util::Array<Ptr<Graphics::ModelEntity>> selectedModels;			
	for (IndexT i = 0; i < this->selectedEntities.Size(); i++)
	{				
		const Ptr<Game::Entity>& selectedEntity = LevelEditor2EntityManager::Instance()->GetEntityById(this->selectedEntities[i]);
		Ptr<GraphicsFeature::GetModelEntity> msg;
		msg = GraphicsFeature::GetModelEntity::Create();
		__SendSync(selectedEntity, msg);
		const Ptr<Graphics::ModelEntity>& model = msg->GetEntity();
		if (model.isvalid())
		{
			selectedModels.Append(model);
		}			
	}
	float4 color = LevelEditor2App::Instance()->GetWindow()->GetSelectionColour();
	color.set_w(1);

	// set models to be rendered
	Silhouette::SilhouetteAddon::Instance()->SetModels("primary", selectedModels, color);

	Util::Array<Ptr<Graphics::ModelEntity>> childModels;
	Util::Array<Ptr<Game::Entity>> allEntities = this->GetSelectedEntities(true);
	for (IndexT i = 0; i < allEntities.Size();i++)
	{
		if (this->selectedEntities.FindIndex(allEntities[i]->GetGuid(Attr::EntityGuid)) == InvalidIndex)
		{
			Ptr<GraphicsFeature::GetModelEntity> msg;
			msg = GraphicsFeature::GetModelEntity::Create();
			__SendSync(allEntities[i], msg);
			const Ptr<Graphics::ModelEntity>& model = msg->GetEntity();
			if (model.isvalid())
			{
				childModels.Append(model);
			}
		}
	}	
	Silhouette::SilhouetteAddon::Instance()->SetModels("secondary", childModels, float4(1.0f));
}

}// namespace LevelEditor

