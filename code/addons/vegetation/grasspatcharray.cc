//------------------------------------------------------------------------------
//  grasspatcharray.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grasspatcharray.h"
#include "vegetation/grassrenderer.h"
#include "vegetation/grassgroundmap.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassPatchArray, 'VGAR', Core::RefCounted);
 
using namespace Resources;
//------------------------------------------------------------------------------
/**
*/
GrassPatchArray::GrassPatchArray() :
    isOpen(false)
{
    // initialize current origin to some impossible value
    this->origin.x = 100000;
    this->origin.y = 100000;
}

//------------------------------------------------------------------------------
/**
*/
GrassPatchArray::~GrassPatchArray()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
GrassPatchArray::Open()
{
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    this->isOpen = true;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPatchArray::Close()
{
    IndexT i;
    for (i = 0; i < this->activeGrassPatches.Size(); i++)
    {
        if (this->activeGrassPatches[i]->IsActive())
        {
            this->activeGrassPatches[i]->Deactivate();
            this->activeGrassPatches[i] = 0;
        }
    }
    this->activeGrassPatches.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPatchArray::Update(const Math::matrix44& cameraTransform)
{
    // convert camera position to grass patch coordinates
    GrassPatchCoords newOrigin(cameraTransform.get_position());    
    GrassPatchCoords oldOrigin = this->origin;    
    this->origin = newOrigin;

    // check if a grass patch boundary has been crossed
    bool patchBoundaryCrossed = (newOrigin != oldOrigin);

    // update the ground map textures
    Ptr<GrassGroundMap> groundMap = GrassRenderer::Instance()->GroundMap();
    if (patchBoundaryCrossed || groundMap->NeedsUpdate())
    {
        groundMap->Update(newOrigin, cameraTransform.get_position());
    }

    // make sure the active grass patches are valid for rendering
    IndexT i;
    for (i = 0; i < this->activeGrassPatches.Size(); i++)
    {
        if (this->activeGrassPatches[i]->GetResourceState() == Resource::Initial)
        {
            this->activeGrassPatches[i]->Validate();  
            patchBoundaryCrossed = true;
            oldOrigin.Set(-1, -1);
        }        
    }

    if (patchBoundaryCrossed)
    {
        // deactivate grass patches which have become invisible
        Util::Array<GrassPatchCoords> obsoletePatchCoords = this->ComputePatchCoordsToDeactivate(newOrigin, oldOrigin);
        this->DeactivatePatches(obsoletePatchCoords);

        // activate grass patches which have become visible
        Util::Array<GrassPatchCoords> newPatchCoords = this->ComputePatchCoordsToActivate(newOrigin, oldOrigin);
        this->ActivatePatches(newPatchCoords);

        // check if need to update the texture transforms on all grass patches
        int i;
        for (i = 0; i < this->activeGrassPatches.Size(); i++)
        {
            this->activeGrassPatches[i]->OnBoundaryCrossed();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Takes a set of global patch coordinates, and returns local patch 
    coordinates relativ to the current origin.
*/
GrassPatchCoords
GrassPatchArray::AsLocalPatchCoords(const GrassPatchCoords& coords)
{
    GrassPatchCoords rel;
    int halfPatchArraySize =  GrassRenderer::Instance()->GetPatchArraySize() / 2;
    rel.x = (coords.x - this->origin.x) + halfPatchArraySize;
    rel.y = (coords.y - this->origin.y) + halfPatchArraySize;
    return rel;
}

//------------------------------------------------------------------------------
/**
    This method computes a model-space-to-uv-transform for a given grass
    patch coordinate. This is used by the grass patches to lookup
    into the GrassGroundMap textures.

    Patch-vertex coordinates are roughly between (-patchSize,+patchSize)*0.5, but may
    overlap. This needs to be mapped into one of the UV quadrants.
*/
Math::matrix44
GrassPatchArray::ComputeModelToUvTransform(const GrassPatchCoords& coords)
{
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    Math::matrix44 m = Math::matrix44::identity();

    // scale patch coords into the uv quadrant size
    float oneDivSize = 1.0f / (grassRenderer->GetPatchSize() * grassRenderer->GetPatchArraySize());
    m.scale(Math::vector(oneDivSize, 1.0f, oneDivSize));

    // now shift into the right uv quadrant
    GrassPatchCoords rel = this->AsLocalPatchCoords(coords);
    float uvQuadSize = 1.0f / grassRenderer->GetPatchArraySize();
    float halfUvQuadSize = uvQuadSize * 0.5f;
    float shiftU = (rel.x * uvQuadSize) + halfUvQuadSize;
    float shiftV = (rel.y * uvQuadSize) + halfUvQuadSize;
    m.translate(Math::vector(shiftU, 0.0f, shiftV));
    return m;
}

//------------------------------------------------------------------------------
/**
    Returns the indices of all grass patches in the patch array
    which are located at the provided patch coordinates.
*/
Util::Array<IndexT>
GrassPatchArray::FindPatchIndicesByCoords(const GrassPatchCoords& coords)
{
    Util::Array<IndexT> result;
    IndexT i;
    for (i = 0; i < this->activeGrassPatches.Size(); i++)
    {
        if (this->activeGrassPatches[i]->GetPosition() == coords)
        {
            result.Append(i);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    This will deactivate any grass patches in the provided coordinate array
    and remove them from the patchArray. There may be more then one
    patch located at a given patch coordinate.
*/
void
GrassPatchArray::DeactivatePatches(const Util::Array<GrassPatchCoords>& coords)
{
    int coordsIndex;
    for (coordsIndex = 0; coordsIndex < coords.Size(); coordsIndex++)
    {
        const GrassPatchCoords& curCoords = coords[coordsIndex];

        // note: there may be more then one grass patch located at
        // a patch coordinate!
        Util::Array<Ptr<GrassPatch>>::Iterator iter;
        for (iter = this->activeGrassPatches.Begin(); iter != this->activeGrassPatches.End();)
        {
            Ptr<GrassPatch> grassPatch = *iter;
            if (grassPatch->GetPosition() == curCoords)
            {
                if (grassPatch->IsActive())
                {
                    grassPatch->Deactivate();
                }
                iter = this->activeGrassPatches.Erase(iter);
            }
            else
            {
                iter++;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    This will activate any grass patches in the provided coordinate array and
    add them to the patchArray. There may be more then one patch attached
    to a given coordinate.
*/
void
GrassPatchArray::ActivatePatches(const Util::Array<GrassPatchCoords>& coords)
{
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    Ptr<GrassGroundMap> groundMap = grassRenderer->GroundMap();
    const Util::Dictionary<Util::String, Ptr<GrassLayer> >& grassLayers = grassRenderer->Layers();

    groundMap->BeginLookupPatchMap();
    int coordsIndex;
    for (coordsIndex = 0; coordsIndex < coords.Size(); coordsIndex++)
    {
        const GrassPatchCoords& curCoords = coords[coordsIndex];

        // need to find out what type of grass patch must be positioned
        // at the current patch coordinate, this depends on the 
        // painted grass weight and may be between zero and all patches,
        // FIXME:
        // for performance reasons, only one type of patch will be allowed
        // per patch coordinate, we select the grass patch with the highest 
        // value
        int numActivePatches = 0;
        uint patchMapValue = groundMap->LookupPatchMap(this->origin, curCoords);
        if (patchMapValue != 0)
        {
            uint maxChnVal = 0;
            Ptr<GrassPatch> bestGrassPatch = 0;
            int bestGrassPatchChnIndex = 0;
            int rightShifts[] = { 16, 8, 0, 24 };
            int chnIndex;
            for (chnIndex = 0; chnIndex < 4; chnIndex++)
            {
                // look at red/green/blue/alpha
                uint chnVal = (patchMapValue >> rightShifts[chnIndex]) & 0xff;
                if ((chnVal > 0) && (chnVal > maxChnVal))
                {
                    // find the grass layer with the matching channel index, and
                    // only consider the one grass layer with the highest weight
                    int layerIndex;
                    for (layerIndex = 0; layerIndex < grassLayers.Size(); layerIndex++)
                    {
                        if (chnIndex == grassLayers.ValueAtIndex(layerIndex)->GetLayerInfo().GetWeightTextureChannelIndex())
                        {
                            Ptr<GrassLibrary> lib = grassLayers.ValueAtIndex(layerIndex)->GetGrassLibrary();
                            Ptr<GrassPatch> curGrassPatch = lib->GetFirstFreeGrassPatch();
                            if (curGrassPatch.isvalid())
                            {
                                bestGrassPatch = curGrassPatch;
                                bestGrassPatchChnIndex = chnIndex;
                                maxChnVal = chnVal;
                            }
                        }
                    }
                }
            }
            if (bestGrassPatch.isvalid())
            {
                bestGrassPatch->SetPosition(curCoords);
                bestGrassPatch->Activate(bestGrassPatchChnIndex);
                this->activeGrassPatches.Append(bestGrassPatch);
                numActivePatches++;
            }
        }
    }
    groundMap->EndLookupPatchMap();

}

//------------------------------------------------------------------------------
/**
    Computes the grass patch coordinates which need to be deactivated. This
    basically checks what cells in the patch area around the old origin do 
    not overlap with the patch area around the new origin.
*/
Util::Array<GrassPatchCoords>
GrassPatchArray::ComputePatchCoordsToDeactivate(const Vegetation::GrassPatchCoords &newOrigin, 
                                                const Vegetation::GrassPatchCoords &oldOrigin)
{
    Util::Array<GrassPatchCoords> result;

    // shortcut: new origin is identical to old origin
    if (newOrigin == oldOrigin)
    {
        return result;
    }

    // get min and max coords for the 2 patch areas around their origins
    int half = GrassRenderer::Instance()->GetPatchArraySize() / 2;    
    int newMinX = newOrigin.x - half; 
    int newMaxX = newOrigin.x + half;
    int newMinY = newOrigin.y - half;
    int newMaxY = newOrigin.y + half;
    int oldMinX = oldOrigin.x - half;
    int oldMaxX = oldOrigin.x + half;
    int oldMinY = oldOrigin.y - half;
    int oldMaxY = oldOrigin.y + half;

    // get all "old" coords which are not in the "new" area
    int x;
    for (x = oldMinX; x <= oldMaxX; x++)
    {
        int y;
        for (y = oldMinY; y <= oldMaxY; y++)
        {
            if ((x < newMinX) || (x > newMaxX) || (y < newMinY) || (y > newMaxY))
            {
                result.Append(GrassPatchCoords(x, y));
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Computes the grass patch coordinates which need to be activated. This is
    just the reverse operation of ComputePatchCoordsToDeactivate().
*/
Util::Array<GrassPatchCoords>
GrassPatchArray::ComputePatchCoordsToActivate(const Vegetation::GrassPatchCoords &newOrigin, 
                                              const Vegetation::GrassPatchCoords &oldOrigin)
{
    Util::Array<GrassPatchCoords> result;

    // shortcut: new origin is identical to old origin
    if (newOrigin == oldOrigin)
    {
        return result;
    }

    // get min and max coords for the 2 patch areas around their origins
    int half = GrassRenderer::Instance()->GetPatchArraySize() / 2;    
    int newMinX = newOrigin.x - half; 
    int newMaxX = newOrigin.x + half;
    int newMinY = newOrigin.y - half;
    int newMaxY = newOrigin.y + half;
    int oldMinX = oldOrigin.x - half;
    int oldMaxX = oldOrigin.x + half;
    int oldMinY = oldOrigin.y - half;
    int oldMaxY = oldOrigin.y + half;

    // get all "new" coords which are not in the "old" area
    int x;
    for (x = newMinX; x <= newMaxX; x++)
    {
        int y;
        for (y = newMinY; y <= newMaxY; y++)
        {
            if ((x < oldMinX) || (x > oldMaxX) || (y < oldMinY) || (y > oldMaxY))
            {
                result.Append(GrassPatchCoords(x, y));
            }
        }
    }
    return result;
}

} // namespace Vegetation
