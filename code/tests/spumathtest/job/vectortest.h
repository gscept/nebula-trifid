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
class VectorTest : public TestCase
{
public:
    VectorTest(const char *name) : TestCase(name) {}
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
