#pragma once
#ifndef TEST_ATTRIBUTECONTAINERTEST_H
#define TEST_ATTRIBUTECONTAINERTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::AttributeContainerTest
    
    Test AttributeContainer functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testbase/testcase.h"

//------------------------------------------------------------------------------
namespace Test
{
class AttributeContainerTest : public TestCase
{
    __DeclareClass(AttributeContainerTest);
public:
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
#endif

    