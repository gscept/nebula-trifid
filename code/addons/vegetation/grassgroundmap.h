#ifndef VEGETATIONFEATURE_GRASSGROUNDMAP_H
#define VEGETATIONFEATURE_GRASSGROUNDMAP_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassGroundMap
    
    Provide color and normal information about underlying ground for the
    grass renderer. This contains 2 offscreen views which render
    the color and normal information of the ground objects around 
    the viewer, these are used by the grass pixel shader for coloring
    and lighting.
    
    (C) 2008 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "internalgraphics/internalview.h"
#include "coregraphics/texture.h"

namespace InternalGraphics
{
    class InternalCameraEntity;
}
//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPatchCoords;

class GrassGroundMap : public Core::RefCounted
{
    __DeclareClass(GrassGroundMap);
public:
    /// constructor
    GrassGroundMap();
    /// destructor
    virtual ~GrassGroundMap();

    /// setup the object
    void Setup();
    /// update the color and normal maps, only call this when needed!
    void Update(const GrassPatchCoords& origin, const Math::vector& cameraPos);
    /// return true if the render targets are valid (may become invalid on Alt-TAB etc...)
    bool NeedsUpdate() const;

    /// begin looking up a patch map value
    bool BeginLookupPatchMap();
    /// lookup a RGBA value from the patch map
    uint LookupPatchMap(const GrassPatchCoords& origin, const GrassPatchCoords& patchCoord) const;
    /// finish looking up patch map values
    void EndLookupPatchMap();

    /// compute a start index into the weight map by patch coordinate
    uint ComputeWeightMapStartIndex(const GrassPatchCoords& patchCoord) const;
    /// compute a patch-local per-vertex index into the weight map
    uint ComputeWeightMapVertexIndex(float x, float z) const;
    /// begin looking up weight map values (locks the system-mem surface)
    bool BeginLookupWeightMap();
    /// lookup an RGBA value from the weight map
    uint LookupWeightMap(uint index);
    /// end looking up weight map values (unlocks the system mem surface)
    void EndLookupWeightMap();

private:
    const int colorMapSizeInPixels;
    const int normalMapSizeInPixels;
    const int lightMapSizeInPixels;
    const int weightMapSizeInPixels;
    Ptr<InternalGraphics::InternalView> view;
    Ptr<CoreGraphics::RenderTarget> patchRenderTarget;
    Ptr<CoreGraphics::Texture> patchMapSurface;   // system ram surface for fast read access
    Ptr<CoreGraphics::Texture> weightMapSurface;        
    CoreGraphics::Texture::MapInfo weightMapLockInfo;
    CoreGraphics::Texture::MapInfo patchMapLockInfo;
    uint weightMapByteSize;
    uint frameCounter;
    Ptr<InternalGraphics::InternalStage> stage;
    Ptr<InternalGraphics::InternalCameraEntity> viewCam;    
};

//------------------------------------------------------------------------------
/**
    Lookup a weight RGBA value in the weight map surface with a pre-computed
    byte lookup index. Must be called between BeginLookupWeightMap() and 
    EndLookupWeightMap(). NOTE: this method must be extremely fast, so no 
    sanity checks will be performed on the input.
*/
inline
uint
GrassGroundMap::LookupWeightMap(uint index)
{
    uint* ptr = (uint*)(((uchar*)this->weightMapLockInfo.data) + index);
    //n_assert(index < this->weightMapByteSize);
    return *ptr;
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
