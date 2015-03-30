//------------------------------------------------------------------------------
//  viewerstresstestapplication.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viewerstresstestapplication.h"

namespace Test
{
using namespace Resources;
using namespace Math;
using namespace Util;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
ViewerStressTestApplication::ViewerStressTestApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ViewerStressTestApplication::~ViewerStressTestApplication()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerStressTestApplication::GetRandomTransform(matrix44& m)
{
    point pos(n_rand() * 100.0f, n_rand() * 100.0f, n_rand() * 100.0f);
    m.set_position(pos);
}

//------------------------------------------------------------------------------
/**
*/
bool
ViewerStressTestApplication::Open()
{
    n_assert(!this->IsOpen());
    if (ViewerApplication::Open())
    {
        // re-position the camera so that something is visible
        this->mayaCameraUtil.Setup(point(50.0f, 50.0f, 50.0f), point(50.0f, 50.0f, 120.0f), vector(0.0f, 1.0f, 0.0f));

        // setup a global light
        matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-70.0f));
        this->globalLight = GlobalLightEntity::Create();
        this->globalLight->SetTransform(lightTransform);
        this->globalLight->SetColor(float4(1.0f, 1.0f, 1.0f, 1.0f));
        this->globalLight->SetBackLightColor(float4(0.0f, 0.0f, 0.5f, 0.0f));
        this->globalLight->SetCastShadows(false);
        this->stage->AttachEntity(this->globalLight.cast<GraphicsEntity>());

        // setup a thousand graphics objects
        ResourceId modelResId("mdl:examples/box.n2");
        matrix44 modelTransform = matrix44::identity();
        const SizeT num = 1000;
        this->modelEntities.SetSize(num);
        IndexT i;
        for (i = 0; i < num; i++)
        {
            this->GetRandomTransform(modelTransform);
            this->modelEntities[i] = ModelEntity::Create();
            this->modelEntities[i]->SetTransform(modelTransform);
            this->modelEntities[i]->SetResourceId(modelResId);
            this->stage->AttachEntity(this->modelEntities[i].cast<GraphicsEntity>());
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerStressTestApplication::Close()
{
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        this->stage->RemoveEntity(this->modelEntities[i].cast<GraphicsEntity>());
        this->modelEntities[i] = 0;
    }
    this->stage->RemoveEntity(this->globalLight.cast<GraphicsEntity>());
    this->globalLight = 0;
    ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerStressTestApplication::OnUpdateFrame()
{
    matrix44 modelTransform = matrix44::identity();
    IndexT i;
    for (i = 0; i < this->modelEntities.Size(); i++)
    {
        this->GetRandomTransform(modelTransform);
        this->modelEntities[i]->SetTransform(modelTransform);
    }
    ViewerApplication::OnUpdateFrame();
}

} // namespace Test