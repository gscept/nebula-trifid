#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::PointTest
    
    Tests point functionality.
    
    (C) 2009 Radon Labs GmbH
*/
#include "testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class PointTest : public TestCase
{
public:
    PointTest(const char *name) : TestCase(name) {}

    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
