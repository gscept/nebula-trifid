#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EnvQueryManager
    
    The EnvQueryManager implements environment queries into the game world,
    like stabbing queries, line-of-sight checks, etc...
    
    (C) 2005 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "game/entity.h"
#include "physics/filterset.h"
#include "physics/materialtable.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/line.h"
#include "math/float2.h"
#include "graphics/view.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class EnvQueryManager : public Game::Manager
{
	__DeclareClass(EnvQueryManager);
    __DeclareSingleton(EnvQueryManager);
public:
    /// constructor
    EnvQueryManager();
    /// destructor
    virtual ~EnvQueryManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called per-frame by game server
    virtual void OnFrame();
    
	/// get a const Ref on the MouseExcludeSet
	const Physics::FilterSet& GetMouseExcludeSet() const;
	/// set a new MouseExcludeSet
	void SetMouseExcludeSet( const Physics::FilterSet& mouseExcludeSet );
    /// get the material under the mouse cursor
//    Physics::MaterialType GetMaterialUnderMouse() const;

	/// enable on-frame mouse updates calculating entity under mouse and 3d position
	void SetMouseTracking(bool enable);
	/// get on-frame mouse tracking status
	const bool GetMouseTracking() const;
	/// enable entity messages when entering/leaving/clicking on objects. will enable tracking as well
	void SetMouseMessages(bool enable);
	/// get on-frame mouse messaging status
	const bool GetMouseMessages() const;
    /// get the entity under the mouse cursor
    Game::Entity* GetEntityUnderMouse() const;
    /// get the mouse position in the 3d world
    const Math::point& GetMousePos3d() const;
    /// compute 3d ray thru mouse pos
    Math::line ComputeMouseWorldRay(const Math::float2& mousePos, float length, const Ptr<Graphics::View>& view) const;
    /// get the upVector of the face under the mouse cursor
    const Math::vector& GetUpVector() const;
    /// return true if mouse is over "something"
    bool HasMouseIntersection() const;
    /// get all entities in a given spherical area
    Util::Array<Ptr<Game::Entity> > GetEntitiesInSphere(const Math::point& midPoint, float radius);
    /// get all entities in a given box shaped area
    Util::Array<Ptr<Game::Entity> > GetEntitiesInBox(const Math::vector& scale, const Math::matrix44& m); 
	
 
protected:
    Game::Entity::EntityId entityUnderMouse;
    bool mouseIntersection;     
	bool mouseTracking;
	bool mouseMessages;
    Math::point mousePos3d;
    Math::vector upVector;   
    
	Physics::FilterSet mouseExcludeSet;
    //Physics::MaterialType materialUnderMouse;
	Ptr<GraphicsFeature::MouseEnter> enterMessage;
	Ptr<GraphicsFeature::MouseLeave> leaveMessage;
};

//------------------------------------------------------------------------------
/**
*/
inline const bool
EnvQueryManager::GetMouseTracking() const
{
	return this->mouseTracking;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvQueryManager::SetMouseTracking(bool enable)
{
	this->mouseTracking = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
EnvQueryManager::GetMouseMessages() const
{
	return this->mouseMessages;
}

//------------------------------------------------------------------------------
/**
*/
inline void
EnvQueryManager::SetMouseMessages(bool enable)
{
	this->mouseMessages = enable;
	if (enable)
	{
		this->SetMouseTracking(true);
	}
}

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
