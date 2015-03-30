#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::VectorTest
    
    Tests vector functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class QuaternionTest : public TestCase
{
public:
    QuaternionTest(const char *name) : TestCase(name) {}
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
