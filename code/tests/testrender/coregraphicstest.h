#pragma once
#ifndef TEST_COREGRAPHICSTEST_H
#define TEST_COREGRAPHICSTEST_H
//------------------------------------------------------------------------------
/** 
    @class Test::CoreGraphicsTest
    
    Base class for test cases which needs a CoreGraphics environment.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testbase/testcase.h"
#include "io/ioserver.h"
#include "io/iointerface.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/vertexlayoutserver.h"
#include "resources/resourcemanager.h"
#include "debug/debuginterface.h"
#include "http/httpinterface.h"

//------------------------------------------------------------------------------
namespace Test
{
class CoreGraphicsTest : public Test::TestCase
{
    __DeclareClass(CoreGraphicsTest);
protected:
    /// setup the required runtime
    bool SetupRuntime();
    /// shutdown the runtime
    void ShutdownRuntime();

    Ptr<IO::IoServer> ioServer;
    Ptr<IO::IoInterface> ioInterface;
    Ptr<Http::HttpInterface> httpInterface;
    Ptr<Debug::DebugInterface> debugInterface;
    Ptr<Resources::ResourceManager> resManager;
    Ptr<CoreGraphics::DisplayDevice> displayDevice;
    Ptr<CoreGraphics::RenderDevice> renderDevice;
    Ptr<CoreGraphics::ShaderServer> shaderServer;
    Ptr<CoreGraphics::VertexLayoutServer> vertexLayoutServer;
};

} // namespace Test    
//------------------------------------------------------------------------------
#endif
