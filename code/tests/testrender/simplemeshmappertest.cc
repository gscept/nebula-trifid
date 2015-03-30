//------------------------------------------------------------------------------
//  simplemeshmappertest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "simplemeshmappertest.h"
#include "coregraphics/mesh.h"
#include "timing/timer.h"
#include "resources/managedmesh.h"

namespace Test
{
__ImplementClass(Test::SimpleMeshMapperTest, 'SMMT', Test::CoreGraphicsTest);

using namespace CoreGraphics;
using namespace Resources;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
void
SimpleMeshMapperTest::Run()
{
    if (this->SetupRuntime())
    {
        ResourceId id0 = "msh:cube.nvx2";
        ResourceId id1 = "msh:pyramid.nvx2";

        // create a few managed resources
        Ptr<ManagedMesh> mesh0 = resManager->CreateManagedResource(Mesh::RTTI, id0).downcast<ManagedMesh>();
        Ptr<ManagedMesh> mesh1 = resManager->CreateManagedResource(Mesh::RTTI, id1).downcast<ManagedMesh>();
        Ptr<ManagedMesh> mesh2 = resManager->CreateManagedResource(Mesh::RTTI, id0).downcast<ManagedMesh>();
        this->Verify(resManager->HasManagedResource(id0));
        this->Verify(resManager->HasManagedResource(id1));
        this->Verify(mesh0 == mesh2);
        this->Verify(mesh0->GetResourceId() == id0);
        this->Verify(mesh1->GetResourceId() == id1);
        this->Verify(mesh2->GetResourceId() == id0);
        this->Verify(mesh0->GetResourceType() == &Mesh::RTTI);
        this->Verify(mesh1->GetResourceType() == &Mesh::RTTI);
        this->Verify(mesh2->GetResourceType() == &Mesh::RTTI);
        this->Verify(mesh0->GetClientCount() == 2);
        this->Verify(mesh1->GetClientCount() == 1);
        this->Verify(mesh2->GetClientCount() == 2);
        this->Verify(mesh0->GetPriority() == ManagedResource::NormalPriority);
        this->Verify(mesh1->GetPriority() == ManagedResource::NormalPriority);
        this->Verify(mesh2->GetPriority() == ManagedResource::NormalPriority);

        // all managed meshes should now have a placeholder mesh
        this->Verify(mesh0->GetMesh().isvalid());
        this->Verify(mesh1->GetMesh().isvalid());
        this->Verify(mesh2->GetMesh().isvalid());
        this->Verify(mesh0->GetState() == Resource::Pending);
        this->Verify(mesh1->GetState() == Resource::Pending);
        this->Verify(mesh2->GetState() == Resource::Pending);
        
        // idle until everything is loaded (or timeout is reached)        
        Timer timer;
        timer.Start();
        const Time timeOut = 5.0;
        const Time startTime = timer.GetTime();
        bool timedOut = false;
        bool allLoaded = false;
        do
        {
            // trigger the resource manager (this is usually done per-frame)
            Timing::Sleep(0.01);
            resManager->Prepare(true);
            resManager->Update(0);
            allLoaded = (mesh0->GetState() == Resource::Loaded) && 
                        (mesh1->GetState() == Resource::Loaded) &&
                        (mesh2->GetState() == Resource::Loaded);
            timedOut = timer.GetTime() > (startTime + timeOut);
        }
        while (!(allLoaded || timedOut));
        this->Verify(allLoaded);

        // discard managed resources
        resManager->DiscardManagedResource(mesh0.upcast<ManagedResource>());
        this->Verify(mesh0->GetState() == Resource::Pending);
        this->Verify(mesh2->GetClientCount() == 1);
        this->Verify(resManager->HasManagedResource(id0));
        resManager->DiscardManagedResource(mesh1.upcast<ManagedResource>());
        this->Verify(mesh1->GetState() == Resource::Pending);
        this->Verify(!resManager->HasManagedResource(id1));
        resManager->DiscardManagedResource(mesh2.upcast<ManagedResource>());
        this->Verify(mesh2->GetState() == Resource::Pending);
        this->Verify(!resManager->HasManagedResource(id0));
    
        this->ShutdownRuntime();
    }
}

} // namespace Test