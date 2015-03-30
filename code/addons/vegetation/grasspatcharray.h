#ifndef VEGETATION_GRASSPATCHARRAY_H
#define VEGETATION_GRASSPATCHARRAY_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassPatchArray
  
    This implements the 2D array of grass patches around the current
    view point which manages the active set of grass patches which
    need to be rendered.
    
    (C) 2006 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "vegetation/grasspatchcoords.h"
#include "vegetation/grasspatch.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPatchArray : public Core::RefCounted
{
    __DeclareClass(GrassPatchArray);
public:
    /// constructor
    GrassPatchArray();
    /// destructor
    virtual ~GrassPatchArray();

    /// open the grass patch array
    bool Open();
    /// close the grass patch array
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// update the grass patch array, call once per frame
    void Update(const Math::matrix44& cameraTransform);
    /// compute a model-space-to-texture-space matrix for a given grass patch coordinate
    Math::matrix44 ComputeModelToUvTransform(const GrassPatchCoords& coords);
    /// convert global patch coordinates to relative patch coordinates in the range (0..PatchArraySize)
    GrassPatchCoords AsLocalPatchCoords(const GrassPatchCoords& coords);

private:
    /// compute new patch coordinates from old and new origin
    Util::Array<GrassPatchCoords> ComputePatchCoordsToActivate(const GrassPatchCoords& newOrigin, const GrassPatchCoords& oldOrigin);
    /// compute obsolete patch coordinates from old and new origin
    Util::Array<GrassPatchCoords> ComputePatchCoordsToDeactivate(const GrassPatchCoords& newOrigin, const GrassPatchCoords& oldOrigin);
    /// activate "new" grass patches
    void ActivatePatches(const Util::Array<GrassPatchCoords>& patchCoords);
    /// deactivate "obsolete" grass patches
    void DeactivatePatches(const Util::Array<GrassPatchCoords>& patchCoords);
    /// find patch array indices by patch coordinates
    Util::Array<IndexT> FindPatchIndicesByCoords(const GrassPatchCoords& coords);

    bool isOpen;
    GrassPatchCoords origin;    // current origin in patch coordinates
    Util::Array<Ptr<GrassPatch> > activeGrassPatches;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
GrassPatchArray::IsOpen() const
{
    return this->isOpen;
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
