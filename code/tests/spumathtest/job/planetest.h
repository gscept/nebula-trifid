#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::PlaneTest
    
    Tests plane functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class PlaneTest : public TestCase
{
public:
    PlaneTest(const char *name) : TestCase(name) {}
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
