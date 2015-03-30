//------------------------------------------------------------------------------
//  grassgroundmap.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grassgroundmap.h"
#include "vegetation/grasspatchcoords.h"
#include "vegetation/grassrenderer.h"
#include "vegetation/grasspatcharray.h"
#include "frame/frameserver.h"
#include "frame/frameshader.h"
#include "frame/framepass.h"
#include "internalgraphics/internalstage.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/simplestagebuilder.h"
#include "coregraphics/renderdevice.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassGroundMap, 'VGGM', Core::RefCounted);

using namespace InternalGraphics;
using namespace CoreGraphics;
using namespace Math;
using namespace Frame;
using namespace Util;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
GrassGroundMap::GrassGroundMap() :
    colorMapSizeInPixels(256),
    normalMapSizeInPixels(256),
    lightMapSizeInPixels(256),
    weightMapSizeInPixels(256),
    weightMapByteSize(0),
    frameCounter(0)
{
}

//------------------------------------------------------------------------------
/**
*/
GrassGroundMap::~GrassGroundMap()
{
    this->stage->RemoveAllEntities();
    this->stage = 0;
    this->viewCam = 0;
    this->view->SetStage(0);
    this->view->SetFrameShader(0);
    this->view->SetRenderTarget(0);
    this->view->SetMultipleRenderTarget(0);
    this->view = 0;  

    if (this->patchMapSurface.isvalid())
    {
        this->patchMapSurface = 0;        
    }
    if (this->weightMapSurface.isvalid())
    {
        this->weightMapSurface = 0;        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GrassGroundMap::Setup()
{
    n_assert(!this->view.isvalid());

    /// get internal stage from default view
    const Ptr<SimpleStageBuilder>& stageBuilder = SimpleStageBuilder::Create();
    this->stage = InternalGraphicsServer::Instance()->CreateStage(StringAtom("GrassStage"), stageBuilder.cast<StageBuilder>());

    // setup the color offscreen views
    this->view = InternalView::Create();
    const Ptr<FrameShader>& frameShader = FrameServer::Instance()->GetFrameShaderByName(ResourceId("GrassAttributes"));
    this->view->SetFrameShader(frameShader);    
    this->view->SetStage(this->stage);
    this->viewCam = InternalCameraEntity::Create();
    this->stage->AttachEntity(this->viewCam.cast<InternalGraphicsEntity>());
    this->view->SetCameraEntity(this->viewCam);    
    
    // get rendertargets
    const Ptr<FrameShader>& frameshader = this->view->GetFrameShader();
    const Ptr<FramePass>& framepass = frameshader->GetFramePassBaseByName(ResourceId("GrassPatchMapPass"));
    const Ptr<RenderTarget>& renderTarget = framepass->GetRenderTarget();
    this->patchMapSurface = renderTarget->GetCPUResolveTexture();

    const Ptr<FramePass>& framepass1 = frameshader->GetFramePassBaseByName(ResourceId("GrassColorLightNormalWeightMapPass"));
    const Ptr<MultipleRenderTarget>& mrt1 = framepass1->GetMultipleRenderTarget();    
    this->weightMapSurface = mrt1->GetRenderTarget(3)->GetCPUResolveTexture();  
    this->view->SetMultipleRenderTarget(mrt1);
}

//------------------------------------------------------------------------------
/**
    The core rendering method. This will setup an orthogonal view matrix
    which looks downward onto the ground and update the render targets
    in the offscreen views. This method should only be called
    when the viewer crosses a patch boundary or when the NeedsUpdate()
    method returns true.
*/
void
GrassGroundMap::Update(const GrassPatchCoords& origin, const vector& cameraPos)
{
    // create a viewer matrix which looks downward at the world space 
    // origin of the patch array
    Math::matrix44 viewMatrix = matrix44::rotationx(Math::n_deg2rad(-90.0f));

    // create an orthogonal projection matrix as wide as the grass patch array
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    const float patchAreaSize = grassRenderer->GetPatchSize() * grassRenderer->GetPatchArraySize();
    const float nearPlane = 0.1f;
    const float farPlane  = 200.0f;
    
    RenderDevice* renderDevice = RenderDevice::Instance();
    renderDevice->BeginFrame();
    // perform offscreen rendering
    // need to shift the view matrix by a half pixel
    float rtWidth;
    float rtHeight;
    if (this->view->GetRenderTarget().isvalid())
    {
        rtWidth = (float) this->view->GetRenderTarget()->GetWidth();
        rtHeight = (float) this->view->GetRenderTarget()->GetHeight();
    }
    else
    {
        const Ptr<RenderTarget>& rt = this->view->GetMultipleRenderTarget()->GetRenderTarget(0);
        rtWidth = (float) rt->GetWidth();
        rtHeight = (float) rt->GetHeight();
    }
    float halfPixelWidth = (patchAreaSize / rtWidth) * 0.5f;
    float halfPixelHeight = (patchAreaSize / rtHeight) * 0.5f;
    point rtCameraPos = origin.GetAsWorldSpace() + vector(halfPixelWidth, 0.0f, halfPixelHeight);
    rtCameraPos.y() = cameraPos.y() + 100.0f;
    viewMatrix.set_position(rtCameraPos);

    // setup the offscreen view's camera
    const Ptr<InternalCameraEntity>& cameraEntity = this->viewCam;
    cameraEntity->SetupOrthogonal(patchAreaSize, patchAreaSize, nearPlane, farPlane);
    cameraEntity->SetTransform(viewMatrix);  
    if (!cameraEntity->IsAttachedToStage())
    {
        this->stage->AttachEntity(cameraEntity.cast<InternalGraphicsEntity>());
    }
    this->view->SetCameraEntity(cameraEntity);        
    this->view->ApplyCameraSettings();

    // clip ground entities against camera and add visible entities only
    const Util::Dictionary<Util::String, Ptr<InternalModelEntity> >& groundEntities = grassRenderer->GroundEntities();
    int i;
    for (i = 0; i < groundEntities.Size(); i++)
    {
        Ptr<InternalModelEntity> groundEntity = groundEntities.ValueAtIndex(i);
        ClipStatus::Type clipStatus;
        clipStatus = cameraEntity->ComputeClipStatus(groundEntity->GetGlobalBoundingBox());
        if (ClipStatus::Outside != clipStatus)
        {
            this->stage->AttachEntity(groundEntity.cast<InternalGraphicsEntity>());
        }
    }
    this->stage->UpdateEntities(0.0, frameCounter++);
    // update visibility from the view's camera
    this->view->UpdateVisibilityLinks();

    // perform offscreen rendering and cleanup
    this->view->Render();
    this->stage->RemoveAllEntities();    
    renderDevice->EndFrame();
}

//------------------------------------------------------------------------------
/**
    Returns true if render targets need an out-of-order-update. This is the
    case when the 3D device was lost and thus the existing render target
    content has become invalid. 
*/
bool
GrassGroundMap::NeedsUpdate() const
{
    /*
    const Ptr<FrameShader>& frameshader1 = this->view->GetFrameShader();
    const Ptr<FramePass>& framepass1 = frameshader1->GetFramePassByName(ResourceId("GrassColorLightNormalWeightMapPass"));
    const Ptr<MultipleRenderTarget>& renderTarget1 = framepass1->GetMultipleRenderTarget();   
    */
    // @todo: handle lost device event
    return false;
}

//------------------------------------------------------------------------------
/**
    Begin looking up patch map values.
*/
bool
GrassGroundMap::BeginLookupPatchMap()
{
    bool locked = this->patchMapSurface->Map(0, Texture::MapRead, this->patchMapLockInfo);
    return locked;
}

//------------------------------------------------------------------------------
/**
    Lookup a patch map RGBA value.
*/
uint
GrassGroundMap::LookupPatchMap(const GrassPatchCoords& origin, const GrassPatchCoords& patchCoord) const
{
    // we need to average 4 lookups in order because of aliasing problem in the low-resolution patch map

    // compute top-left coordinates
    const int patchArraySize = GrassRenderer::Instance()->GetPatchArraySize();
    const int halfPatchArraySize = patchArraySize / 2;
    const int x = (patchCoord.x - origin.x) + halfPatchArraySize;
    const int y = (patchCoord.y - origin.y) + halfPatchArraySize;
    n_assert((x >= 0) && (x < patchArraySize));
    n_assert((y >= 0) && (y < patchArraySize));

    // we're doing 4 lookups and average them
    const int patchRtSize = patchArraySize * 2;
    int x0 = 2 * x;  int y0 = 2 * y;
    int x1 = x0 + 1; int y1 = y0 + 1;
    if (x1 >= patchRtSize) x1 = patchRtSize - 1;
    if (y1 >= patchRtSize) y1 = patchRtSize - 1;
    x0 *= sizeof(uint);
    x1 *= sizeof(uint);
    y0 *= this->patchMapLockInfo.rowPitch;
    y1 *= this->patchMapLockInfo.rowPitch;
    
    // lookup RGBA value from patch map, channels are != 0
    // if the associated grass type exists in this patch
    uint* ptr00 = (uint*)(((uchar*)this->patchMapLockInfo.data) + x0 + y0);
    uint* ptr10 = (uint*)(((uchar*)this->patchMapLockInfo.data) + x1 + y0);
    uint* ptr01 = (uint*)(((uchar*)this->patchMapLockInfo.data) + x0 + y1);
    uint* ptr11 = (uint*)(((uchar*)this->patchMapLockInfo.data) + x1 + y1);
    
    // we're only interested in the upper 4 bit per channel, since the lower bits
    // contain noise which we're not interested in, also we need the space
    // for a clean shifting operation
    uint rgb0 = (*ptr00 & 0xF0F0F0F0) >> 4;
    uint rgb1 = (*ptr10 & 0xF0F0F0F0) >> 4;
    uint rgb2 = (*ptr01 & 0xF0F0F0F0) >> 4;
    uint rgb3 = (*ptr11 & 0xF0F0F0F0) >> 4;
    uint result = rgb0 + rgb1 + rgb2 + rgb3;
    return result;
}

//------------------------------------------------------------------------------
/**
    End looking up patch map values.
*/
void
GrassGroundMap::EndLookupPatchMap()
{
    this->patchMapSurface->Unmap(0);
    this->patchMapLockInfo.data = 0;
    this->patchMapLockInfo.rowPitch = 0;
}

//------------------------------------------------------------------------------
/**
    Computes a lookup start index for a grass patch at the given patch
    coordinates. This should be computed once when a grass patch becomes
    activated.
*/
uint
GrassGroundMap::ComputeWeightMapStartIndex(const GrassPatchCoords& patchCoord) const
{
    n_assert(this->weightMapLockInfo.rowPitch > 0);
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    GrassPatchCoords relCoords = grassRenderer->PatchArray()->AsLocalPatchCoords(patchCoord);
    n_assert(relCoords.x < grassRenderer->GetPatchArraySize());
    n_assert(relCoords.y < grassRenderer->GetPatchArraySize());
    int patchArraySize = grassRenderer->GetPatchArraySize();
    uint x = (this->weightMapSurface->GetWidth() * relCoords.x) / patchArraySize;
    uint y = (this->weightMapSurface->GetHeight() * relCoords.y) / patchArraySize;
    n_assert(x < uint(this->weightMapSurface->GetWidth()));
    n_assert(y < uint(this->weightMapSurface->GetHeight()));
    uint index = x * sizeof(uint) + y * this->weightMapLockInfo.rowPitch;
    return index;
    return 0;
}

//------------------------------------------------------------------------------
/**
    Compute a local lookup for a grass patch model coordinate. Add this
    to the start index computed with ComputeWeightMapPatchStartIndex()
    to get a resulting lookup index into the weight map. This method
    should be executed per-vertex only when a grass patch is setup and the resulting
    indices should be cached somewhere in the patch object.
*/
uint
GrassGroundMap::ComputeWeightMapVertexIndex(float modelX, float modelZ) const
{
    n_assert(this->weightMapLockInfo.rowPitch > 0);
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();

    // compute a scaling value for the model coordinates to shift
    // them into the range uv quadrant size for a single patch 0..(1/grassPatchSize).
    uint patchArraySize = grassRenderer->GetPatchArraySize();
    uint weightMapSurfaceWidth = this->weightMapSurface->GetWidth();
    uint weightMapSurfaceHeight = this->weightMapSurface->GetHeight();
    float halfPatchSize = grassRenderer->GetPatchSize() * 0.5f;
    float oneDivSize = 1.0f / (grassRenderer->GetPatchSize() * patchArraySize);
    float u = Math::n_saturate((modelX + halfPatchSize) * oneDivSize);
    float v = n_saturate((modelZ + halfPatchSize) * oneDivSize);
    uint x = uint(u * weightMapSurfaceWidth);
    uint y = uint(v * weightMapSurfaceHeight);
    uint maxX = weightMapSurfaceWidth / patchArraySize;
    uint maxY = weightMapSurfaceHeight / patchArraySize;
    if (x >= maxX) x = maxX - 1;
    if (y >= maxY) y = maxY - 1;
    uint index = x * sizeof(uint) + y * this->weightMapLockInfo.rowPitch;
    return index;
}

//------------------------------------------------------------------------------
/**
    Begin looking up weight map values. This will lock the weight map
    system mem surface.
*/
bool
GrassGroundMap::BeginLookupWeightMap()
{
    bool locked = this->weightMapSurface->Map(0, Texture::MapRead, this->weightMapLockInfo);
    return locked;
}

//------------------------------------------------------------------------------
/**
    End looking up weight map values. This will unlock the weight map
    system mem surface.
*/
void
GrassGroundMap::EndLookupWeightMap()
{
    this->weightMapSurface->Unmap(0);
    this->weightMapLockInfo.data = 0;
    // NOTE: don't delete surface pitch!
}

}