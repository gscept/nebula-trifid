#pragma once
#ifndef TEST_LOADANIMATIONTEST_H
#define TEST_LOADANIMATIONTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::LoadAnimationTest
    
    Test animation loading.
    
    (C) 2008 Radon Labs GmbH
*/
#include "coregraphicstest.h"
#include "coreanimation/animresource.h"

//------------------------------------------------------------------------------
namespace Test
{
class LoadAnimationTest : public CoreGraphicsTest
{
    __DeclareClass(LoadAnimationTest);
public:
    /// run the test
    virtual void Run();
private:
    /// verify anim resource content
    void VerifyAnimResource(const Ptr<CoreAnimation::AnimResource>& anim);
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
    
    