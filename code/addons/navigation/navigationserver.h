#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::NavigationServer
  
    Provides navigation features
    
    (C) 2012 Johannes Hirche
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "io/uri.h"
#include "util/blob.h"
#include "util/guid.h"
#include "path3d.h"
#include "recast/recastutil.h"

#define MAX_NAV_NODES 8192
class dtNavMesh;
class dtNavMeshQuery;

//------------------------------------------------------------------------------
namespace Navigation
{
class CrowdManager;

class NavigationServer : public Core::RefCounted
{
	__DeclareClass(NavigationServer);
	__DeclareSingleton(NavigationServer);
public:
	/// constructor
	NavigationServer();
	/// destructor
	virtual ~NavigationServer();

	/// Load and set current navmesh (HACK for the time being)
	bool LoadNavigationData(const Util::String& guid, const IO::URI & navDataFile);

	/// Load and set current navmesh (HACK for the time being)
	bool LoadNavigationData(const Util::String& guid, const Util::Blob& data);

    /// load navmesh generation data from database
    bool LoadNavMeshGenerationData(const Util::String& id, const Ptr<Db::Reader>& reader);

    /// select active navmesh
    bool SelectNavMesh(const Util::String& guid);

    /// queries wether server contains navmesh for given guid
    bool HasNavMesh(const Util::String& guid);
    /// Deletes navmesh with given guid
    void DeleteNavMesh(const Util::String& guid);

	/// updates a navmesh 
	void UpdateNavMesh(const Util::String& guid, const Util::Array<Util::String> & meshResources, const Util::Array<Math::matrix44> &transforms);

	/// resets a navmesh (removes additional meshresources and regenerates it
	void ResetNavMesh(const Util::String & guid);

    /// gives access to internal navmesh blob (for saving)
    const Util::Blob * GetInternalData(const Util::String& guid);
	
	/// render debug visualization of currently selected or all navmeshes
	void RenderDebug(bool all = false);
    /// render debug visualization of specific navmesh
    void RenderDebug(const Util::String& guid);

	/// is a position near the map ?
	bool IsOnMap(const Math::point& position);
    /// find closest point on map
    Math::point FindClosestOnMap(const Util::String & mapname, const Math::point & p, float radius);
    /// find closest point on selected map
    Math::point FindClosestOnMap(const Math::point & p, float radius);
	/// create a path from point `a' to point `b'.
	Ptr<Path3D> MakePath(const Math::point& a, const Math::point& b, float radius = 10.0f);
	/// create a string path from point `a' to point `b'.
	Ptr<Path3D> MakeDirectPath(const Math::point& a, const Math::point& b);

    /// create a path from point `a' to point `b' on specific map.
    Ptr<Path3D> MakePath(const Util::String & mapname, const Math::point& a, const Math::point& b, float radius = 10.0f);    

    /// list all available navmeshes (slow)
    Util::Array<Util::String> GetNavMeshes();
    /// currently selected navmesh
    const Util::String& GetCurrentNavmesh();
	/// get mesh guid from id
	const Util::String& GetNavmeshFromId(const Util::String& id);
    
	///
	void UpdateAreaId(const Util::String & mapname, const Math::point& pos, unsigned short areaId, unsigned short filter);

    friend class Navigation::CrowdManager;	
private:    
    /// remove all navmeshes
    void Cleanup();
    Util::HashTable<Util::String, dtNavMesh *> meshes;		
    Util::HashTable<Util::String, dtNavMeshQuery *> queries;
    Util::HashTable<Util::String, Util::Blob*> blobs;		
    Util::HashTable<Util::String, Ptr<Navigation::RecastUtil>> recasts;
	Util::HashTable<Util::String, Util::String> idMapping;
    Util::String selected;      
};


//------------------------------------------------------------------------------
/**
*/
inline 
bool
NavigationServer::SelectNavMesh(const Util::String& guid)
{
    if(this->meshes.Contains(guid))
    {
        this->selected = guid;
        return true;
    }
    return false;    
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool
NavigationServer::HasNavMesh(const Util::String& guid)
{
    return this->meshes.Contains(guid);    
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
NavigationServer::GetNavmeshFromId(const Util::String& id)
{
	return this->idMapping[id];
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Util::Blob *
NavigationServer::GetInternalData(const Util::String& guid)
{
    n_assert(this->blobs.Contains(guid));    
    return this->blobs[guid];
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Util::String &
NavigationServer::GetCurrentNavmesh()
{
    return this->selected; 
}

}
