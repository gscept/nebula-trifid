//------------------------------------------------------------------------------
//  loadmeshtest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loadmeshtest.h"
#include "timing/timer.h"
#include "coregraphics/mesh.h"
#include "coregraphics/streammeshloader.h"

namespace Test
{
__ImplementClass(Test::LoadMeshTest, 'lmsh', Test::CoreGraphicsTest);

using namespace Util;
using namespace CoreGraphics;
using namespace IO;
using namespace Interface;
using namespace Resources;
using namespace Timing;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
void
LoadMeshTest::Run()
{
    Timer timer;
    if (this->SetupRuntime())
    {
        const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
        // test synchronous loading of nvx2 files
        const Util::StringAtom mesh1Name = "msh:cube.nvx2";
        const Util::StringAtom mesh2Name = "msh:pyramid.nvx2";
        this->Verify(!resManager->HasResource(mesh1Name));
        this->Verify(!resManager->HasResource(mesh2Name));
        
        Ptr<Mesh> mesh0 = resManager->CreateUnmanagedResource(mesh1Name, Mesh::RTTI, (ResourceLoader*) StreamMeshLoader::Create()).downcast<Mesh>();
        n_assert(mesh0.isvalid());
        this->Verify(resManager->HasResource(mesh1Name));
        mesh0->SetAsyncEnabled(false);
        this->Verify(!mesh0->IsLoaded());
        this->Verify(!mesh0->IsPending());
        this->Verify(!mesh0->LoadFailed());
        this->Verify(mesh0->GetState() == Resource::Initial);
        timer.Start();
        timer.Stop();
        this->Verify(mesh0->IsLoaded());
        if (mesh0->IsLoaded())
        {
            n_printf("Mesh '%s' sync-loaded in %d ticks.\n", mesh1Name.Value(), timer.GetTicks());

            this->Verify(mesh0->HasVertexBuffer());
            this->Verify(mesh0->HasIndexBuffer());

            this->Verify(mesh0->GetNumPrimitiveGroups() == 1);
            const PrimitiveGroup& primGroup = mesh0->GetPrimitiveGroupAtIndex(0);
            this->Verify(primGroup.GetBaseVertex() == 0);
            this->Verify(primGroup.GetNumVertices() == 24);
            this->Verify(primGroup.GetBaseIndex() == 0);
            this->Verify(primGroup.GetNumIndices() == 36);
            this->Verify(primGroup.GetPrimitiveTopology() == PrimitiveTopology::TriangleList);
            this->Verify(primGroup.GetBoundingBox().size() == vector(1.0f, 1.0f, 1.0f));
            
            this->Verify(mesh0->GetVertexBuffer()->GetNumVertices() == 24);
            const Ptr<VertexLayout>& vertexLayout = mesh0->GetVertexBuffer()->GetVertexLayout();
            this->Verify(vertexLayout->GetNumComponents() == 5);
            this->Verify(vertexLayout->GetVertexByteSize() == 56);
            this->Verify(vertexLayout->HasComponent(VertexComponent::Position, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Normal, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Tangent, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Binormal, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::TexCoord, 0));

            this->Verify(mesh0->GetIndexBuffer()->GetIndexType() == IndexType::Index16);
            this->Verify(mesh0->GetIndexBuffer()->GetNumIndices() == 36);
        }

        // test asynchronous loading
        Ptr<Mesh> mesh1 = resManager->CreateUnmanagedResource(mesh2Name, Mesh::RTTI, (ResourceLoader*) StreamMeshLoader::Create()).downcast<Mesh>();
        n_assert(mesh1.isvalid());
        this->Verify(resManager->HasResource(mesh2Name));
        mesh1->SetAsyncEnabled(true);
        this->Verify(!mesh1->IsLoaded());
        this->Verify(!mesh1->IsPending());
        this->Verify(!mesh1->LoadFailed());
        this->Verify(mesh1->GetState() == Resource::Initial);
        timer.Reset();
        timer.Start();
        do
        {
            mesh1->Load();
            Timing::Sleep(0);
        }
        while (mesh1->IsPending());
        timer.Stop();
        this->Verify(mesh1->IsLoaded());
        if (mesh1->IsLoaded())
        {
            n_printf("Mesh '%s' sync-loaded in %d ticks.\n", mesh1Name.Value(), timer.GetTicks());

            this->Verify(mesh1->HasVertexBuffer());
            this->Verify(mesh1->HasIndexBuffer());

            this->Verify(mesh1->GetNumPrimitiveGroups() == 1);
            const PrimitiveGroup& primGroup = mesh1->GetPrimitiveGroupAtIndex(0);
            this->Verify(primGroup.GetBaseVertex() == 0);
            this->Verify(primGroup.GetNumVertices() == 16);
            this->Verify(primGroup.GetBaseIndex() == 0);
            this->Verify(primGroup.GetNumIndices() == 18);
            this->Verify(primGroup.GetPrimitiveTopology() == PrimitiveTopology::TriangleList);
            
            this->Verify(mesh1->GetVertexBuffer()->GetNumVertices() == 16);
            const Ptr<VertexLayout>& vertexLayout = mesh1->GetVertexBuffer()->GetVertexLayout();
            this->Verify(vertexLayout->GetNumComponents() == 5);
            this->Verify(vertexLayout->GetVertexByteSize() == 56);
            this->Verify(vertexLayout->HasComponent(VertexComponent::Position, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Normal, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Tangent, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::Binormal, 0));
            this->Verify(vertexLayout->HasComponent(VertexComponent::TexCoord, 0));

            this->Verify(mesh1->GetIndexBuffer()->GetIndexType() == IndexType::Index16);
            this->Verify(mesh1->GetIndexBuffer()->GetNumIndices() == 18);
        }
        this->ShutdownRuntime();
    }
}

} // namespace Test