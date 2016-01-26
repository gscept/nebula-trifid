//------------------------------------------------------------------------------
//  navigation/navigationmanager.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "addons/navigation/crowdmanager.h"
#include "DetourCrowd.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "navigationserver.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "basegamefeature/basegametiming/gametimesource.h"
#include "navigation/navigationattributes.h"
#include "basegamefeature/basegameprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "debug/navigationpagehandler.h"
#include "http/httpserverproxy.h"

namespace Navigation
{
__ImplementClass(Navigation::CrowdManager, 'NGCR', Game::Manager);
__ImplementSingleton(Navigation::CrowdManager);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
CrowdManager::CrowdManager():
    maxRadius(5.0f),
    agentBuffer(0),
    maxCrowdAgents(64)
{
	__ConstructSingleton;
    for(int i = 0 ; i < 3 ; i++)
    {
        this->defaultRadius[i] = 10.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
CrowdManager::~CrowdManager()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::SetMaxCrowdAgents(int count)
{    
    this->maxCrowdAgents = count;
    if(this->agentBuffer)
    {
        delete[] this->agentBuffer;
    }
    this->agentBuffer = new dtCrowdAgent*[this->maxCrowdAgents];
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::OnActivate()
{
    // create navigation server
    this->navigationServer = Navigation::NavigationServer::Create();
    this->agentBuffer = new dtCrowdAgent*[this->maxCrowdAgents];
#if __NEBULA3_HTTP__
    // setup http page handlers
    this->navigationPage = Navigation::NavigationPageHandler::Create();
    Http::HttpServerProxy::Instance()->AttachRequestHandler(this->navigationPage);
#endif
	// create some default avoidance parameter sets
	this->avoidanceParams.SetSize(4);
	dtObstacleAvoidanceParams * parm = new dtObstacleAvoidanceParams;	

	// Low (11)
	parm->velBias = 0.5f;
	parm->adaptiveDivs = 5;
	parm->adaptiveRings = 2;
	parm->adaptiveDepth = 1;
	parm->weightDesVel = 2.0f;
	parm->weightCurVel = 0.75f;
	parm->weightSide = 0.75f;
	parm->weightToi = 2.5f;
	parm->horizTime = 2.5f;
	parm->gridSize = 33;
	this->avoidanceParams[0] = parm;

	parm = new dtObstacleAvoidanceParams;
	// Medium (22)
	parm->velBias = 0.5f;
	parm->adaptiveDivs = 5;
	parm->adaptiveRings = 2;
	parm->adaptiveDepth = 2;
	parm->weightDesVel = 2.0f;
	parm->weightCurVel = 0.75f;
	parm->weightSide = 0.75f;
	parm->weightToi = 2.5f;
	parm->horizTime = 2.5f;
	parm->gridSize = 33;
	this->avoidanceParams[1] = parm;

	parm = new dtObstacleAvoidanceParams;
	// Good (45)
	parm->velBias = 0.5f;
	parm->adaptiveDivs = 7;
	parm->adaptiveRings = 2;
	parm->adaptiveDepth = 3;
	parm->weightDesVel = 2.0f;
	parm->weightCurVel = 0.75f;
	parm->weightSide = 0.75f;
	parm->weightToi = 2.5f;
	parm->horizTime = 2.5f;
	parm->gridSize = 33;
	this->avoidanceParams[2] = parm;
	
	parm = new dtObstacleAvoidanceParams;
	// High (66)
	parm->velBias = 0.5f;
	parm->adaptiveDivs = 7;
	parm->adaptiveRings = 3;
	parm->adaptiveDepth = 3;
	parm->weightDesVel = 2.0f;
	parm->weightCurVel = 0.75f;
	parm->weightSide = 0.75f;
	parm->weightToi = 2.5f;
	parm->horizTime = 2.5f;
	parm->gridSize = 33;
	this->avoidanceParams[3] = parm;

}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::OnDeactivate()
{
#if __NEBULA3_HTTP__    
    Http::HttpServerProxy::Instance()->RemoveRequestHandler(this->navigationPage);
    this->navigationPage = 0;
#endif

    this->Cleanup();
    // create navigation server
    this->navigationServer = 0;
    delete[] this->agentBuffer;
	for (int i = 0; i < this->avoidanceParams.Size(); i++)
	{
		delete this->avoidanceParams[i];
	}
	this->avoidanceParams.Clear();
}
//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::Cleanup()
{
	this->navCrowds.Clear();
	for(int i = 0 ; this->agentDict.Size() ; i++)
	{
		Util::KeyValuePair<Game::Entity::EntityId, agentData*> pp = this->agentDict.KeyValuePairAtIndex(i);
		delete pp.Value()->parms;
		delete pp.Value();
	}
	this->agentDict.Clear();
	for(int i = 0 ; i < crowds.Size() ; i++)
	{
		dtFreeCrowd(this->crowds[i]->crowd);
		delete crowds[i];
	}
	this->crowds.Clear();
    this->navigationServer->Cleanup();
	
}


//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::UpdateAgentTarget(const Ptr<Game::Entity> & entity, const Math::point & newTarget)
{
    Game::Entity::EntityId eid = entity->GetUniqueId();

    n_assert(this->agentDict.Contains(eid));

    static float t[4];
    newTarget.storeu(t);

    agentData * ad = this->agentDict[eid];

    dtNavMeshQuery * query = this->navigationServer->queries[ad->navMesh];
    dtPolyRef pRef;
    dtQueryFilter filter;
        
    float tp[3];
    dtStatus res = query->findNearestPoly(t,this->defaultRadius,&filter,&pRef,tp);
    n_assert2(res == DT_SUCCESS, "failed to find point on navmesh");
        
    ad->crowd->requestMoveTarget(ad->agentId,pRef,tp);    

}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::UpdateAgentSpeed(const Ptr<Game::Entity> & entity, float speed, float accel)
{
    Game::Entity::EntityId eid = entity->GetUniqueId();
    n_assert(this->agentDict.Contains(eid));

    agentData * ad = this->agentDict[eid];

    ad->parms->maxSpeed = speed;
    if(accel < 0.0f)
    {
        ad->parms->maxAcceleration = entity->GetFloat(Attr::RelVelocity);
    }
    else
    {
        ad->parms->maxAcceleration = accel;
    }
    
    ad->crowd->updateAgentParameters(ad->agentId,ad->parms);    
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::DeregisterAgent(const Ptr<Game::Entity> & entity)
{
    Game::Entity::EntityId eid = entity->GetUniqueId();

    n_assert(this->agentDict.Contains(eid));

    agentData * ad = this->agentDict[eid];
    delete ad->parms;
    ad->crowd->removeAgent(ad->agentId);
    delete ad;
    this->agentDict.Erase(eid);    
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::RegisterAgent(const Util::String & navMesh, const Ptr<Game::Entity> & entity)
{
    Game::Entity::EntityId eid = entity->GetUniqueId();
    
    agentData * ad = new agentData;

    dtCrowdAgentParams * parms = new dtCrowdAgentParams;
    
    parms->height = entity->GetFloat(Attr::CapsuleHeight);
    parms->radius = entity->GetFloat(Attr::CapsuleRadius);
    parms->maxSpeed = entity->GetFloat(Attr::MaxVelocity);
    parms->maxAcceleration = entity->GetFloat(Attr::RelVelocity);
	parms->updateFlags = DT_CROWD_SEPARATION | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE;
    parms->collisionQueryRange = entity->GetFloat(Attr::NavigationQueryRange);
    parms->separationWeight = entity->GetFloat(Attr::NavigationSeparationWeight);
	parms->pathOptimizationRange = parms->radius * 30.0f;
	parms->obstacleAvoidanceType = entity->GetInt(Attr::AvoidanceQuality);
	parms->queryFilterType = 1;
    parms->userData = ad;

    ad->parms = parms;
	const Util::String& guid = NavigationServer::Instance()->GetNavmeshFromId(navMesh);
    Math::point startPos = entity->GetMatrix44(Attr::Transform).get_position();  
	Math::point closest = NavigationServer::Instance()->FindClosestOnMap(guid, startPos, 20.0f);
    static float pos[4];
    closest.storeu(pos);


	ad->crowd = FindCrowd(guid, closest);
    ad->agentId = ad->crowd->addAgent(pos,parms);
    ad->eId = eid;
	ad->navMesh = guid;
            
    this->agentDict.Add(eid,ad);
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::RegisterAgent(const Ptr<Game::Entity> & entity)
{
    this->RegisterAgent(this->navigationServer->selected,entity);
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::OnBeginFrame()
{
    
    for(int i = 0 ; i < this->crowds.Size() ; i++)
    {
        crowdData * cd = this->crowds[i];
        float now = (float)BaseGameFeature::GameTimeSource::Instance()->GetTime();
        cd->crowd->update(now - cd->lastUpdate,NULL);
        cd->lastUpdate = now;

        // now update the positions
        int active = cd->crowd->getActiveAgents(this->agentBuffer, this->maxCrowdAgents);
        for(int j = 0 ; j < active ; j++)
        {
            agentData * ad = (agentData*)this->agentBuffer[j]->params.userData;
            Ptr<Game::Entity> entity = BaseGameFeature::EntityManager::Instance()->GetEntityByUniqueId(ad->eId);
            if(entity.isvalid())
            {
                matrix44 oldtrans = entity->GetMatrix44(Attr::Transform);
                float *fpos = this->agentBuffer[j]->npos;
                float *fvel = this->agentBuffer[j]->vel;
                point pos(fpos[0],fpos[1],fpos[2]);
                vector dir(fvel[0],fvel[1],fvel[2]);
                dir = vector::normalize(dir);
                if(dir.lengthsq3() <N_TINY)
                {
                    dir = oldtrans.get_zaxis();
                }
                matrix44 trans;
                trans.set_zaxis(dir);
                trans.set_yaxis(oldtrans.get_yaxis());
                trans.set_xaxis(Math::float4::cross3(trans.get_yaxis(),dir));
                trans.set_position(pos);
                Ptr<BaseGameFeature::SetTransform> msgTrans = BaseGameFeature::SetTransform::Create();
                msgTrans->SetMatrix(trans);
                __SendSync(entity,msgTrans);	
            }
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
dtCrowd * 
CrowdManager::FindCrowd(const Util::String & navMesh, const Math::point & pos)
{	
    if(this->navCrowds.Contains(navMesh))
    {
        // check all crowds and do a sample of 5 agents to find the closest one
        const Util::Array<dtCrowd*> & arr = this->navCrowds[navMesh];
        int candidate = -1;
        float distSq = -1.0f;        

        for(int i = 0 ; i < arr.Size() ; i++)
        {
            dtCrowd * crowd = arr[i];
            int active = crowd->getActiveAgents(this->agentBuffer,this->maxCrowdAgents);
            if(active == this->maxCrowdAgents)            
                continue;
                        
            if(active)
            {
                for(int j = 0 ; j < active ; j++)
                {
                    if(this->agentBuffer[j])
                    {
                        float * fpos = this->agentBuffer[j]->npos;
                        Math::point p(fpos[0],fpos[1],fpos[2]);
                        float dist = (pos-p).lengthsq3();
                        if(dist < distSq || distSq < 0)
                        {
                            candidate = i;
                            distSq = dist;
                        }
                    }
                }
            }
            else if(distSq < 0.0f)
            {
                // no active agents in crowd, use this crowd instead of creating a new one in case no other is found
                candidate = i;
            }            
        }        
        if(candidate != -1)
        {
            return arr[candidate];
        }
    }    
    // no matching crowd was found, create new one
    n_assert(this->navigationServer->HasNavMesh(navMesh));
    dtCrowd * crowd = dtAllocCrowd();
	crowd->getEditableFilter(1)->setIncludeFlags(1);
	crowd->getEditableFilter(1)->setExcludeFlags(2);
    crowd->init(this->maxCrowdAgents,this->maxRadius,this->navigationServer->meshes[navMesh]);
    crowdData * cd = new crowdData;
    cd->crowd = crowd;
    cd->lastUpdate =  (float)BaseGameFeature::GameTimeSource::Instance()->GetTime();
    cd->navMesh = navMesh;
    this->crowds.Append(cd);

	for (int i = 0; i < this->avoidanceParams.Size(); i++)
	{ 
		crowd->setObstacleAvoidanceParams(i, this->avoidanceParams[i]);
	}

    if(this->navCrowds.Contains(navMesh))
    {
        this->navCrowds[navMesh].Append(crowd);
    }
    else
    {
        Util::Array<dtCrowd*> cr;
        cr.Append(crowd);
        this->navCrowds.Add(navMesh,cr);
    }            
    return crowd;        
}

//------------------------------------------------------------------------------
/**
*/
void
CrowdManager::StopAgent(const Ptr<Game::Entity> &entity)
{
    Game::Entity::EntityId eid = entity->GetUniqueId();

	if (this->agentDict.Contains(eid))
	{
		agentData * ad = this->agentDict[eid];
		UpdateAgentTarget(entity, entity->GetMatrix44(Attr::Transform).get_position());
		UpdateAgentSpeed(entity, 0.0f);
		ad->parms->maxAcceleration = 9999.0f;
		ad->crowd->updateAgentParameters(ad->agentId, ad->parms);
	}    
}
}