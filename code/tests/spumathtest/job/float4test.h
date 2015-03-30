#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::Float4Test
    
    Tests float4 functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class Float4Test : public TestCase
{
public:
    Float4Test(const char *name) : TestCase(name) {}
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
