#pragma once
#ifndef TEST_LOADTEXTURETEST_H
#define TEST_LOADTEXTURETEST_H
//------------------------------------------------------------------------------
/**
    @class Test::LoadTextureTest
    
    Test synchronous and asynchronous texture loading.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphicstest.h"

//------------------------------------------------------------------------------
namespace Test
{
class LoadTextureTest : public CoreGraphicsTest
{
    __DeclareClass(LoadTextureTest);
public:
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
    