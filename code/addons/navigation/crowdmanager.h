#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::CrowdManager
  
    Provides crowd navigation features
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "game/entity.h"
#include "navigationserver.h"
#include "http/httprequesthandler.h"
#include "debug/navigationpagehandler.h"
#include "util/fixedarray.h"

class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
struct dtCrowdAgent;
struct dtCrowdAgentParams;
struct dtObstacleAvoidanceParams;

//------------------------------------------------------------------------------
namespace Navigation
{
class CrowdManager : public Game::Manager
{
	__DeclareClass(CrowdManager);
	__DeclareSingleton(CrowdManager);
public:
	/// constructor
	CrowdManager();
	/// destructor
	virtual ~CrowdManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called before frame by the game server
    virtual void OnBeginFrame();

    /// register agent to specific navmesh (must be present in navigation server
    void RegisterAgent(const Util::String & navMesh, const Ptr<Game::Entity> & entity);
    /// register agent to default navmesh 
    void RegisterAgent(const Ptr<Game::Entity> & entity);
    /// remove agent from system
    void DeregisterAgent(const Ptr<Game::Entity> & entity);
    /// update agent target
    void UpdateAgentTarget(const Ptr<Game::Entity> & entity, const Math::point & newTarget);
    /// update agent speed
    void UpdateAgentSpeed(const Ptr<Game::Entity> & entity, float speed, float accel = -1.0f);    
	/// render debug 
    void RenderDebug(){}
    /// cleanup crowd structures and navmeshes
    void Cleanup();
   	
    /// sets default search radius for point queries while doing pathfinding
    void SetDefaultQueryRadius(float radius);
    /// set maximum agent radius
    void SetMaxRadius(float radius);    
    /// stop agent
    void StopAgent(const Ptr<Game::Entity> &entity);
    /// Set maximum agents per crowd (has to be called before adding any entities)
    void SetMaxCrowdAgents(int count);
    /// Get maximum agents per crowd
    int GetMaxCrowdAgents() const;
	/// Get agent count
	int GetAgentCount() const;

private:            
    /// find a matching crowd, will create new one if necessary
    dtCrowd * FindCrowd(const Util::String & navMesh, const Math::point & pos);

    typedef struct 
    {
        dtCrowd * crowd;
        int agentId;
        Util::String navMesh;
        dtCrowdAgentParams * parms;
        Game::Entity::EntityId eId;        
    } agentData;

    typedef struct
    {
        dtCrowd * crowd;
        Util::String navMesh;
        float lastUpdate;
    } crowdData;

    Util::Dictionary<Game::Entity::EntityId, agentData*> agentDict;
    Ptr<Navigation::NavigationServer> navigationServer;
    Util::Array<crowdData*> crowds;
	Util::FixedArray<dtObstacleAvoidanceParams*> avoidanceParams;
    Util::HashTable<Util::String,Util::Array<dtCrowd*>> navCrowds;
        
    float defaultRadius[3];
    float maxRadius;
    int maxCrowdAgents;
    dtCrowdAgent ** agentBuffer;

    friend class Navigation::NavigationPageHandler;
#if __NEBULA3_HTTP__
    Ptr<Http::HttpRequestHandler> navigationPage;
    
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline 
void
CrowdManager::SetDefaultQueryRadius(float radius)
{
    for(int i = 0 ; i < 2 ; i++)
    {
        this->defaultRadius[i] = radius;
    }    
}

//------------------------------------------------------------------------------
/**
*/
inline 
void
CrowdManager::SetMaxRadius(float radius)
{
   this->maxRadius = radius;
}
//------------------------------------------------------------------------------
/**
*/
inline 
int
CrowdManager::GetMaxCrowdAgents() const
{
    return this->maxCrowdAgents;
}

//------------------------------------------------------------------------------
/**
*/
inline 
int
CrowdManager::GetAgentCount() const
{
	return this->agentDict.Size();
}

}
