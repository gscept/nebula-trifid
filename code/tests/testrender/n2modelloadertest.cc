//------------------------------------------------------------------------------
//  n2modelloadertest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n2modelloadertest.h"
#include "models/modelserver.h"
#include "timing/timer.h"
#include "coregraphics/shaderserver.h"

namespace Test
{
__ImplementClass(Test::N2ModelLoaderTest, 'N2MT', Test::CoreGraphicsTest);

using namespace IO;
using namespace Util;
using namespace Resources;
using namespace Models;
using namespace Timing;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
void
N2ModelLoaderTest::Run()
{
    if (this->SetupRuntime())
    {
        ResourceManager* resManager = ResourceManager::Instance();
        IoServer* ioServer = IoServer::Instance();
        AssignRegistry* assignRegistry = AssignRegistry::Instance();

        // needed to backward compatibility
        assignRegistry->SetAssign(Assign("meshes", "msh:"));
        assignRegistry->SetAssign(Assign("textures", "tex:"));

        Ptr<ModelServer> modelServer = ModelServer::Create();
        modelServer->Open();
        
        // asynchronously load a legacy Nebula2 object file
        Ptr<ManagedModel> managedModel = modelServer->LoadManagedModel(ResourceId("mdl:tiger.n2"));
        managedModel->GetModel()->LoadResources();

        // wait until resources are loaded (or timeout is reached) 
        bool allLoaded = resManager->WaitForPendingResources(5.0);
        this->Verify(allLoaded);

        // discard everything
        modelServer->DiscardManagedModel(managedModel);
        modelServer->Close();
        modelServer = 0;
        this->ShutdownRuntime();
    }
}

} // namespace Test