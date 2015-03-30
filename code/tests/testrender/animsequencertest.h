#pragma once
#ifndef TEST_ANIMSEQUENCERTEST_H
#define TEST_ANIMSEQUENCERTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::AnimSequencerTest
    
    Test Animation::AnimSequencerTest functionality.
    
    (C) 2008 Radon Labs GmbH
*/
#include "coregraphicstest.h"

//------------------------------------------------------------------------------
namespace Test
{
class AnimSequencerTest : public CoreGraphicsTest
{
    __DeclareClass(AnimSequencerTest);
public:
    /// run the test
    virtual void Run();    
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
    