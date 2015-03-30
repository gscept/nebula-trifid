#ifndef VEGETATIONFEATURE_GRASSLIBRARY_H
#define VEGETATIONFEATURE_GRASSLIBRARY_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassLibrary
    
    A grass library defines a set of GrassPlantMeshes which all use the
    same shader and texture for rendering, so that plants from the
    library can be rendered in a single draw call. Furthermore a 
    GrassWeightMap is associated with the library, which defines where
    in the level this type of grass is rendered. The grass library
    will pre-generate a number of grass patches which are randomly
    populated with the plants in the library and which will
    be used for rendering.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "vegetation/grasslayerinfo.h"
#include "vegetation/grassplantmesh.h"

namespace Models
{
    class ModelNode;
    class ManagedModel;
};

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPatch;
class GrassPlantMesh;

class GrassLibrary : public Core::RefCounted
{
    __DeclareClass(GrassLibrary);
public:
    /// constructor
    GrassLibrary();
    /// destructor
    virtual ~GrassLibrary();
    /// initialize the grass library
    void Setup(const GrassLayerInfo& grassLayerInfo);
    /// get the resource name
    const Resources::ResourceId& GetResourceId() const;
    /// get the number of plant meshes in the lib
    int GetNumPlantMeshes() const;
    /// get a plant mesh in the lib
    Ptr<GrassPlantMesh> GetPlantMeshAtIndex(int index) const;
    /// "allocate" a grass patch currently deactivated grass patch
    Ptr<GrassPatch> GetFirstFreeGrassPatch();
    /// set grass patches to visible/invisible
    void SetGrassPatchVisibility(bool b);
    /// validate mesh
    void ValidateMesh();
    /// is library valid
    bool IsValid() const;

private:
    /// setup plant mesh objects
    void SetupPlantMeshes();
    /// collect meshes from shapenodes
    void CollectMeshes(const Ptr<Models::ModelNode>& node);
    /// is grasslibrary fully loaded
    bool IsModelLoaded() const;

    Ptr<Resources::ManagedMesh> managedMesh;
    Ptr<Models::ManagedModel> managedModel;
    SizeT numGrassPatches;
    Util::Array<Ptr<GrassPlantMesh> > plantMeshes;
    Util::Array<Ptr<GrassPatch> > patches;
    bool valid;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassLibrary::GetNumPlantMeshes() const
{
    return this->plantMeshes.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
Ptr<GrassPlantMesh>
GrassLibrary::GetPlantMeshAtIndex(int index) const
{
    return this->plantMeshes[index];
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
