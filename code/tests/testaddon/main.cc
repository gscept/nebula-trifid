//------------------------------------------------------------------------------
//  testfoundation/main.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "foundation.h"
#include "core/coreserver.h"
#include "core/sysfunc.h"
#include "testbase/testrunner.h"
#include "testaddon/databasetest.h"
#include "testaddon/datasettest.h"

using namespace Core;
using namespace Test;

int
__cdecl main()
{
    // create Nebula3 runtime
    Ptr<CoreServer> coreServer = CoreServer::Create();
    coreServer->SetAppName("Nebula3 Foundation Tests");
    coreServer->Open();

    n_printf("NEBULA3 FOUNDATION TESTS\n");
    n_printf("========================\n");

    // setup and run test runner
    Ptr<TestRunner> testRunner = TestRunner::Create();
    testRunner->AttachTestCase(DatabaseTest::Create());
    testRunner->AttachTestCase(DatasetTest::Create());
    testRunner->Run(); 

    coreServer->Close();
    testRunner = 0;
    coreServer = 0;
    
    Core::SysFunc::Exit(0);
}
