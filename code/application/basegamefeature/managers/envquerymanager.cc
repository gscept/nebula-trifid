//------------------------------------------------------------------------------
//  managers/envquerymanager.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/envquerymanager.h"
#include "managers/entitymanager.h"
#include "input/inputserver.h"
#include "input/mouse.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "renderutil/mouserayutil.h"
#include "physics/physicsserver.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

namespace BaseGameFeature
{
__ImplementClass(EnvQueryManager, 'EVQM', Game::Manager);
__ImplementSingleton(EnvQueryManager);

using namespace Game;
using namespace Input;
using namespace Math;
using namespace RenderUtil;

//------------------------------------------------------------------------------
/**
*/
EnvQueryManager::EnvQueryManager() :
    entityUnderMouse(0),
    mouseIntersection(false),
	mouseTracking(false)
//    ,materialUnderMouse(Physics::InvalidMaterial)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
EnvQueryManager::~EnvQueryManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
EnvQueryManager::OnActivate()
{
    Manager::OnActivate();

    // setup exclude set for mouse queries
//	Physics::MaterialType probeType = Physics::MaterialTable::StringToMaterialType("Probe");
  //  this->mouseExcludeSet.AddMaterialType(probeType);
	this->leaveMessage = GraphicsFeature::MouseLeave::Create();
	this->enterMessage = GraphicsFeature::MouseEnter::Create();
}

//------------------------------------------------------------------------------
/**
*/
void
EnvQueryManager::OnDeactivate()
{    
    this->mouseExcludeSet.Clear();
	this->leaveMessage = 0;
	this->enterMessage = 0;
    Manager::OnDeactivate();
}


//------------------------------------------------------------------------------
/**
	get a const Ref on the MouseExcludeSet
*/
const Physics::FilterSet&
EnvQueryManager::GetMouseExcludeSet() const
{
 	return this->mouseExcludeSet;
}

//------------------------------------------------------------------------------
/**
	set a new MouseExcludeSet
*/
void
EnvQueryManager::SetMouseExcludeSet( const Physics::FilterSet& mouseExcludeSet )
{
     this->mouseExcludeSet = mouseExcludeSet;
}

//------------------------------------------------------------------------------
/**
    This returns a pointer to the entity under the mouse, or 0 if mouse is 
    not over an entity.
*/
Entity*
EnvQueryManager::GetEntityUnderMouse() const
{
     return EntityManager::Instance()->GetEntityByUniqueId(this->entityUnderMouse).get_unsafe();
}

//------------------------------------------------------------------------------
/**
    This returns the MaterialType of the entity under the mouse, or 
    InvalidMaterial if mouse is not over an entity.
*/
#if 0
Physics::MaterialType
EnvQueryManager::GetMaterialUnderMouse() const
{
    return this->materialUnderMouse;
}
#endif

//------------------------------------------------------------------------------
/**
    This returns the position where a vector through the mouse position 
    intersects the 3d world (or the nearest entity). If the mouse doesn't
    intersect, the result will be undefined, and the method
    HasMouseIntersection() returns false.
*/
const point&
EnvQueryManager::GetMousePos3d() const
{
    return this->mousePos3d;
}

//------------------------------------------------------------------------------
/**
    This returns the upvector of the face under the mousecursor.
    If the mouse doesn't intersect, the result will be undefined, 
    and the method HasMouseIntersection() returns false.
*/
const vector&
EnvQueryManager::GetUpVector() const
{
    return this->upVector;
}

//------------------------------------------------------------------------------
/**
    Returns true if the vector through the current mouse position intersects
    the world, or an entity, false if no intersection exists.
*/
bool
EnvQueryManager::HasMouseIntersection() const
{
    return this->mouseIntersection;
}

//------------------------------------------------------------------------------
/**
    Returns all game entities which intersect the given sphere. Uses the
    physics subsystem to do the query.
*/
Util::Array<Ptr<Game::Entity> >
EnvQueryManager::GetEntitiesInSphere(const point& midPoint, float radius)
{
    Util::Array<Ptr<Game::Entity> > gameEntities;

    Physics::FilterSet excludeSet;
    Util::Array<Ptr<Physics::PhysicsObject> > physicsEntities;
    Physics::PhysicsServer::Instance()->GetScene()->GetObjectsInSphere(midPoint, radius, excludeSet, physicsEntities);
    
    // convert physics entities back into game entities
    EntityManager* entityManager = EntityManager::Instance();
    int i;
    int num = physicsEntities.Size();
    for (i = 0; i < num; i++)
    {
		if(physicsEntities[i]->GetUserData()->object.isvalid() && physicsEntities[i]->GetUserData()->object->IsA(Game::Entity::RTTI))
		{			
			gameEntities.Append(physicsEntities[i]->GetUserData()->object.cast<Game::Entity>());
		}
    }

    return gameEntities;
}
//------------------------------------------------------------------------------
/**
    Returns all game entities which intersect the given box. Uses the
    physics subsystem to do the query.
*/
Util::Array<Ptr<Game::Entity> >
EnvQueryManager::GetEntitiesInBox(const vector& scale, const matrix44& m)
{
    Util::Array<Ptr<Game::Entity> > gameEntities;

    Physics::FilterSet excludeSet;
    Util::Array<Ptr<Physics::PhysicsObject> > physicsEntities;
    Physics::PhysicsServer::Instance()->GetScene()->GetObjectsInBox(m, scale, excludeSet, physicsEntities);
    
    // convert physics entities back into game entities
    EntityManager* entityManager = EntityManager::Instance();
    int i;
    int num = physicsEntities.Size();
    for (i = 0; i < num; i++)
    {     
		if(physicsEntities[i]->GetUserData()->object.isvalid() && physicsEntities[i]->GetUserData()->object->IsA(Game::Entity::RTTI))
        {			
			gameEntities.Append(physicsEntities[i]->GetUserData()->object.cast<Game::Entity>());     
        }
    }

    return gameEntities;
}
//------------------------------------------------------------------------------
/**
    This method is called per-frame by the game server and updates the
    current values, like entity-under-mouse, 3d mouse position, etc...
*/
void
EnvQueryManager::OnFrame()
{
	// save last entity
	Game::Entity::EntityId lastEntity = this->entityUnderMouse;

	// reset values
	this->entityUnderMouse = 0;
	this->mousePos3d.set(0.0f, 0.0f, 0.0f);
	this->mouseIntersection = false;

	// check if mouse tracking is desired
	if (this->mouseTracking)
	{
		// get 3d contact under mouse

		Physics::PhysicsServer* physicsServer = Physics::PhysicsServer::Instance();
		const Ptr<Graphics::View> & view = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
		if (InputServer::HasInstance() && view->GetCameraEntity().isvalid())
		{
			float2 mousePos = InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();
			float length = 1000;
			line worldRay = this->ComputeMouseWorldRay(mousePos, length, view);

			// do the actual picking
			Ptr<Physics::Contact> contact;
			contact = Physics::PhysicsServer::Instance()->GetScene()->GetClosestContactUnderMouse(worldRay, this->mouseExcludeSet);
			if (contact.isvalid())
			{
				// store intersection position
				this->mousePos3d = contact->GetPoint();
				this->upVector = contact->GetUpVector();
				this->mouseIntersection = true;
				Game::Entity::EntityId gameEntityUnderMouse = -1;

				// new entity under mouse?
				const Physics::PhysicsObject* physicsEntity = contact->GetCollisionObject();
				if (physicsEntity)
				{
					Ptr<Core::RefCounted> obj = physicsEntity->GetUserData()->object;
					if (obj.isvalid() && obj->IsA(Game::Entity::RTTI))
					{
						gameEntityUnderMouse = obj.cast<Game::Entity>()->GetUniqueId();
					}
				}
				if (gameEntityUnderMouse != this->entityUnderMouse)
				{
					this->entityUnderMouse = gameEntityUnderMouse;
				}
			}
		}
	}
	if (this->mouseMessages)
	{
		if (this->entityUnderMouse != lastEntity)
		{
			if (lastEntity)
			{
				Ptr<Game::Entity> entity = EntityManager::Instance()->GetEntityByUniqueId(lastEntity);
				if (entity.isvalid())
				{
					__SendSync(entity, this->leaveMessage);
				}				
			}
			if (this->entityUnderMouse)
			{
				Ptr<Game::Entity> entity = EntityManager::Instance()->GetEntityByUniqueId(this->entityUnderMouse);
				if (entity.isvalid())
				{
					__SendSync(entity, this->enterMessage);
				}				
			}
		}		
		if (this->entityUnderMouse)
		{
			Ptr<Game::Entity> entity = EntityManager::Instance()->GetEntityByUniqueId(this->entityUnderMouse);
			if (entity.isvalid())
			{
				const Ptr<Input::Mouse> mouse = Input::InputServer::Instance()->GetDefaultMouse();
				Ptr<GraphicsFeature::MouseButtonEvent> msg = GraphicsFeature::MouseButtonEvent::Create();
				msg->SetButton(Input::MouseButton::LeftButton);
				if (mouse->ButtonDown(Input::MouseButton::LeftButton))
				{
					msg->SetDown(true);
					__SendSync(entity, msg);
				}
				if (mouse->ButtonUp(Input::MouseButton::LeftButton))
				{
					msg->SetDown(false);
					__SendSync(entity, msg);
				}
				msg->SetButton(Input::MouseButton::RightButton);
				if (mouse->ButtonDown(Input::MouseButton::RightButton))
				{
					msg->SetDown(true);
					__SendSync(entity, msg);
				}
				if (mouse->ButtonUp(Input::MouseButton::RightButton))
				{
					msg->SetDown(false);
					__SendSync(entity, msg);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Math::line 
EnvQueryManager::ComputeMouseWorldRay(const float2& mousePos, float length, const Ptr<Graphics::View>& view) const
{    
    const Ptr<Graphics::CameraEntity>& cam = view->GetCameraEntity();
    n_assert(cam.isvalid());
    const matrix44& invView = matrix44::inverse(cam->GetViewTransform());
    matrix44 invProj = matrix44::inverse(cam->GetProjTransform());    
    return MouseRayUtil::ComputeWorldMouseRay(mousePos, length, invView, invProj, cam->GetCameraSettings().GetZNear());    
}

} // namespace Game
