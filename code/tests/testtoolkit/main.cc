//------------------------------------------------------------------------------
//  testtoolkit/main.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/coreserver.h"
#include "testbase/testrunner.h"
#include "meshbuildervertextest.h"
#include "meshbuildertest.h"

using namespace Test;
using namespace Core;

//------------------------------------------------------------------------------
/**
*/
int __cdecl
main()
{
    // create Nebula3 runtime
    Ptr<CoreServer> coreServer = CoreServer::Create();
    coreServer->SetAppName(Util::StringAtom("Nebula3 Toolkit Tests"));
    coreServer->Open();

    n_printf("NEBULA3 TOOLKIT TESTS\n");
    n_printf("=====================\n");

    // setup and run test runner
    Ptr<TestRunner> testRunner = TestRunner::Create();
    testRunner->AttachTestCase(MeshBuilderVertexTest::Create());
    testRunner->AttachTestCase(MeshBuilderTest::Create());
    testRunner->Run(); 

    coreServer->Close();
    testRunner = 0;
    coreServer = 0;
    
    Core::SysFunc::Exit(0);
}
