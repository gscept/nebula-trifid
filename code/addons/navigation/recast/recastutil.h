#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::RecastUtil
    
    A utility class for generating navmeshes via recast
    
    (C) 2014 Johannes Hirche
*/
#include "core/refcounted.h"
#include "io/uri.h"
#include "util/blob.h"
#include "math/bbox.h"
#include "db/reader.h"
#include "game/entity.h"

struct rcConfig;

//------------------------------------------------------------------------------
namespace Navigation
{
class RecastUtil : public Core::RefCounted
{
	__DeclareClass(RecastUtil);

public:	
	/// constructor
	RecastUtil();
    /// destructor
    ~RecastUtil();

	/// apply all individual settings
	void SetupConfig();
	/// configure agent sizes
	void SetAgentParams(float height, float radius, float maxClimb);
	/// adds a mesh resource to the geometry pool
	void AddMesh(const IO::URI & meshFile, const Math::matrix44 & transform);
    /// add convex region 
    void AddConvexArea(const Ptr<Game::Entity> & areaEntity);

	/// generate navmesh
	Util::Blob GenerateNavMeshData();

	/// get user friendly id
	const Util::String & GetId() const;
	/// set user friendly name (Id)
	void SetId(const Util::String & id);	
	///
	float GetAgentHeight() const;
	///
	void SetAgentHeight(float val);

	///
	void SetAgentRadius(float val);
	///
	float GetAgentRadius() const;
	
	///
	float GetAgentMaxClimb() const;
	///
	void SetAgentMaxClimb(float val);

	///
	float GetCellSize() const;
	///
	void SetCellSize(float val);

	///
	float GetCellHeight() const;
	///
	void SetCellHeight(float val);
	
	///
	float GetMaxSlope() const;
	///
	void SetMaxSlope(float val);

	///
	float GetMaxEdgeError() const;
	///
	void SetMaxEdgeError(float val);
	
	///
	float GetDetailSampleDist() const;
	///
	void SetDetailSampleDist(float val);

	///
	float GetDetailSampleMaxError() const;
	///
	void SetDetailSampleMaxError(float val);

	///
	int GetMaxEdgeLength() const;
	///
	void SetMaxEdgeLength(int val);

	///
	int GetRegionMinSize() const;
	///
	void SetRegionMinSize(int val);

    ///
    int GetRegionMergeSize() const;
    ///
    void SetRegionMergeSize(int val);

	/// get bounding box
	const Math::bbox & GetBoundingBox() const;
	/// set bounding box
	void SetBoundingBox(const Math::bbox & ibox);

    /// load all settings from a db
    bool LoadNavMeshGenerationData(const Ptr<Db::Reader>& reader);

    /// add extra mesh to navmesh generation (use the graphics resource)
    void AddExtraMesh(const Util::String & res, const Math::matrix44 & transform);
    /// reset all extra meshes
    void ResetExtraMeshes();

private:

	float agentRadius;
	float agentHeight;	
	float maxEdgeError;	
	float detailSampleDist;
	float maxSlope;
	float cellHeight;
	float cellSize;
	float agentMaxClimb;
	float detailSampleMaxError;
	int maxEdgeLength;
	int regionMinSize;
	int regionMergeSize;
	Math::bbox boundingBox;
	Util::String id;


	rcConfig *config;	
	Util::Array<IO::URI> meshes;
	Util::Array<Math::matrix44> transforms;
    Util::Array<IO::URI> extraMeshes;
    Util::Array<Math::matrix44> extraTransforms;
    Util::Array<Ptr<Game::Entity>> areaEntities;
};

//------------------------------------------------------------------------------
/**
*/
inline	
void
RecastUtil::SetId(const Util::String & iid)
{ 
	this->id = iid;
}

//------------------------------------------------------------------------------
/**
*/
inline	
const Util::String & 
RecastUtil::GetId() const
{ 
	return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RecastUtil::SetAgentParams(float height, float radius, float maxClimb)
{
	this->agentHeight = height;
	this->agentRadius = radius;
	this->agentMaxClimb = maxClimb;
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetAgentHeight() const
{ 
	return this->agentHeight; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetAgentHeight(float val) 
{ 
	this->agentHeight = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetAgentRadius() const
{ 
	return this->agentRadius; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetAgentRadius(float val) 
{ 
	this->agentRadius = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetAgentMaxClimb() const
{ 
	return this->agentMaxClimb; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetAgentMaxClimb(float val) 
{ 
	this->agentMaxClimb = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetCellSize() const
{ 
	return this->cellSize; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetCellSize(float val) 
{ 
	this->cellSize = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetCellHeight() const
{ 
	return this->cellHeight; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetCellHeight(float val) 
{ 
	this->cellHeight = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetMaxSlope() const
{ 
	return this->maxSlope; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetMaxSlope(float val) 
{ 
	this->maxSlope = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetMaxEdgeError() const
{ 
	return this->maxEdgeError; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetMaxEdgeError(float val) 
{ 
	this->maxEdgeError = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetDetailSampleDist() const
{ 
	return this->detailSampleDist; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetDetailSampleDist(float val) 
{ 
	this->detailSampleDist = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
float RecastUtil::GetDetailSampleMaxError() const
{ 
	return this->detailSampleMaxError; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetDetailSampleMaxError(float val) 
{ 
	this->detailSampleMaxError = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
int RecastUtil::GetMaxEdgeLength() const
{ 
	return this->maxEdgeLength; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetMaxEdgeLength(int val) 
{ 
	this->maxEdgeLength = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
int RecastUtil::GetRegionMinSize() const
{ 
	return this->regionMinSize; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetRegionMinSize(int val) 
{ 
	this->regionMinSize = val; 
}

//------------------------------------------------------------------------------
/**
*/
inline	
int RecastUtil::GetRegionMergeSize() const
{ 
    return this->regionMergeSize; 
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetRegionMergeSize(int val) 
{ 
    this->regionMergeSize = val; 
}
//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::SetBoundingBox(const Math::bbox & ibox)
{ 
	this->boundingBox = ibox;
}

//------------------------------------------------------------------------------
/**
*/
inline	
const Math::bbox & RecastUtil::GetBoundingBox() const
{ 
	return this->boundingBox;
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::AddExtraMesh(const Util::String & res, const Math::matrix44 & transform)
{
	this->extraMeshes.Append(res);
	this->extraTransforms.Append(transform);
}

//------------------------------------------------------------------------------
/**
*/
inline
void RecastUtil::ResetExtraMeshes()
{
	this->extraMeshes.Clear();
	this->extraTransforms.Clear();
    this->areaEntities.Clear();
}
}