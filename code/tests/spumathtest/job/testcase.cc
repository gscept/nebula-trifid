//------------------------------------------------------------------------------
//  testcase.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "testcase.h"

#include <spu_printf.h>

namespace Test
{

//------------------------------------------------------------------------------
/**
*/
TestCase::TestCase(const char *_name) :
    name(_name),
    numVerified(0),
    numSucceeded(0),
    numFailed(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TestCase::~TestCase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Override this method in your derived class.
*/    
void
TestCase::Run()
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
void
TestCase::Verify(bool b)
{
    if (b)
    {
        spu_printf("%s #%d: ok\n", this->name, this->numVerified);
        this->numSucceeded++;
    }
    else
    {
        spu_printf("%s #%d: FAILED\n", this->name, this->numVerified);
        this->numFailed++;
    }
    this->numVerified++;
}

}; // namespace Test
