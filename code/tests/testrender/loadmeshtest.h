#pragma once
#ifndef TEST_LOADMESHTEST_H
#define TEST_LOADMESHTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::LoadMeshTest
  
    Test synchronous and asynchronous mesh loading.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphicstest.h"

//------------------------------------------------------------------------------
namespace Test
{
class LoadMeshTest : public CoreGraphicsTest
{
    __DeclareClass(LoadMeshTest);
public:
    /// run the test
    virtual void Run();
};

} // namespace Test
//------------------------------------------------------------------------------
#endif
