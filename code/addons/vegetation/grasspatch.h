#ifndef VEGETATION_GRASSPATCH_H
#define VEGETATION_GRASSPATCH_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassPatch
    
    A grass patch is what actually gets rendered in the end. Grass patches
    contain a renderable mesh which contains the combined plant geometry
    for the patch at sea level. At runtime, dynamic Y-offset data is
    written into the vertex buffer to displace plants by terrain
    height. This only happens when the viewer crosses a patch boundary,
    not at every frame. Grass patches are owned by a GrassLibrary, and
    used by the grass renderer to render the grassy area around the
    viewer.

    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "coregraphics/mesh.h"
#include "models/nodes/shapenode.h"
#include "internalgraphics/internalmodelentity.h"
#include "vegetation/grasspatchcoords.h"
#include "coregraphics/shadervariableinstance.h"
#include "resources/resource.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassHeightMap;
class GrassLibrary;
class GrassWeightMap;

class GrassPatch : public Core::RefCounted
{
    __DeclareClass(GrassPatch);
public:
    /// constructor
    GrassPatch();
    /// destructor
    virtual ~GrassPatch();
    
    /// set the current position in patch coordinates
    void SetPosition(const GrassPatchCoords& p);
    /// get the current position in patch coordinates
    const GrassPatchCoords& GetPosition() const;

    /// initial setup of the grass patch
    void Setup(GrassLibrary* lib, int patchIndex);
    /// validate the grass patch, call this whenever IsValid() returns false
    void Validate();
    /// return true if the patch is valid, this may change when the display mode is changed
    Resources::Resource::State GetResourceState() const;
    /// activate the patch (active patches are currently rendered)
    void Activate(int weightChannelIndex);
    /// deactivate the patch (inactive patches are not currently rendered)
    void Deactivate();
    /// called when viewer crosses a boundary
    void OnBoundaryCrossed();
    /// return true if the patch is active
    bool IsActive() const;
    /// set grass patch visibility (mainly for debugging)
    void SetVisible(bool b);
    /// return true if currently visible (mainly for debugging)
    bool IsVisible() const;

private:
    /// cleanup internal resources
    void Cleanup();
    /// update dynamic vertex components (called in Activate())
    void Update();
    /// setup precomputed height map indices
    void SetupHeightMapIndices();
    /// allocate the height lookup array
    void AllocHeightLookupArray(int numPlants);
    /// setup the precomputed height lookup infos for a plant
    void SetupMapLookupInfos(const Math::vector& plantMidPoint, float* vertexPtr, int firstVertexIndex, int numVertices, int vertexWidth);

    struct LookupInfo
    {
        uint vertexHeightMapIndex;      // index into height map by vertex
        uint vertexWeightMapIndex;      // index into height map by vertex
        uint midPointHeightMapIndex;    // index into height map of midpoint
        float maxHeightDist;            // maximum tolerable height distance between vertex and midpoint
    };

    GrassPatchCoords pos;
    GrassLibrary* grassLibraryBackPointer;      // back pointer to grass lib (thus not a Ptr<>!)
    int grassLibraryPatchIndex;
    Ptr<InternalGraphics::InternalModelEntity> graphicsEntity;
    Ptr<CoreGraphics::ShaderVariableInstance> textureTransformVariable; // texture transform of graphics entities shapenodeinstance
    Ptr<Models::ShapeNode> refShapeNode;
    Ptr<CoreGraphics::Mesh> refMesh;
    LookupInfo* mapLookupArray;  // pre-computed heightmap lookup stuff
    bool isActive;
    float origMinY;
    float origMaxY;
    uint weightMapStartIndex;       // current start index into height map, updated in Activate()
    Math::float4 vecRGBAMask;
    uint intRGBAMask;
    Ptr<Models::ManagedModel> managedModel;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassPatch::IsActive() const
{
    return this->isActive;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassPatch::SetVisible(bool b)
{
    this->graphicsEntity->SetVisible(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassPatch::IsVisible() const
{
    return this->graphicsEntity->IsVisible();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
GrassPatch::SetPosition(const GrassPatchCoords& p)
{
    this->pos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const GrassPatchCoords&
GrassPatch::GetPosition() const
{
    return this->pos;
}

} // namespace GrassPatch
//------------------------------------------------------------------------------
#endif
