//------------------------------------------------------------------------------
//  testrunner.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "testrunner.h"

#include "testcase.h"
#include "stackdebug.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <spu_printf.h>

namespace Test
{

//------------------------------------------------------------------------------
/**
*/
TestRunner::TestRunner() :
    testCaseCount(0)
{
    memset(testCases, 0, sizeof(this->testCases));
}

//------------------------------------------------------------------------------
/**
*/
void
TestRunner::AttachTestCase(TestCase* testCase)
{
    assert(testCase);
    this->testCases[this->testCaseCount++] = testCase;
}

//------------------------------------------------------------------------------
/**
*/
void
TestRunner::Run()
{
    int i;
    int numSucceeded = 0;
    int numFailed = 0;
    spu_printf("testcase count: %d\n", this->testCaseCount);
    for (i = 0; i < this->testCaseCount; i++)
    {
        TestCase* curTestCase = this->testCases[i];
        spu_printf("-> Running test: %s\n", curTestCase->GetName());
        char checkpointBuffer[100];
        sprintf(checkpointBuffer, "%s::Run() before", curTestCase->GetName());
        STACK_CHECKPOINT(checkpointBuffer);
        curTestCase->Run();
        sprintf(checkpointBuffer, "%s::Run() after", curTestCase->GetName());
        STACK_CHECKPOINT(checkpointBuffer);
        if (curTestCase->GetNumFailed() == 0)
        {
            spu_printf("-> SUCCESS: %s runs %d tests ok!\n", curTestCase->GetName(), curTestCase->GetNumSucceeded());
        }
        else
        {
            spu_printf("-> FAILURE: %d of %d tests failed in %s!\n", 
                curTestCase->GetNumFailed(),
                curTestCase->GetNumVerified(),
                curTestCase->GetName());
        }
        numFailed += curTestCase->GetNumFailed();
        numSucceeded += curTestCase->GetNumSucceeded();
        spu_printf("\n");       
    }
    spu_printf("* TEST RESULT: %d succeeded, %d failed!\n", numSucceeded, numFailed);    
    DumpStackCheckpoints();
}

}; // namespace Test
