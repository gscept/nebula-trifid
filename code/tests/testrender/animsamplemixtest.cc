//------------------------------------------------------------------------------
//  animsamplemixtest.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animsamplemixtest.h"
#include "resources/resourcemanager.h"
#include "resources/managedresource.h"
#include "coreanimation/animsamplebuffer.h"

namespace Test
{
__ImplementClass(Test::AnimSampleMixTest, 'asmt', Test::CoreGraphicsTest);

using namespace Util;
using namespace Resources;
using namespace CoreAnimation;

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
void
AnimSampleMixTest::Run()
{
/*
    if (this->SetupRuntime())
    {
        // first load an animation resource through the resource manager
        Ptr<ManagedResource> res = this->resManager->CreateManagedResource(AnimResource::RTTI, ResourceId("anim:characters/mensch_m_animations.nax2"));
        this->resManager->WaitForPendingResources(5.0);
        this->Verify(!res->IsPlaceholder());
        if (!res->IsPlaceholder())
        {
            Ptr<AnimResource> animRes = res->GetResource().downcast<AnimResource>();

            // we need 3 sample buffers (2 source buffer, and one which holds the mixed result)
            Ptr<AnimSampleBuffer> srcBuffer0 = AnimSampleBuffer::Create();
            Ptr<AnimSampleBuffer> srcBuffer1 = AnimSampleBuffer::Create();
            Ptr<AnimSampleBuffer> dstBuffer  = AnimSampleBuffer::Create();
            Ptr<AnimSampleBuffer> lastResultBuffer = AnimSampleBuffer::Create();
            Ptr<AnimSampleBuffer> deltaBuffer = AnimSampleBuffer::Create();
            srcBuffer0->Setup(animRes);
            srcBuffer1->Setup(animRes);
            dstBuffer->Setup(animRes);
            lastResultBuffer->Setup(animRes);
            deltaBuffer->Setup(animRes);

            // sample 2 animations and mix into a single result
            AnimSampler::Sample(animRes, 0, SampleType::Linear, 0, srcBuffer0);
            AnimSampler::Sample(animRes, 1, SampleType::Linear, 40, srcBuffer1);
            AnimMixer::Mix(animRes, srcBuffer0, srcBuffer1, 0.5f, dstBuffer);
        }
        this->ShutdownRuntime();
    }
*/
}

} // namespace Test