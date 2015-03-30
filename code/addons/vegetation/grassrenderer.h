#ifndef VEGETATION_GRASSRENDERER_H
#define VEGETATION_GRASSRENDERER_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassRenderer

    A complete grass renderer for Mangalore. Can also be used for small
    ground rubble etc... The grass renderer manages an 2D array of
    active grass patches around the current viewer position and updates
    and renders them efficiently.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "vegetation/grasspatchcoords.h"
#include "vegetation/grasslayer.h"
#include "vegetation/grasslibrary.h"
#include "vegetation/grasslayerinfo.h"
#include "internalgraphics/internalmodelentity.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPatchArray;

class GrassRenderer : public Core::RefCounted
{
    __DeclareClass(GrassRenderer);
    __DeclareSingleton(GrassRenderer);
public:
    /// constructor
    GrassRenderer();
    /// destructor
    virtual ~GrassRenderer();

    /// enable/disable grass rendering
    void SetGrassRenderingEnabled(bool b);
    /// return true if grass rendering is enabled
    bool IsGrassRenderingEnabled() const;
    /// toggle grass rendering
    void ToggleGrassRendering();
    /// set the size of a grass patch
    void SetPatchSize(float s);
    /// get the size of a grass patch
    float GetPatchSize() const;
    /// set the size of the active patch array around the viewer
    void SetPatchArraySize(int s);
    /// get the active patch array size
    int GetPatchArraySize() const;
    /// set the number of plants per x/y dimension in a grass patch
    void SetNumPlantsPerPatchDimension(int n);
    /// get the number of plants per x/y dimension in a grass patch
    int GetNumPlantsPerPatchDimension() const;
    /// set the maximum random plant spread (0.0 .. 1.0) for plants, zero will result in an even grid
    void SetMaxRandomPlantSpread(float s);
    /// get the maximum random plant spread
    float GetMaxRandomPlantSpread() const;

    /// scan graphics entities for grass attributes
    void ScanGraphicsEntities(const Util::Array<Ptr<InternalGraphics::InternalGraphicsEntity> > & entities);
    /// setup grass layers from graphics entity (if it has grass layers)
    void SetupGrassLayersFromGraphicsEntity(const Ptr<InternalGraphics::InternalModelEntity>& gfxEntity);

    /// open the grass renderer
    bool Open();
    /// close the grass renderer
    void Close();
    /// return true if grass renderer is open
    bool IsOpen() const;
    /// update the grass renderer, call per frame
    void Update();
    /// get current frameid
    uint GetFrameId() const;

private:
    friend class GrassLayer;
    friend class GrassPatchArray;
    friend class GrassHeightMap;
    friend class GrassPatch;
    friend class GrassGroundMap;

    /// add a grass layer
    void AddGrassLayer(const GrassLayerInfo& grassLayerInfo);
    /// traverse thru hierarchy and scan for grass attributes
    void ScanNodesForGrassLayer(const Ptr<InternalGraphics::InternalModelEntity> gfxEntity, Util::Array<Ptr<Models::ModelNode> > nodes);
    /// setup a single grass layer from a Nebula2 multilayer node
    void SetupGrassLayerFromMultilayerNode(const Ptr<InternalGraphics::InternalModelEntity> gfxEntity, const Util::StringAtom& attrName, const Util::StringAtom& attrValue);
    
    /// access to grass libraries
    Util::Dictionary<Util::String, Ptr<GrassLibrary> >& Libraries();
    /// access to grass layers
    Util::Dictionary<Util::String, Ptr<GrassLayer> >& Layers();
    /// access to ground objects
    Util::Dictionary<Util::String,Ptr<InternalGraphics::InternalModelEntity> >& GroundEntities();
    /// access to grass height map
    Ptr<GrassHeightMap> HeightMap();
    /// access to ground map
    Ptr<GrassGroundMap> GroundMap();
    /// access to grass patch array
    Ptr<GrassPatchArray> PatchArray();
    /// get the current camera position
    const Math::matrix44& GetCameraTransform() const;
    /// validate resources
    void ValidateResources();

    bool isOpen;
    bool grassRenderingEnabled;
    bool allResourcesValid;
    int patchArraySize;
    int numPlantsPerPatchDimension;
    float maxRandomPlantSpread;
    Math::matrix44 cameraTransform;
    
    Util::Dictionary<Util::String, Ptr<GrassLibrary> > libraries;
    Util::Dictionary<Util::String, Ptr<InternalGraphics::InternalModelEntity> > groundEntities;
    Util::Dictionary<Util::String, Ptr<GrassLayer> > layers;
    Ptr<GrassHeightMap> heightMap;
    Ptr<GrassGroundMap> groundMap;
    Ptr<GrassPatchArray> patchArray;
    uint frameId;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassRenderer::IsGrassRenderingEnabled() const
{
    return this->grassRenderingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassRenderer::ToggleGrassRendering()
{
    this->SetGrassRenderingEnabled(!this->grassRenderingEnabled);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassRenderer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassRenderer::SetNumPlantsPerPatchDimension(int n)
{
    n_assert(n > 0);
    this->numPlantsPerPatchDimension = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassRenderer::GetNumPlantsPerPatchDimension() const
{
    return this->numPlantsPerPatchDimension;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassRenderer::SetMaxRandomPlantSpread(float s)
{
    this->maxRandomPlantSpread = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
GrassRenderer::GetMaxRandomPlantSpread() const
{
    return this->maxRandomPlantSpread;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassRenderer::SetPatchSize(float s)
{
    GrassPatchCoords::SetGrassPatchSize(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
GrassRenderer::GetPatchSize() const
{
    return GrassPatchCoords::GetGrassPatchSize();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassRenderer::SetPatchArraySize(int numPatches)
{
    n_assert(!this->IsOpen());
    n_assert(numPatches & 1);   // must be an odd number
    this->patchArraySize = numPatches;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassRenderer::GetPatchArraySize() const
{
    return this->patchArraySize;
}

//------------------------------------------------------------------------------
/**
*/
inline uint 
GrassRenderer::GetFrameId() const
{
    return this->frameId;
}
} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
