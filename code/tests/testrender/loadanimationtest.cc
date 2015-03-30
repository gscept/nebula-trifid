//------------------------------------------------------------------------------
//  loadanimationtest.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loadanimationtest.h"
#include "coreanimation/animresource.h"
#include "coreanimation/streamanimationloader.h"

namespace Test
{
__ImplementClass(Test::LoadAnimationTest, 'latt', Test::CoreGraphicsTest);

using namespace Util;
using namespace CoreAnimation;
using namespace IO;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
void
LoadAnimationTest::VerifyAnimResource(const Ptr<AnimResource>& anim)
{
    StringAtom clip0Name("clip0");
    StringAtom clip1Name("clip1");
    StringAtom clip2Name("clip2");

    this->Verify(anim->GetNumClips() == 3);
    this->Verify(anim->HasClip(clip0Name));
    this->Verify(anim->HasClip(clip1Name));
    this->Verify(anim->HasClip(clip2Name));
    this->Verify(!anim->HasClip(StringAtom("bla")));
    this->Verify(anim->GetClipByIndex(0).GetName() == clip0Name);
    this->Verify(anim->GetClipByIndex(1).GetName() == clip1Name);
    this->Verify(anim->GetClipByIndex(2).GetName() == clip2Name);
    this->Verify(anim->GetClipIndexByName(clip0Name) == 0);
    this->Verify(anim->GetClipIndexByName(clip1Name) == 1);
    this->Verify(anim->GetClipIndexByName(clip2Name) == 2);
    this->Verify(anim->GetClipByName(clip0Name).GetName() == clip0Name);
    this->Verify(anim->GetClipByName(clip1Name).GetName() == clip1Name);
    this->Verify(anim->GetClipByName(clip2Name).GetName() == clip2Name);

    const AnimClip& clip0 = anim->GetClipByIndex(0);
    const AnimClip& clip1 = anim->GetClipByIndex(1);
    const AnimClip& clip2 = anim->GetClipByIndex(2);

    this->Verify(clip0.GetNumKeys() == 26);
    this->Verify(clip0.GetKeyStride() == 52); // ????
    this->Verify(clip0.GetKeyDuration() == 80); 
    this->Verify(clip0.GetClipDuration() == 2080);
    this->Verify(clip0.GetPreInfinityType() == InfinityType::Cycle);
    this->Verify(clip0.GetPostInfinityType() == InfinityType::Cycle);
    this->Verify(clip0.GetNumCurves() == 465);
    this->Verify(clip0.GetNumEvents() == 0);

    this->Verify(clip1.GetNumKeys() == 61);
    this->Verify(clip1.GetKeyStride() == 37); // ????
    this->Verify(clip1.GetKeyDuration() == 80);
    this->Verify(clip1.GetClipDuration() == 4880);
    this->Verify(clip1.GetPreInfinityType() == InfinityType::Cycle);
    this->Verify(clip1.GetPostInfinityType() == InfinityType::Cycle);
    this->Verify(clip1.GetNumCurves() == 465);
    this->Verify(clip1.GetNumEvents() == 0);

    this->Verify(clip2.GetNumKeys() == 19);
    this->Verify(clip2.GetKeyStride() == 52);   // ????
    this->Verify(clip2.GetKeyDuration() == 80);
    this->Verify(clip2.GetClipDuration() == 1520);
    this->Verify(clip2.GetPreInfinityType() == InfinityType::Cycle);
    this->Verify(clip2.GetPostInfinityType() == InfinityType::Cycle);
    this->Verify(clip2.GetNumCurves() == 465);
    this->Verify(clip2.GetNumEvents() == 0); 
}

//------------------------------------------------------------------------------
/**
*/
void
LoadAnimationTest::Run()
{
    if (this->SetupRuntime())
    {
        const Ptr<ResourceManager>& resManager = ResourceManager::Instance();
        StringAtom animName("anim:characters/mensch_m_animations.nax2");
        this->Verify(!resManager->HasResource(animName));

        // test synchronous loading
        Ptr<AnimResource> anim0 = resManager->CreateUnmanagedResource(animName, AnimResource::RTTI, (ResourceLoader*) StreamAnimationLoader::Create()).downcast<AnimResource>();
        n_assert(anim0.isvalid());
        this->Verify(resManager->HasResource(animName));
        anim0->SetAsyncEnabled(false);
        this->Verify(!anim0->IsLoaded());
        this->Verify(!anim0->IsPending());
        this->Verify(!anim0->LoadFailed());
        this->Verify(anim0->GetState() == Resource::Initial);
        anim0->Load();
        this->Verify(anim0->IsLoaded());
        if (anim0->IsLoaded())
        {
            this->VerifyAnimResource(anim0);
        }
        resManager->UnregisterUnmanagedResource(anim0.upcast<Resource>());
        anim0 = 0;
        this->Verify(!resManager->HasResource(animName));

        // test asynchronous resource loading
        anim0 = resManager->CreateUnmanagedResource(animName, AnimResource::RTTI, (ResourceLoader*) StreamAnimationLoader::Create()).downcast<AnimResource>();
        n_assert(anim0.isvalid());
        this->Verify(resManager->HasResource(animName));
        anim0->SetAsyncEnabled(true);
        this->Verify(!anim0->IsLoaded());
        this->Verify(!anim0->IsPending());
        this->Verify(!anim0->LoadFailed());
        this->Verify(anim0->GetState() == Resource::Initial);
        do
        {
            anim0->Load();
            Timing::Sleep(0);
        }
        while (anim0->IsPending());
        this->Verify(anim0->IsLoaded());
        if (anim0->IsLoaded())
        {
            this->VerifyAnimResource(anim0);
        }
        resManager->UnregisterUnmanagedResource(anim0.upcast<Resource>());
        anim0 = 0;
        this->Verify(!resManager->HasResource(animName));

        this->ShutdownRuntime();
    }
}

} // namespace Test
