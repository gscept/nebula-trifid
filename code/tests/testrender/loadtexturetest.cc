//------------------------------------------------------------------------------
//  loadtexturetest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loadtexturetest.h"
#include "coregraphics/texture.h"
#include "coregraphics/streamtextureloader.h"
#include "timing/timer.h"

namespace Test
{
__ImplementClass(Test::LoadTextureTest, 'ltex', Test::CoreGraphicsTest);

using namespace Util;
using namespace CoreGraphics;
using namespace IO;
using namespace Interface;
using namespace Resources;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
void
LoadTextureTest::Run()
{
    Timer timer;
    if (this->SetupRuntime())
    {
        const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
        const Util::StringAtom tex1Name = "tex:test1.dds";
        const Util::StringAtom tex2Name = "tex:test2.dds";
        this->Verify(!resManager->HasResource(tex1Name));
        this->Verify(!resManager->HasResource(tex2Name));

        // test synchronous loading
        Ptr<Texture> tex0 = resManager->CreateUnmanagedResource(tex1Name, Texture::RTTI, (ResourceLoader*) StreamTextureLoader::Create()).downcast<Texture>();
        n_assert(tex0.isvalid());
        this->Verify(resManager->HasResource(tex1Name));
        tex0->SetAsyncEnabled(false);
        this->Verify(!tex0->IsLoaded());
        this->Verify(!tex0->IsPending());
        this->Verify(!tex0->LoadFailed());
        this->Verify(tex0->GetState() == Resource::Initial);
        timer.Start();
        timer.Stop();
        this->Verify(tex0->IsLoaded());
        if (tex0->IsLoaded())
        {
            n_printf("Texture '%s' sync-loaded in %d ticks.\n", tex1Name.Value(), timer.GetTicks());

            this->Verify(tex0->GetType() == Texture::Texture2D);
            this->Verify(tex0->GetWidth() == 512);
            this->Verify(tex0->GetHeight() == 512);
            this->Verify(tex0->GetDepth() == 1);
            this->Verify(tex0->GetNumMipLevels() == 10);
            this->Verify(tex0->GetPixelFormat() == PixelFormat::DXT1);
        }

        // test asynchronous loading
        Ptr<Texture> tex1 = resManager->CreateUnmanagedResource(tex2Name, Texture::RTTI, (ResourceLoader*) StreamTextureLoader::Create()).downcast<Texture>();
        n_assert(tex1.isvalid());
        this->Verify(resManager->HasResource(tex2Name));
        tex1->SetAsyncEnabled(true);
        timer.Reset();
        timer.Start();
        do 
        {
            tex1->Load();
            Timing::Sleep(0);
        }
        while (tex1->IsPending());
        timer.Stop();
        this->Verify(tex1->IsLoaded());
        if (tex1->IsLoaded())
        {
            n_printf("Texture '%s' async-loaded in %d ticks.\n", tex1Name.Value(), timer.GetTicks());

            this->Verify(tex1->GetType() == Texture::Texture2D);
            this->Verify(tex1->GetWidth() == 512);
            this->Verify(tex1->GetHeight() == 512);
            this->Verify(tex1->GetDepth() == 1);
            this->Verify(tex1->GetNumMipLevels() == 10);
            this->Verify(tex1->GetPixelFormat() == PixelFormat::A8R8G8B8);
        }
        this->ShutdownRuntime();
    }
}

} // namespace Test