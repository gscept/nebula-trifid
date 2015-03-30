#pragma once
#ifndef TEST_SHADERTEST_H
#define TEST_SHADERTEST_H
//------------------------------------------------------------------------------
/**
    @class Test::ShaderTest
    
    Test Shader functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphicstest.h"

//------------------------------------------------------------------------------
namespace Test
{
class ShaderTest : public CoreGraphicsTest
{
    __DeclareClass(ShaderTest);
public:
    /// run the test
    virtual void Run();
};

}; // namespace Test
//------------------------------------------------------------------------------
#endif        
    