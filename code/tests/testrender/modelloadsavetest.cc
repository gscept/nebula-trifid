//------------------------------------------------------------------------------
//  modelloadsavetest.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelloadsavetest.h"
#include "models/modelserver.h"
#include "io/ioserver.h"
#include "io/filestream.h"

namespace Attr
{
    DefineFloat4(ModelPos, 'mpos', ReadWrite);
    DefineInt(ModelInt, 'mint', ReadWrite);
    DefineBool(ModelBool, 'mbol', ReadWrite);
    DefineString(ModelString, 'mstr', ReadWrite);
    DefineInt(ModelNodeInt, 'mnin', ReadWrite);
    DefineString(ModelNodeString, 'mnst', ReadWrite);
}

namespace Test
{
__ImplementClass(Test::ModelLoadSaveTest, 'MLST', Test::TestCase);

using namespace Models;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
void
ModelLoadSaveTest::TestModel(const Ptr<Model>& model)
{
/*
    FIXME

    this->Verify(model.isvalid());
    this->Verify(model->Attributes().Contains(Attr::ModelPos));
    this->Verify(model->Attributes().Contains(Attr::ModelInt));
    this->Verify(model->Attributes().Contains(Attr::ModelBool));
    this->Verify(model->Attributes().Contains(Attr::ModelString));
    this->Verify(model->Attributes().GetVector3(Attr::ModelPos) == Math::vector3(1.0f, 2.0f, 3.0f));
    this->Verify(model->Attributes().GetInt(Attr::ModelInt) == 23);
    this->Verify(model->Attributes().GetBool(Attr::ModelBool) == true);
    this->Verify(model->Attributes().GetString(Attr::ModelString) == "Bla Bla");

    this->Verify(model->GetNodes().Size() == 3);
    this->Verify(model->HasNode("Node0"));
    this->Verify(model->HasNode("Node1"));
    this->Verify(model->HasNode("Node2"));

    const Ptr<ModelNode>& node0 = model->LookupNode("Node0");
    const Ptr<ModelNode>& node1 = model->LookupNode("Node1");
    const Ptr<ModelNode>& node2 = model->LookupNode("Node2");

    this->Verify(node0->GetName() == "Node0");
    this->Verify(!node0->HasParent());
    this->Verify(node0->IsAttachedToModel());
    this->Verify(node0->GetModel() == model);
    this->Verify(node0->GetChildren().Size() == 1);
    this->Verify(node0->GetChildren()[0] == node1);
    this->Verify(node0->Attributes().Contains(Attr::ModelNodeInt));
    this->Verify(node0->Attributes().Contains(Attr::ModelNodeString));
    this->Verify(node0->Attributes().GetInt(Attr::ModelNodeInt) == 24);
    this->Verify(node0->Attributes().GetString(Attr::ModelNodeString) == "Blub Blub");

    this->Verify(node1->GetName() == "Node1");
    this->Verify(node1->HasParent());
    this->Verify(node1->GetParent() == node0);
    this->Verify(node1->IsAttachedToModel());
    this->Verify(node1->GetModel() == model);
    this->Verify(node1->GetChildren().Size() == 0);
    this->Verify(node1->Attributes().Contains(Attr::ModelNodeInt));
    this->Verify(node1->Attributes().Contains(Attr::ModelNodeString));
    this->Verify(node1->Attributes().GetInt(Attr::ModelNodeInt) == 25);
    this->Verify(node1->Attributes().GetString(Attr::ModelNodeString) == "Blob");

    this->Verify(node2->GetName() == "Node2");
    this->Verify(!node2->HasParent());
    this->Verify(node2->IsAttachedToModel());
    this->Verify(node2->GetModel() == model);
    this->Verify(node2->GetChildren().Size() == 0);
    this->Verify(node2->Attributes().Contains(Attr::ModelNodeString));
    this->Verify(node2->Attributes().GetString(Attr::ModelNodeString) == "Blah");
*/
}

//------------------------------------------------------------------------------
/**
*/
void
ModelLoadSaveTest::TestPartialModel(const Ptr<Model>& model)
{
/*
    FIXME

    this->Verify(model.isvalid());
    this->Verify(model->Attributes().Contains(Attr::ModelPos));
    this->Verify(model->Attributes().Contains(Attr::ModelInt));
    this->Verify(model->Attributes().Contains(Attr::ModelBool));
    this->Verify(model->Attributes().Contains(Attr::ModelString));
    this->Verify(model->Attributes().GetVector3(Attr::ModelPos) == Math::vector3(1.0f, 2.0f, 3.0f));
    this->Verify(model->Attributes().GetInt(Attr::ModelInt) == 23);
    this->Verify(model->Attributes().GetBool(Attr::ModelBool) == true);
    this->Verify(model->Attributes().GetString(Attr::ModelString) == "Bla Bla");

    this->Verify(model->GetNodes().Size() == 2);
    this->Verify(model->HasNode("Node0"));
    this->Verify(model->HasNode("Node1"));

    const Ptr<ModelNode>& node0 = model->LookupNode("Node0");
    const Ptr<ModelNode>& node1 = model->LookupNode("Node1");

    this->Verify(node0->GetName() == "Node0");
    this->Verify(!node0->HasParent());
    this->Verify(node0->IsAttachedToModel());
    this->Verify(node0->GetModel() == model);
    this->Verify(node0->GetChildren().Size() == 1);
    this->Verify(node0->GetChildren()[0] == node1);
    this->Verify(node0->Attributes().Contains(Attr::ModelNodeInt));
    this->Verify(node0->Attributes().Contains(Attr::ModelNodeString));
    this->Verify(node0->Attributes().GetInt(Attr::ModelNodeInt) == 24);
    this->Verify(node0->Attributes().GetString(Attr::ModelNodeString) == "Blub Blub");

    this->Verify(node1->GetName() == "Node1");
    this->Verify(node1->HasParent());
    this->Verify(node1->GetParent() == node0);
    this->Verify(node1->IsAttachedToModel());
    this->Verify(node1->GetModel() == model);
    this->Verify(node1->GetChildren().Size() == 0);
    this->Verify(node1->Attributes().Contains(Attr::ModelNodeInt));
    this->Verify(node1->Attributes().Contains(Attr::ModelNodeString));
    this->Verify(node1->Attributes().GetInt(Attr::ModelNodeInt) == 25);
    this->Verify(node1->Attributes().GetString(Attr::ModelNodeString) == "Blob");
*/
}

//------------------------------------------------------------------------------
/**
*/
void
ModelLoadSaveTest::Run()
{
/*
    FIXME

    Ptr<IO::Server> ioServer = IO::Server::Create();
    ioServer->RegisterUriScheme("file", IO::FileStream::RTTI);
    Ptr<ModelServer> modelServer = ModelServer::Create();

    const URI srcFile = "home:work/testdata/modelloadsavetest.xml";
    const URI xmlDestFile = "home:intermediate/testdata/modelloadsavetest.xml";
    const URI binDestFile = "home:intermediate/testdata/modelloadsavetest.n3";

    if (ioServer->FileExists(xmlDestFile))
    {
        ioServer->DeleteFile(xmlDestFile);
        this->Verify(!ioServer->FileExists(xmlDestFile));
    }
    if (ioServer->FileExists(binDestFile))
    {
        ioServer->DeleteFile(binDestFile);
        this->Verify(!ioServer->FileExists(binDestFile));
    }
    
    // load model from xml file
    bool modelLoaded = modelServer->LoadModels(srcFile);
    this->Verify(modelLoaded);
    if (modelLoaded)
    {
        this->Verify(modelServer->HasModel("ModelLoadSaveTest"));

        // lookup the loaded model
        Ptr<Model> model = modelServer->LookupModel("ModelLoadSaveTest");
        this->Verify(model->GetName() == "ModelLoadSaveTest");
        this->TestModel(model);
        
        // save Model back to xml file
        bool xmlModelSaved = modelServer->SaveModel(model, xmlDestFile, XmlModelWriter::RTTI);
        this->Verify(xmlModelSaved);
        this->Verify(ioServer->FileExists(xmlDestFile));

        // save Model back to binary file
        bool binModelSaved = modelServer->SaveModel(model, binDestFile, BinaryModelWriter::RTTI);
        this->Verify(binModelSaved);
        this->Verify(ioServer->FileExists(binDestFile));

        modelServer->DiscardModel("ModelLoadSaveTest");
        this->Verify(!modelServer->HasModel("ModelLoadSaveTest"));
        model = 0;

        // load back the saved xml file
        modelLoaded = modelServer->LoadModels(xmlDestFile);
        if (modelLoaded)
        {
            this->Verify(modelServer->HasModel("ModelLoadSaveTest"));
            model = modelServer->LookupModel("ModelLoadSaveTest");
            this->Verify(model->GetName() == "ModelLoadSaveTest");
            this->TestModel(model);
            modelServer->DiscardModel("ModelLoadSaveTest");
            this->Verify(!modelServer->HasModel("ModelLoadSaveTest"));
            model = 0;
        }

        // load back saved binary file
        modelLoaded = modelServer->LoadModels(binDestFile);
        if (modelLoaded)
        {
            this->Verify(modelServer->HasModel("ModelLoadSaveTest"));
            model = modelServer->LookupModel("ModelLoadSaveTest");
            this->Verify(model->GetName() == "ModelLoadSaveTest");
            this->TestModel(model);
            modelServer->DiscardModel("ModelLoadSaveTest");
            this->Verify(!modelServer->HasModel("ModelLoadSaveTest"));
            model = 0;
        }

        // test cloning
        modelLoaded = modelServer->LoadModels(srcFile);
        this->Verify(modelLoaded);
        model = modelServer->LookupModel("ModelLoadSaveTest");
        Ptr<Model> clone0 = modelServer->CloneModel(model, "Clone0");
        this->Verify(clone0->GetName() == "Clone0");
        this->TestModel(clone0);

        // test partial cloning
        Ptr<Model> clone1 = modelServer->CloneModelFragment(model, "Node0", "Clone1");
        this->Verify(clone1->GetName() == "Clone1");
        this->TestPartialModel(clone1);
    }

    modelServer = 0;
    ioServer = 0;
*/
}

}
