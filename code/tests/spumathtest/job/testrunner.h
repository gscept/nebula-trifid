#pragma once
//------------------------------------------------------------------------------
/**
    @class Test::TestRunner
    
    The test runner class which runs all test cases.
    
    (C) 2006 Radon Labs GmbH
*/

//------------------------------------------------------------------------------
namespace Test
{
class TestCase;
class TestRunner
{
    enum { MAX_TESTS = 10 };
public:
    TestRunner();
    /// attach a test
    void AttachTestCase(TestCase* testCase);
    /// run the tests
    void Run();

private:
    TestCase *testCases[MAX_TESTS];
    int testCaseCount;
};

};    
//------------------------------------------------------------------------------