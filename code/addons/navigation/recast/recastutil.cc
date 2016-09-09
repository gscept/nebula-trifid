//  navigation/recast/recastutil.cc
//  (C) 2014 Johannes Hirche
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "Recast.h"
#include "recastutil.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "coregraphics/legacy/nvx2streamreader.h"
#include "io/ioserver.h"
#include "coregraphics/primitivegroup.h"
#include "navigationattributes.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "toolkit/toolkitutil/modelutil/modelphysics.h"

using namespace Math;
using namespace Util;
namespace Navigation
{

__ImplementClass(Navigation::RecastUtil, 'RCST', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
RecastUtil::RecastUtil():
	maxEdgeLength(12),
	cellHeight(0.2f),
	cellSize(0.3f),
	maxEdgeError(1.3f),
	regionMinSize(8),
	regionMergeSize(20),
	detailSampleDist(6),
	detailSampleMaxError(1.0f),
	maxSlope(45.0f),
    config(0)
{
	
}


//------------------------------------------------------------------------------
/**
*/
RecastUtil::~RecastUtil()
{
    if(this->config != NULL)
    {
        Memory::Free(Memory::ScratchHeap,this->config);
        this->config = NULL;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RecastUtil::AddMesh(const IO::URI & meshFile, const Math::matrix44 & transform)
{
	this->meshes.Append(meshFile);
	this->transforms.Append(transform);
}

//------------------------------------------------------------------------------
/**
*/
void 
RecastUtil::SetupConfig()
{		
    if(this->config == NULL)
    {
        this->config = (rcConfig*)Memory::Alloc(Memory::ScratchHeap,sizeof(rcConfig));
    }
	Memory::Clear(config,sizeof(config));
	config->cs = cellSize;
	config->ch = cellHeight;
	config->walkableSlopeAngle = maxSlope;
	config->walkableHeight = (int)ceilf(agentHeight / config->ch);
	config->walkableClimb = (int)floorf(agentMaxClimb / config->ch);
	config->walkableRadius = (int)ceilf(agentRadius / config->cs);
	config->maxEdgeLen = (int)(maxEdgeLength / cellSize);
	config->maxSimplificationError = maxEdgeError;
	config->minRegionArea = regionMinSize * regionMinSize;
	config->mergeRegionArea = regionMergeSize * regionMergeSize;
	config->maxVertsPerPoly = 6;
	config->detailSampleDist = detailSampleDist < 0.9f ? 0 : cellSize * detailSampleDist;
	config->detailSampleMaxError = cellHeight * detailSampleMaxError;
	vector extents = boundingBox.extents();
	point center = boundingBox.center();
	vector bmax = center + extents;
	vector bmin = center - extents;

	for(int i = 0 ; i < 3 ; i++)
	{
		config->bmax[i] = bmax[i];
		config->bmin[i] = bmin[i];
	}


}


//------------------------------------------------------------------------------
/**
*/
Util::Blob
RecastUtil::GenerateNavMeshData()
{
	unsigned char* m_triareas = NULL;	
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;	
	rcPolyMeshDetail* m_dmesh;
	rcContext m_ctx;
	
	
	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcCalcGridSize(config->bmin, config->bmax, config->cs, &config->width, &config->height);

	//
	// Step 1. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	n_assert(m_solid);
	rcCreateHeightfield(&m_ctx, *m_solid, config->width, config->height, config->bmin, config->bmax, config->cs, config->ch);
	

	Util::Array<IO::URI> allMeshes = this->meshes;
	allMeshes.AppendArray(this->extraMeshes);
	Util::Array<Math::matrix44> allTransforms = this->transforms;
	allTransforms.AppendArray(this->extraTransforms);
	// loop through all the mesh files
	IndexT idx;
	for(idx = 0 ; idx < allMeshes.Size() ; idx++)
	{
		Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(allMeshes[idx]);
		Ptr<Legacy::Nvx2StreamReader> nvx2Reader = Legacy::Nvx2StreamReader::Create();
		nvx2Reader->SetStream(stream);
		nvx2Reader->SetUsage(Base::ResourceBase::UsageImmutable);	
		nvx2Reader->SetAccess(Base::ResourceBase::AccessNone);
		nvx2Reader->SetRawMode(true);

		if (nvx2Reader->Open())
		{
			
			const Util::Array<CoreGraphics::PrimitiveGroup>& groups = nvx2Reader->GetPrimitiveGroups();		

			float *vertexData = nvx2Reader->GetVertexData();
			uint *indexData = (uint*)nvx2Reader->GetIndexData(); 

			IndexT count = 0;
 			for(int i = 0; i < nvx2Reader->GetNumVertices(); i++)
 			{
 				unsigned int offset = i * nvx2Reader->GetVertexWidth();
 				float4 v(vertexData[offset],vertexData[offset+1],vertexData[offset+2],1);
 				float4 trans = Math::matrix44::transform(v, allTransforms[idx]);
 				for(int j = 0; j < 3 ; j++)
 				{
 					vertexData[offset+j] = trans[j];
 				}
 			}


			for(int i=0;i < groups.Size();i++)
			{
				int ntris = groups[i].GetNumPrimitives(CoreGraphics::PrimitiveTopology::TriangleList);
				float* verts = &(vertexData[groups[i].GetBaseVertex()]);
				int nverts = groups[i].GetNumVertices();
				uint* tris = &(indexData[groups[i].GetBaseIndex()]);
				
				n_assert2(groups[i].GetPrimitiveTopology() == CoreGraphics::PrimitiveTopology::TriangleList,"Only triangle lists are supported");
				
				m_triareas = (unsigned char*)Memory::Alloc(Memory::DefaultHeap, groups[i].GetNumPrimitives(CoreGraphics::PrimitiveTopology::TriangleList));
				Memory::Clear(m_triareas,ntris * sizeof(unsigned char));		

				rcMarkWalkableTriangles(&m_ctx, config->walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas, RC_IDX_INT,nvx2Reader->GetVertexWidth());
				rcRasterizeTriangles(&m_ctx, verts, nvx2Reader->GetVertexWidth(), nverts, tris, m_triareas, ntris, *m_solid, config->walkableClimb);								
				Memory::Free(Memory::DefaultHeap, m_triareas);		
			}		
			nvx2Reader->Close();
		}				
		

	}
	
	//
	// Step 2. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(&m_ctx, config->walkableClimb, *m_solid);
	rcFilterLedgeSpans(&m_ctx, config->walkableHeight, config->walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(&m_ctx, config->walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	
	rcBuildCompactHeightfield(&m_ctx, config->walkableHeight, config->walkableClimb, *m_solid, *m_chf);
	

	
	rcFreeHeightField(m_solid);
	m_solid = 0;
	

	// Erode the walkable area by agent radius.
	rcErodeWalkableArea(&m_ctx, config->walkableRadius, *m_chf);
	
    // add area markers
    
    Math::point verts[8];   
    verts[0].set(-0.5f, 0.5f, 0.5f);
    verts[1].set(0.5f, 0.5f, 0.5f);
    verts[2].set(-0.5f, -0.5f, 0.5f);
    verts[3].set(0.5f, -0.5f, 0.5f);
    verts[4].set(-0.5f, 0.5f, -0.5f);
    verts[5].set(0.5f, 0.5f, -0.5f);
    verts[6].set(-0.5f, -0.5f, -0.5f);
    verts[7].set(0.5f, -0.5f, -0.5);
	Math::point corners[4];
	corners[0].set(-0.5f, 0.0f, -0.5f);
	corners[1].set(-0.5f, 0.0f, 0.5f);
	corners[2].set(0.5f, 0.0f, 0.5f);
	corners[3].set(0.5f, 0.0f, -0.5f);	
    float boxData[12];
    float pv[4];
    for(int i = 0 ; i<this->areaEntities.Size();i++)
    {
        const Ptr<Game::Entity> & ent = this->areaEntities[i];
        // currently we only do boxes
        Math::matrix44 trans = ent->GetMatrix44(Attr::Transform);
        int areaId = ent->GetInt(Attr::NavMeshAreaCost);
		int areaFlags = ent->GetInt(Attr::NavMeshAreaFlags);
        float maxHeight = -FLT_MAX;
        float maxDepth = FLT_MAX;
        for(int j = 0 ; j<8;j++)
        {
            Math::point p = Math::matrix44::transform(verts[j],trans);            
            maxHeight = maxHeight < p.y() ? p.y() : maxHeight;
            maxDepth = maxDepth > p.y() ? p.y() : maxDepth;
        }
		for (int j = 0; j < 4; j++)
		{
			Math::point p = Math::matrix44::transform(corners[j], trans);
			p.storeu(pv);
			rcVcopy(boxData + 3 * j, pv);
		}
		areaId = (areaId << 8) + areaFlags;
        rcMarkConvexPolyArea(&m_ctx, boxData, 4, maxDepth, maxHeight, areaId, *m_chf);        
    }

	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	rcBuildDistanceField(&m_ctx, *m_chf);
	

	// Partition the walkable surface into simple regions without holes.
	rcBuildRegions(&m_ctx, *m_chf, 0, config->minRegionArea, config->mergeRegionArea);

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	
	rcBuildContours(&m_ctx, *m_chf, config->maxSimplificationError, config->maxEdgeLen, *m_cset);
	

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	
	rcBuildPolyMesh(&m_ctx, *m_cset, config->maxVertsPerPoly, *m_pmesh);
	
	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	
	rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf, config->detailSampleDist, config->detailSampleMaxError, *m_dmesh);
	
	
	rcFreeCompactHeightfield(m_chf);
	m_chf = 0;
	rcFreeContourSet(m_cset);
	m_cset = 0;
	

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	n_assert(config->maxVertsPerPoly <= DT_VERTS_PER_POLYGON);
	
	unsigned char* navData = 0;
	int navDataSize = 0;

	
	// Update poly flags from areas.
	for (int i = 0; i < m_pmesh->npolys; ++i)
	{
		if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
		{
			m_pmesh->areas[i] = 1;
			m_pmesh->flags[i] = 1;
		}
		else
		{
			// custom area
			int id = m_pmesh->areas[i];
			m_pmesh->flags[i] = id & 255;
			m_pmesh->areas[i] = id >> 8;
		}
		
	}


	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = m_pmesh->verts;
	params.vertCount = m_pmesh->nverts;
	params.polys = m_pmesh->polys;
	params.polyAreas = m_pmesh->areas;
	params.polyFlags = m_pmesh->flags;
	params.polyCount = m_pmesh->npolys;
	params.nvp = m_pmesh->nvp;
	params.detailMeshes = m_dmesh->meshes;
	params.detailVerts = m_dmesh->verts;
	params.detailVertsCount = m_dmesh->nverts;
	params.detailTris = m_dmesh->tris;
	params.detailTriCount = m_dmesh->ntris;
	/*
	params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	params.offMeshConUserID = m_geom->getOffMeshConnectionId();
	params.offMeshConCount = m_geom->getOffMeshConnectionCount();
	*/
	
	params.walkableHeight = agentHeight;
	params.walkableRadius = agentRadius;
	params.walkableClimb = agentMaxClimb;
	rcVcopy(params.bmin, m_pmesh->bmin);
	rcVcopy(params.bmax, m_pmesh->bmax);
	params.cs = config->cs;
	params.ch = config->ch;
	params.buildBvTree = true;

	dtCreateNavMeshData(&params, &navData, &navDataSize);


	Util::Blob blob;
	if(navDataSize>0)
	{
		dtNavMesh blorf;
		blorf.init(navData,navDataSize,0);
		blob.Set(navData, navDataSize);
	}
	return blob;

}

//------------------------------------------------------------------------------
/**
*/
bool
RecastUtil::LoadNavMeshGenerationData(const Ptr<Db::Reader>& reader)
{
    n_assert(reader->IsOpen());
    this->agentRadius = reader->GetFloat(Attr::AgentRadius);
    this->agentHeight = reader->GetFloat(Attr::AgentHeight);
    this->maxEdgeError = reader->GetFloat(Attr::MaxEdgeError);
    this->detailSampleDist = reader->GetFloat(Attr::DetailSampleDist);
    this->maxSlope = reader->GetFloat(Attr::MaxSlope);
    this->cellHeight = reader->GetFloat(Attr::CellHeight);
    this->cellSize = reader->GetFloat(Attr::CellSize);
    this->agentMaxClimb = reader->GetFloat(Attr::AgentMaxClimb);
    this->detailSampleMaxError = reader->GetFloat(Attr::DetailSampleMaxError);
    this->maxEdgeLength = reader->GetInt(Attr::MaxEdgeLength);
    this->regionMinSize = reader->GetInt(Attr::RegionMinSize);
    this->regionMergeSize = reader->GetInt(Attr::RegionMergeSize);
	this->id = reader->GetString(Attr::Id);

	Math::point center = reader->GetFloat4(Attr::NavMeshCenter);
	Math::vector extends = reader->GetFloat4(Attr::NavMeshExtends);
	
	this->boundingBox.set(center, extends);

    String meshString = reader->GetString(Attr::NavMeshMeshString);
    Dictionary<String,String> pairs = String::ParseKeyValuePairs(meshString);

    for(int i = 0 ; i < pairs.Size() ; i++)
    {        
        this->AddMesh(pairs.KeyAtIndex(i),pairs.ValueAtIndex(i).AsMatrix44());        		
    }

	this->SetupConfig();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RecastUtil::AddConvexArea( const Ptr<Game::Entity> & areaEntity )
{
    this->areaEntities.Append(areaEntity);
}


}