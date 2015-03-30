#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::Matrix44Test

    Test matrix44 functionality.

    (C) 2007 Radon Labs GmbH
*/
#include "testcase.h"
#include "math/float4.h"
#include "math/matrix44.h"

//------------------------------------------------------------------------------
namespace Test
{
class Matrix44Test : public TestCase
{
public:
    Matrix44Test(const char *name);
    /// run the test
    virtual void Run();

private:
    void RunTest0();
    void RunTest1();
    void RunTest2();

private:
    const Math::float4 pOneTwoThree;
    const Math::float4 vOneTwoThree;
    const Math::matrix44 trans123;
    const Math::matrix44 midentity;
    const Math::matrix44 rotOneX;
    const Math::float4 pZero;
    const Math::float4 pOneX;
    const Math::float4 pOneY;
};

} // namespace Test
//------------------------------------------------------------------------------

