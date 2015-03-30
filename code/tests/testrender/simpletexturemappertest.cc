//------------------------------------------------------------------------------
//  simpletexturemappertest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "simpletexturemappertest.h"
#include "coregraphics/texture.h"
#include "timing/timer.h"
#include "resources/managedtexture.h"

namespace Test
{
__ImplementClass(Test::SimpleTextureMapperTest, 'STMT', Test::CoreGraphicsTest);

using namespace CoreGraphics;
using namespace Resources;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
void
SimpleTextureMapperTest::Run()
{
    if (this->SetupRuntime())
    {
        ResourceId id0 = "tex:test1.dds";
        ResourceId id1 = "tex:test2.dds";

        // create a few managed texture resources
        Ptr<ManagedTexture> tex0 = resManager->CreateManagedResource(Texture::RTTI, id0).downcast<ManagedTexture>();
        Ptr<ManagedTexture> tex1 = resManager->CreateManagedResource(Texture::RTTI, id1).downcast<ManagedTexture>();
        Ptr<ManagedTexture> tex2 = resManager->CreateManagedResource(Texture::RTTI, id0).downcast<ManagedTexture>();
        this->Verify(resManager->HasManagedResource(id0));
        this->Verify(resManager->HasManagedResource(id1));
        this->Verify(tex0 == tex2);
        this->Verify(tex0->GetResourceId() == id0);
        this->Verify(tex1->GetResourceId() == id1);
        this->Verify(tex2->GetResourceId() == id0);
        this->Verify(tex0->GetResourceType() == &Texture::RTTI);
        this->Verify(tex1->GetResourceType() == &Texture::RTTI);
        this->Verify(tex2->GetResourceType() == &Texture::RTTI);
        this->Verify(tex0->GetClientCount() == 2);
        this->Verify(tex1->GetClientCount() == 1);
        this->Verify(tex2->GetClientCount() == 2);
        this->Verify(tex0->GetPriority() == ManagedResource::NormalPriority);
        this->Verify(tex1->GetPriority() == ManagedResource::NormalPriority);
        this->Verify(tex2->GetPriority() == ManagedResource::NormalPriority);

        // all textures should now have a placeholder texture
        this->Verify(tex0->GetTexture().isvalid());
        this->Verify(tex1->GetTexture().isvalid());
        this->Verify(tex2->GetTexture().isvalid());
        this->Verify(tex0->GetState() == Resource::Pending);
        this->Verify(tex1->GetState() == Resource::Pending);
        this->Verify(tex2->GetState() == Resource::Pending);
        
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
            allLoaded = (tex0->GetState() == Resource::Loaded) && 
                        (tex1->GetState() == Resource::Loaded) &&
                        (tex2->GetState() == Resource::Loaded);
            timedOut = timer.GetTime() > (startTime + timeOut);
        }
        while (!(allLoaded || timedOut));
        this->Verify(allLoaded);

        // discard managed resources
        resManager->DiscardManagedResource(tex0.upcast<ManagedResource>());
        this->Verify(tex0->GetState() == Resource::Pending);
        this->Verify(tex2->GetClientCount() == 1);
        this->Verify(resManager->HasManagedResource(id0));
        resManager->DiscardManagedResource(tex1.upcast<ManagedResource>());
        this->Verify(tex1->GetState() == Resource::Pending);
        this->Verify(!resManager->HasManagedResource(id1));
        resManager->DiscardManagedResource(tex2.upcast<ManagedResource>());
        this->Verify(tex2->GetState() == Resource::Pending);
        this->Verify(!resManager->HasManagedResource(id0));
    
        this->ShutdownRuntime();
    }
}

} // namespace Test