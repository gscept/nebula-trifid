//------------------------------------------------------------------------------
//  grassheightmap.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grassheightmap.h"
#include "vegetation/grassrenderer.h"
#include "frame/frameserver.h"
#include "frame/frameshader.h"
#include "frame/framepass.h"
#include "internalgraphics/internalstage.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/simplestagebuilder.h"
#include "coregraphics/renderdevice.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassHeightMap, 'VGHM', Core::RefCounted);

using namespace InternalGraphics;
using namespace CoreGraphics;
using namespace Frame;
using namespace Resources;
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
GrassHeightMap::GrassHeightMap() :
    sizeInPixels(256),
    inBegin(false),
    lookupSurfPitch(0),
    lookupScale(0.0f),
    lookupMax(0),
    lookupAdd(0),
    frameCounter(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GrassHeightMap::~GrassHeightMap()
{
    this->stage->RemoveAllEntities();
    this->view->SetStage(0);
    this->view->SetFrameShader(0);
    this->stage = 0;
    this->view = 0;

    if (this->sysMemSurface.isvalid())
    {
        this->sysMemSurface = 0;
        //this->sysMemSurface->Release();
    }
}

//------------------------------------------------------------------------------
/**
    This sets up the various resources (offscreen renderer, system ram buffer,
    etc...).
*/
void
GrassHeightMap::Setup()
{
    n_assert(!this->view.isvalid());
    n_assert(!this->sysMemSurface.isvalid());

    const Ptr<FrameShader>& heightFrameShader = FrameServer::Instance()->GetFrameShaderByName(ResourceId("GrassHeightMap"));
    const Ptr<FramePass>& framepass = heightFrameShader->GetFramePassByName(ResourceId("GrassHeightMapPass"));
    const Ptr<RenderTarget>& renderTarget = framepass->GetRenderTarget();
    this->sysMemSurface = renderTarget->GetCPUResolveTexture();
    
    // initialize some texture lookup values
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    this->lookupMax = this->sizeInPixels - 1;
    this->lookupScale = float(this->sizeInPixels) / grassRenderer->GetPatchSize();
    this->lookupAdd = this->sizeInPixels / 2;
    bool locked = this->sysMemSurface->Map(0, Texture::MapRead, this->lockInfo);
    n_assert(locked);
    this->lookupSurfPitch = this->lockInfo.rowPitch;
    this->sysMemSurface->Unmap(0);

    // setup the stage and view
    const Ptr<SimpleStageBuilder>& stageBuilder = SimpleStageBuilder::Create();
    this->stage = InternalGraphicsServer::Instance()->CreateStage(StringAtom("GrassHeightStage"), stageBuilder.cast<StageBuilder>());

    this->view = InternalView::Create();
    this->view->SetFrameShader(heightFrameShader);    
    this->view->SetStage(this->stage);    

    this->cam = InternalCameraEntity::Create();
}

//------------------------------------------------------------------------------
/**
    This is the core method. It will setup an orthogonal view matrix,
    and render the visible ground entities into the float offscreen rendertarget
    with a special depth-only shader. The result will be copied back into
    the system RAM buffer, which is faster for random-read-access by the
    Lookup() method. The extra viewer position is required to get the height
    of the viewer in world space for near/far plane setup.
*/
void
GrassHeightMap::Update(const GrassPatchCoords& patchCoord, const vector& cameraPos)
{
    RenderDevice* renderDevice = RenderDevice::Instance();
    renderDevice->BeginFrame();
    // create a viewer matrix which looks downward at the 
    // world space position of the patch
    matrix44 viewMatrix = matrix44::rotationx(n_deg2rad(-90.0f));
    point pos = point(patchCoord.GetAsWorldSpace().x(),
                                  cameraPos.y() + 50.0f,
                                  patchCoord.GetAsWorldSpace().z());    
    viewMatrix.set_position(pos);

    // create an orthogonal projection matrix as wide as a grass patch,
    // the far plane is positioned at the maximum distance where grass 
    // would be visible
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    float grassPatchSize = grassRenderer->GetPatchSize();
    this->cam->SetupOrthogonal(grassPatchSize, grassPatchSize, 0.1f, 200.0f);
    this->cam->SetTransform(viewMatrix);

    // setup the offscreen renderer's camera
    this->stage->AttachEntity(this->cam.cast<InternalGraphics::InternalGraphicsEntity>());
    this->view->SetCameraEntity(this->cam);
    this->view->ApplyCameraSettings();

    // clip ground entities against camera and add visible entities only
    const Util::Dictionary<Util::String,Ptr<InternalModelEntity> >& groundEntities = grassRenderer->GroundEntities();
    int i;
    for (i = 0; i < groundEntities.Size(); i++)
    {
        Ptr<InternalModelEntity> groundEntity = groundEntities.ValueAtIndex(i);
        ClipStatus::Type clipStatus;
        clipStatus = this->cam->ComputeClipStatus(groundEntity->GetGlobalBoundingBox());
        if (ClipStatus::Outside != clipStatus)
        {
            this->stage->AttachEntity(groundEntity.cast<InternalGraphicsEntity>());
        }
    }
    this->stage->UpdateEntities(0, frameCounter++);
    // update visibility from the view's camera
    this->view->UpdateVisibilityLinks();
    // perform offscreen rendering
    this->view->Render();
    this->stage->RemoveAllEntities();
    renderDevice->EndFrame();

    // texture is automatically resolved by rendertarget   
}

//------------------------------------------------------------------------------
/**
    Begin looking up values in the height map. This will first update
    the height map, then lock the system mem surface, so that LookupHeight()
    doesn't have to do this every time.
*/
bool
GrassHeightMap::Begin(const GrassPatchCoords& patchCoords, const vector& cameraPos)
{
    n_assert(!this->inBegin);
    Ptr<GrassRenderer> grassRenderer = GrassRenderer::Instance();
    this->inBegin = true;
    this->Update(patchCoords, cameraPos);
    bool locked = this->sysMemSurface->Map(0, Texture::MapRead, this->lockInfo);
    this->inBegin = locked;
    return locked;
}

//------------------------------------------------------------------------------
/**
    End looking up heights.
*/
void
GrassHeightMap::End()
{
    n_assert(this->inBegin);
    this->sysMemSurface->Unmap(0);
    this->lockInfo.data = 0;
    this->lockInfo.rowPitch = 0;
    this->inBegin = false;
}

} // namespace Vegetation

