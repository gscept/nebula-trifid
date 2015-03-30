//------------------------------------------------------------------------------
//  testrender/main.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "foundation.h"
#include "render_classregistry.h"
#include "core/coreserver.h"
#include "core/sysfunc.h"
#include "testbase/testrunner.h"
#include "loadmeshtest.h"
#include "loadtexturetest.h"
#include "modelloadsavetest.h"
#include "shadertest.h"
#include "simplemeshmappertest.h"
#include "simpletexturemappertest.h"
#include "n2modelloadertest.h"
#include "loadanimationtest.h"
#include "animsamplemixtest.h"
#include "animsequencertest.h"
#include "attributetabletest.h"
#include "attributecontainertest.h"

using namespace Core;
using namespace Test;

int
__cdecl main()
{
    // create Nebula3 runtime
    Ptr<CoreServer> coreServer = CoreServer::Create();
    coreServer->SetAppName(Util::StringAtom("Nebula3 Render Tests"));
    coreServer->Open();

    n_printf("NEBULA3 RENDER TESTS\n");
    n_printf("====================\n");

    // setup and run test runner
    Ptr<TestRunner> testRunner = TestRunner::Create();
    testRunner->AttachTestCase(AttributeContainerTest::Create());
    testRunner->AttachTestCase(AttributeTableTest::Create());
    testRunner->AttachTestCase(AnimSequencerTest::Create());
    testRunner->AttachTestCase(AnimSampleMixTest::Create());
    testRunner->AttachTestCase(LoadAnimationTest::Create());
    /*
    testRunner->AttachTestCase(N2ModelLoaderTest::Create());
    testRunner->AttachTestCase(LoadMeshTest::Create());
    testRunner->AttachTestCase(LoadTextureTest::Create());
    testRunner->AttachTestCase(ModelLoadSaveTest::Create());
    testRunner->AttachTestCase(ShaderTest::Create());
    testRunner->AttachTestCase(SimpleMeshMapperTest::Create());
    testRunner->AttachTestCase(SimpleTextureMapperTest::Create());
    */
    testRunner->Run(); 

    coreServer->Close();
    testRunner = 0;
    coreServer = 0;
    
    Core::SysFunc::Exit(0);
}
