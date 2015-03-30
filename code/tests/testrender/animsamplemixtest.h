#pragma once
#ifndef TEST_ANIMSAMPLEMIXTEST_H
#define TEST_ANIMSAMPLEMIXTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::AnimSampleMixTest
  
    Test AnimSampler and AnimMixer functionality.  
    
    (C) 2008 Radon Labs GmbH
*/  
#include "coregraphicstest.h"

//------------------------------------------------------------------------------
namespace Test
{
class AnimSampleMixTest : public CoreGraphicsTest
{
    __DeclareClass(AnimSampleMixTest);
public:
    /// run the test
    virtual void Run();
};            

} // namespace Test
//------------------------------------------------------------------------------
#endif
  