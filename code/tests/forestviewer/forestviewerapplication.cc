//------------------------------------------------------------------------------
//  forestviewerapplication.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "tests/forestviewer/forestviewerapplication.h"
#include "debugrender/debugrender.h"
#include "debugrender/debugshaperenderer.h"
#include "math/quaternion.h"
#include "input/keyboard.h"
#include "input/gamepad.h"

namespace Tools
{
using namespace CoreGraphics;
using namespace Graphics;
using namespace Math;
using namespace Util;
using namespace Resources;
using namespace Timing;
using namespace Debug;
using namespace Input;
using namespace Forest;

//------------------------------------------------------------------------------
/**
*/
ForestViewerApplication::ForestViewerApplication() : 
    avgFPS(0.0f),
    benchmarkmode(false),
    renderDebug(false),
    rotX(-225)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ForestViewerApplication::~ForestViewerApplication()
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
ForestViewerApplication::Open()
{
    n_assert(!this->IsOpen());
    if (ViewerApplication::Open())
    {
        // create forest rendermodule
        this->forestModule = ForestRenderModule::Create();
        this->forestModule->Setup();

        // setup lights
        this->SetupLights();
        
        // setup models
        this->SetupGeometry();

        // wait for resources to be loaded
        GraphicsInterface::Instance()->WaitForPendingResources();
                                      
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::Close()
{
    // remove all lights
    this->DestroyLights();

    // remove all models
    this->DestroyGeometry();

    this->forestModule->Discard();
    this->forestModule = 0;

    ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::SetupLights()
{
    // create global light
    matrix44 lightTransform = matrix44::rotationx(n_deg2rad(-45.0f));
    this->globalLight = GlobalLightEntity::Create();
    this->globalLight->SetTransform(lightTransform);
    this->globalLight->SetColor(float4(0.5f, 0.5f, 0.5f, 1.0f));
    this->globalLight->SetBackLightColor(float4(0.0f, 0.0f, 0.15f, 1.0f));
    this->globalLight->SetAmbientLightColor(float4(0.02f, 0.02f, 0.02f, 1.0f));
    this->globalLight->SetCastShadows(false);
    this->stage->AttachEntity(this->globalLight.cast<GraphicsEntity>());
}
//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::DestroyLights()
{
    this->stage->RemoveEntity(this->globalLight.cast<GraphicsEntity>());          
    this->globalLight = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::SetupGeometry()
{
    /*
    // setup models        
    this->ground = ModelEntity::Create();
    this->ground->SetResourceId(ResourceId("mdl:examples/dummyground.n3"));
    this->ground->SetTransform(matrix44::translation(0.0f, 0.0f, 0.0f));
    this->stage->AttachEntity(ground.cast<GraphicsEntity>());
                   
    IndexT j;
    IndexT i;
    float spacing = 5.0f;
    for (j = 0; j < 4; ++j)
    {
        for (i = 0; i < 4; ++i)
        {
            Ptr<ModelEntity> model = ModelEntity::Create();
            float dir = (i % 2 ? -1.0f : 1.0f);
            float x = (i+1) * spacing * dir - dir * 0.5f * spacing;
            model->SetTransform(matrix44::translation(x, 0.0, 2.0f * spacing * j));             
            model->SetResourceId(ResourceId("mdl:examples/tiger_lod.n3"));  
            this->stage->AttachEntity(model.cast<GraphicsEntity>());
            this->models.Append(model);  
        } 
    }

    // load trees
*/
    // set the number of cells for the quadtree, depending on level size
    Math::bbox box(point(0, 0, 0), vector(300, 300, 300));
    this->forestModule->LoadLevel(box);

    matrix44 trans = matrix44::identity();
    this->forestModule->CreateTreeInstance("mdl:examples/laubbaum_01.n3", "tree_1", trans, "");
    trans = matrix44::translation(30.0f, 0.0f, -10.0f);
    this->forestModule->CreateTreeInstance("mdl:examples/laubbaum_01.n3", "tree_2", trans, "");
    trans = matrix44::translation(60.0f, 0.0f, 0.0f);
    this->forestModule->CreateTreeInstance("mdl:examples/laubbaum_01.n3", "tree_3", trans, "");
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::DestroyGeometry()
{
    /*
    this->stage->RemoveEntity(this->ground.cast<GraphicsEntity>());
    this->ground = 0;   
    
    int i;
    for (i = 0; i < this->models.Size(); i++)
    {
        this->stage->RemoveEntity(this->models[i].cast<GraphicsEntity>());    	
    }
    this->models.Clear();
*/
    // reset the tree module to delete all trees and the quadtree
    this->forestModule->ExitLevel();
}


//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::OnConfigureDisplay()
{
    ViewerApplication::OnConfigureDisplay();
    this->display->Settings().SetVerticalSyncEnabled(true);
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::OnProcessInput()
{
    const Ptr<Keyboard>& kbd = InputServer::Instance()->GetDefaultKeyboard();
    const Ptr<GamePad>& gamePad = InputServer::Instance()->GetDefaultGamePad(0);
    
    // enable/disable debug view
    if (kbd->KeyDown(Key::F4) || gamePad.isvalid() && gamePad->ButtonDown(GamePad::XButton))
    {
        // turn on debug rendering        
        Ptr<Debug::RenderDebugView> renderDebugMsg = Debug::RenderDebugView::Create();
        renderDebugMsg->SetThreadId(Threading::Thread::GetMyThreadId());
        renderDebugMsg->SetEnableDebugRendering(!this->renderDebug);
        Graphics::GraphicsInterface::Instance()->SendBatched(renderDebugMsg.cast<Messaging::Message>());
        this->renderDebug = !this->renderDebug;
    }
    else if (kbd->KeyDown(Key::F3))
    {
        this->benchmarkmode = !this->benchmarkmode;
        // reset to start pos
        this->mayaCameraUtil.Reset();
    }
        
    if (this->benchmarkmode)
    {
        // auto rotate
        this->mayaCameraUtil.SetOrbiting(Math::float2(0.02,0));
        this->mayaCameraUtil.SetOrbitButton(true);
        this->mayaCameraUtil.Update();
        this->camera->SetTransform(this->mayaCameraUtil.GetCameraTransform());
    }
    else
    {                                        
        ViewerApplication::OnProcessInput(); 
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ForestViewerApplication::OnUpdateFrame()
{
    // test text rendering
    Timing::Time frameTime = (float)this->GetFrameTime();
    Util::String fpsTxt;
    fpsTxt.Format("Game FPS: %.2f", 1/frameTime);
    _debug_text(fpsTxt, Math::float2(0.0,0.0), Math::float4(1,1,1,1));
    _debug_text("Toggle Benchmark with F3!", Math::float2(0.0,0.025), Math::float4(1,1,1,1));

    if (this->benchmarkmode)
    {
        // benchmark with avg frames for every 100 frames
        if (this->frameTimes.Size() > 0 || frameTime < 0.08)
        {
            this->frameTimes.Append(frameTime);
        }       
        if (this->frameTimes.Size() > 0 && this->frameTimes.Size() % 50 == 0)
        {
            this->avgFPS = 0;
            IndexT i;
            for (i = 0; i < this->frameTimes.Size(); ++i)
            {
        	    this->avgFPS += this->frameTimes[i];
            }
            this->avgFPS /=  this->frameTimes.Size();
        }   
        if (this->avgFPS > 0)
        {
            fpsTxt.Format("Benchmarking: Avg Game FPS: %.2f", 1/this->avgFPS);
        }
        else
        {
            fpsTxt = "Benchmarking: Wait for measuring average framerate";
        }
        _debug_text(fpsTxt, Math::float2(0.0,0.05f), Math::float4(1,1,1,1));
    }               
          
    ViewerApplication::OnUpdateFrame();
}
} // namespace Tools
