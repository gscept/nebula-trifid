//------------------------------------------------------------------------------
//  navigation/navigationserver.cc
//  (C) 2012 Johannes Hirche
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "addons/navigation/navigationserver.h"
#include "io/ioserver.h"
#include "io/binaryreader.h"
#include "debug/detourdebug.h"
#include "DetourDebugDraw.h"
#include "crowdmanager.h"

namespace Navigation
{
__ImplementClass(Navigation::NavigationServer, 'NGSV', Core::RefCounted);
__ImplementSingleton(Navigation::NavigationServer);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
NavigationServer::NavigationServer()    
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
NavigationServer::~NavigationServer()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
NavigationServer::LoadNavigationData(const Util::String& guid, const IO::URI & navDataFile)
{
	Util::Blob navData;
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(navDataFile);
	Ptr<IO::BinaryReader> reader = IO::BinaryReader::Create();
	reader->SetStream(stream);
	if(reader->Open())
	{
		navData = reader->ReadBlob();
		reader->Close();
		
		return LoadNavigationData(guid, navData);		
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
NavigationServer::LoadNavMeshGenerationData(const Util::String& id, const Ptr<Db::Reader>& reader)
{
    n_assert(reader->IsOpen());
    Ptr<Navigation::RecastUtil> recast = Navigation::RecastUtil::Create();
    recast->LoadNavMeshGenerationData(reader);
    this->recasts.Add(id,recast);
	this->idMapping.Add(recast->GetId(),id);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
NavigationServer::LoadNavigationData(const Util::String& guid, const Util::Blob & navData)
{
	if(navData.Size())
	{			
        if(this->meshes.Contains(guid))
        {
            DeleteNavMesh(guid);
        }        
        dtNavMesh * navMesh = dtAllocNavMesh();                
        this->meshes.Add(guid, navMesh);
        Util::Blob * blob = new Util::Blob(navData);
        this->blobs.Add(guid, blob);
        dtNavMeshQuery * query = dtAllocNavMeshQuery();
        this->queries.Add(guid,query);
	    if(DT_SUCCESS == navMesh->init((unsigned char*)blob->GetPtr(), blob->Size(),0))
	    {            
            query->init(navMesh,MAX_NAV_NODES);
			return true;
	    }
    }
	return false;
}

//------------------------------------------------------------------------------
/**
	render debug visualization	
*/
void 
NavigationServer::RenderDebug(bool all)
{
	if(all)
	{
		Array<KeyValuePair<Util::String, dtNavMesh *>> meshArray = this->meshes.Content();
		for(IndexT i = 0 ; i < meshArray.Size();i++)
		{
			if(meshArray[i].Value()->getMaxTiles())
			{
				DebugDraw dd;
				duDebugDrawNavMesh(&dd,*(meshArray[i].Value()),0);
			}
		}
	}
	else
	{
		if(this->meshes.Contains(this->selected))
		{
			dtNavMesh * current = this->meshes[this->selected];
			if(current && current->getMaxTiles())
			{
				DebugDraw dd;
				duDebugDrawNavMesh(&dd,*(current),0);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
	render debug visualization	
*/
void 
NavigationServer::RenderDebug(const Util::String& guid)
{
    if(this->meshes.Contains(guid))
    {
        dtNavMesh * current = this->meshes[guid];
        if(current && current->getMaxTiles())
	    {
		    DebugDraw dd;
		    duDebugDrawNavMesh(&dd,*(current),0);
	    }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationServer::DeleteNavMesh(const Util::String& guid)
{
	n_assert(this->meshes.Contains(guid));
    dtFreeNavMesh(this->meshes[guid]);
    this->meshes.Erase(guid);
    delete this->blobs[guid];
    this->blobs.Erase(guid);
    dtFreeNavMeshQuery(this->queries[guid]);
    this->queries.Erase(guid);	
}

//------------------------------------------------------------------------------
/**
	remove all navmeshes
*/
void 
NavigationServer::Cleanup()
{
    Array<KeyValuePair<Util::String, dtNavMesh *>> meshArray = this->meshes.Content();
    Array<KeyValuePair<Util::String, dtNavMeshQuery *>> queryArray = this->queries.Content();
    Array<KeyValuePair<Util::String, Util::Blob*>> blobArray = this->blobs.Content();
	for(IndexT i = 0 ; i < meshArray.Size();i++)
	{
		dtFreeNavMesh(meshArray[i].Value());
		dtFreeNavMeshQuery(queryArray[i].Value());
        delete blobArray[i].Value();
	}
	this->meshes.Clear();
	this->blobs.Clear();
	this->queries.Clear();
	this->recasts.Clear();
	this->idMapping.Clear();
}


//------------------------------------------------------------------------------
/**
    return a path without pathfinding
*/
Ptr<Path3D>
NavigationServer::MakeDirectPath(const Math::point& a, const Math::point& b)
{
    // no path finding map available, just return a string path
    Ptr<Path3D> path = Path3D::Create();
    path->Extend(a);
    path->Extend(b);
	path->SetTarget(b);	
    path->SetValid(true);

    return path;
}

//------------------------------------------------------------------------------
/**
    return a path with pathfinding
*/
Ptr<Path3D>
NavigationServer::MakePath(const Math::point& a, const Math::point& b, float radius)
{
    return this->MakePath(this->selected, a, b, radius);
}

//------------------------------------------------------------------------------
/**
    return a path with pathfinding
*/
Ptr<Path3D>
NavigationServer::MakePath(const Util::String & mapname, const Math::point& a, const Math::point& b, float radius)
{  
	n_assert2(this->meshes.Contains(mapname), "invalid navmesh selected");
    Ptr<Path3D> path = Path3D::Create();
    //path->Extend(a);
    
	path->SetTarget(b);	
    path->SetValid(true);

	dtNavMesh * msh = this->meshes[mapname];
	dtNavMeshQuery * query = this->queries[mapname];
    dtQueryFilter filter;
    dtPolyRef ap,bp;
    float anp[3],bnp[3];
    float aa[4],bb[4],ext[3];
    a.storeu(aa);
    b.storeu(bb);
    for(int i = 0 ; i<3 ; i++)
    {    
        ext[i] = radius;
    }

	dtStatus res = query->findNearestPoly(aa,ext,&filter,&ap,anp);
    dtStatus res2 = query->findNearestPoly(bb,ext,&filter,&bp,bnp);

    static const int MAX_POLYS = MAX_NAV_NODES;
    dtPolyRef polys[MAX_POLYS];
    int polyCount;
    float straight[3*MAX_POLYS];
    int straightCount = 0;
    query->findPath(ap,bp,aa,bb,&filter,polys,&polyCount,MAX_POLYS);

    if(polyCount > 0)
    {        
        query->findStraightPath(aa,bb, polys, polyCount,
            straight, 0, 0, &straightCount, MAX_POLYS,DT_STRAIGHTPATH_ALL_CROSSINGS);            
        if(straightCount > 0)
        {
            for(int i = 0 ; i<straightCount ; i++)
            {
                Math::point p;
                p.load_float3(&(straight[i*3]),1);
                path->Extend(p);
            }
        }
    }        
    return path;
}

//------------------------------------------------------------------------------
/**
    return a point on map
*/
Math::point 
NavigationServer::FindClosestOnMap(const Util::String & mapname, const Math::point & p, float radius)
{
    n_assert2(this->meshes.Contains(mapname), "invalid navmesh selected");

    dtNavMeshQuery * query = this->queries[mapname];
    dtQueryFilter filter;
    dtPolyRef ap;
    float anp[3];
    float aa[4],ext[3];
    p.storeu(aa);    
    for(int i = 0 ; i<3 ; i++)
    {    
        ext[i] = radius;
    }

    dtStatus res = query->findNearestPoly(aa,ext,&filter,&ap,anp);
    n_assert2(res == DT_SUCCESS, "failed to find point on navmesh");
    return Math::point(anp[0],anp[1],anp[2]);
}


//------------------------------------------------------------------------------
/**
    return a point on map
*/
Math::point 
NavigationServer::FindClosestOnMap(const Math::point & p, float radius)
{
    return this->FindClosestOnMap(this->selected, p, radius);
}


//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
NavigationServer::GetNavMeshes()
{
    // FIXME this isn't very efficient
    Util::Array<KeyValuePair<Util::String, dtNavMesh*>> meshArray = this->meshes.Content();
    Util::Array<Util::String> stringArray;
    for(IndexT i = 0 ; i < meshArray.Size() ; i++)
    {
        stringArray.Append(meshArray[i].Key());
    }
    return stringArray;
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationServer::UpdateNavMesh(const Util::String& id, const Util::Array<Util::String> & meshResources, const Util::Array<Math::matrix44> &transforms)
{
	n_assert2(CrowdManager::Instance()->GetAgentCount() == 0, "Can't update navmesh while agents are using it");
	const Util::String & guid = this->idMapping[id];
	n_assert2(this->HasNavMesh(guid),"Unknown navmesh guid");
	n_assert(meshResources.Size() == transforms.Size());
	Ptr<RecastUtil> recast = this->recasts[guid];
	for(int i = 0 ; i<meshResources.Size() ; i++)
	{
		recast->AddExtraMesh(meshResources[i],transforms[i]);
	}
	Util::Blob blob = recast->GenerateNavMeshData();
	this->LoadNavigationData(guid,blob);	
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationServer::ResetNavMesh(const Util::String & id)
{
	const Util::String & guid = this->idMapping[id];
	Ptr<RecastUtil> recast = this->recasts[guid];
	Util::Array<Util::String> dummy;
	Util::Array<Math::matrix44> dummy2;
	recast->ResetExtraMeshes();
	this->UpdateNavMesh(guid,dummy,dummy2);
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationServer::UpdateAreaId(const Util::String & id, const Math::point& pos, unsigned short areaId, unsigned short filterFlag)
{
	Util::String mapname = this->GetNavmeshFromId(id);
	dtNavMesh * mesh = this->meshes[mapname];
	dtNavMeshQuery * query = this->queries[mapname];

	dtQueryFilter filter;
	filter.setIncludeFlags(filterFlag);
	dtPolyRef ap;
	float anp[3];
	float aa[4], ext[3];
	pos.storeu(aa);
	for (int i = 0; i < 3; i++)
	{
		ext[i] = 1.0f;
	}

	dtStatus res = query->findNearestPoly(aa, ext, &filter, &ap, anp);
	if (res == DT_SUCCESS)
	{
		const dtMeshTile * tile;
		const dtPoly * poly;
		mesh->getTileAndPolyByRefUnsafe(ap, &tile, &poly);
		if (poly)
		{
			dtPoly * pp = (dtPoly *)(poly);
			pp->flags  = areaId;
		}
	}
	else
	{
		n_printf("failed to update navmesh poly\n");
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
NavigationServer::GetAreaId(const Util::String & id, const Math::point& pos, unsigned short filterFlag)
{
	Util::String mapname = this->GetNavmeshFromId(id);
	dtNavMesh * mesh = this->meshes[mapname];
	dtNavMeshQuery * query = this->queries[mapname];

	dtQueryFilter filter;
	filter.setIncludeFlags(filterFlag);
	dtPolyRef ap;
	float anp[3];
	float aa[4], ext[3];
	pos.storeu(aa);
	for (int i = 0; i < 3; i++)
	{
		ext[i] = 1.0f;
	}

	dtStatus res = query->findNearestPoly(aa, ext, &filter, &ap, anp);
	if (res == DT_SUCCESS)
	{
		const dtMeshTile * tile;
		const dtPoly * poly;
		mesh->getTileAndPolyByRefUnsafe(ap, &tile, &poly);
		if (poly)
		{
			return poly->flags;			
		}
	}
	n_printf("failed to get navmesh poly\n");
	return 0;	
}

//------------------------------------------------------------------------------
/**
*/
bool NavigationServer::IsOnMap(const Math::point& position)
{	
	dtNavMeshQuery * query = this->queries[this->selected];
	dtQueryFilter filter;
	dtPolyRef ap;
	float anp[3];
	float aa[4],ext[3];
	position.storeu(aa); 
	for(int i = 0 ; i<3 ; i++)
	{    
		ext[i] = 0.1f;
	}

	dtStatus res = query->findNearestPoly(aa,ext,&filter,&ap,anp);
	return res == DT_SUCCESS;	
}

}