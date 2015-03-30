#pragma once
#ifndef TEST_VIEWERSTRESSTESTAPPLICATION_H
#define TEST_VIEWERSTRESSTESTAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class Test::ViewerStressTestApplication
  
    A stress test for the multithreaded renderer.
    
    (C) 2008 Radon Labs GmbH
*/    
#include "apprender/viewerapplication.h"
#include "graphics/globallightentity.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace Test
{
class ViewerStressTestApplication : public App::ViewerApplication
{
public:
    /// constructor
    ViewerStressTestApplication();
    /// destructor
    virtual ~ViewerStressTestApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

private:
    /// get a random position transform
    void GetRandomTransform(Math::matrix44& m);
    /// called when frame should be updated
    virtual void OnUpdateFrame();

    Ptr<Graphics::GlobalLightEntity> globalLight;
    Util::FixedArray<Ptr<Graphics::ModelEntity> > modelEntities;
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
