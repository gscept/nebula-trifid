#pragma once
#ifndef TEST_MODELLOADSAVETEST_H
#define TEST_MODELLOADSAVETEST_H
//------------------------------------------------------------------------------
/**
    @class Test::ModelLoadSaveTest
    
    Test Model loading/saving functionality.
    
    (C) 2007 Radon Labs GmbH
*/
#include "testbase/testcase.h"
#include "attr/attribute.h"
#include "models/model.h"

namespace Attr
{
    DeclareFloat4(ModelPos, 'mpos', ReadWrite);
    DeclareInt(ModelInt, 'mint', ReadWrite);
    DeclareBool(ModelBool, 'mbol', ReadWrite);
    DeclareString(ModelString, 'mstr', ReadWrite);
    DeclareInt(ModelNodeInt, 'mnin', ReadWrite);
    DeclareString(ModelNodeString, 'mnst', ReadWrite);
}

//------------------------------------------------------------------------------
namespace Test
{
class ModelLoadSaveTest : public TestCase
{
    __DeclareClass(ModelLoadSaveTest);
public:
    /// run the test
    virtual void Run();
private:
    /// test Model structure
    void TestModel(const Ptr<Models::Model>& model);
    /// test partial Model structure
    void TestPartialModel(const Ptr<Models::Model>& model);
};

}; // namespace Test
//------------------------------------------------------------------------------
#endif        
    