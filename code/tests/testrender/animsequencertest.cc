//------------------------------------------------------------------------------
//  animsequencertest.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animsequencertest.h"
#include "resources/managedresource.h"
#include "coreanimation/animresource.h"
#include "animation/animsequencer.h"
#include "animation/playclipjob.h"

namespace Test
{
__ImplementClass(Test::AnimSequencerTest, 'asrt', Test::CoreGraphicsTest);

using namespace Util;
using namespace Resources;
using namespace CoreAnimation;
using namespace Animation;

//------------------------------------------------------------------------------
/**
    *** OBSOLETE ***
*/
void
AnimSequencerTest::Run()
{
    if (this->SetupRuntime())
    {
        // first load an animation resource through the resource manager
        Ptr<ManagedResource> res = this->resManager->CreateManagedResource(AnimResource::RTTI, ResourceId("anim:characters/mensch_m_animations.nax2"));
        this->resManager->WaitForPendingResources(5.0);
        this->Verify(!res->IsPlaceholder());
        if (!res->IsPlaceholder())
        {
            Ptr<AnimResource> animRes = res->GetLoadedResource().downcast<AnimResource>();

            // setup an AnimSequencer object
            AnimSequencer animSequencer;
            animSequencer.Setup(animRes);

            // setup a few same-priority PlayClip jobs and attach them to the sequencer
            Ptr<PlayClipJob> playClip0 = PlayClipJob::Create();
            playClip0->SetClipName("idle_01");
            playClip0->SetStartTime(0);
            playClip0->SetDuration(5000);
            playClip0->SetFadeInTime(100);
            animSequencer.EnqueueAnimJob(playClip0.cast<AnimJob>());

            Ptr<PlayClipJob> playClip1 = PlayClipJob::Create();
            playClip1->SetClipName("gehen_01");
            playClip1->SetStartTime(2000);
            playClip1->SetDuration(5000);
            playClip1->SetFadeInTime(100);
            animSequencer.EnqueueAnimJob(playClip1.cast<AnimJob>());

            Ptr<PlayClipJob> playClip2 = PlayClipJob::Create();
            playClip2->SetClipName("laufen_01");
            playClip2->SetStartTime(1000);
            playClip2->SetDuration(5000);
            playClip2->SetFadeInTime(100);
            animSequencer.EnqueueAnimJob(playClip2.cast<AnimJob>());

            // evaluate the anim sequencer at different points in time
            animSequencer.UpdateTime(0);
//            animSequencer->UpdateAnimation();

            animSequencer.UpdateTime(50);
//            animSequencer->UpdateAnimation();

            animSequencer.UpdateTime(1000);
//            animSequencer->UpdateAnimation();
        }
        this->ShutdownRuntime();
    }
}

} // namespace Test
