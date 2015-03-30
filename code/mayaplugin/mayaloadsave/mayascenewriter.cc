//------------------------------------------------------------------------------
//  mayascenewriter.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaloadsave/mayascenewriter.h"

namespace Maya
{
__ImplementClass(Maya::MayaSceneWriter, 'MSCW', IO::StreamWriter);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaSceneWriter::MayaSceneWriter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MayaSceneWriter::~MayaSceneWriter()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaSceneWriter::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->stream.isvalid());
    n_assert(this->stream->CanWrite());

    if (StreamWriter::Open())
    {
        this->xmlWriter = XmlWriter::Create();
        this->xmlWriter->SetStream(this->stream);
        if (this->xmlWriter->Open())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneWriter::Close()
{
    n_assert(this->IsOpen());
    this->xmlWriter->Close();
    this->xmlWriter = 0;
    StreamWriter::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneWriter::WriteMayaScene(const Ptr<MayaScene>& mayaScene)
{
    n_assert(this->IsOpen());
    
    this->xmlWriter->BeginNode("MayaScene");
    IndexT nodeIndex;
    SizeT numNodes = mayaScene->GetNumNodes();
    for (nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        const Ptr<MayaNode>& mayaNode = mayaScene->GetNodeByIndex(nodeIndex);
        this->WriteMayaNode(mayaNode);
    }
    this->xmlWriter->EndNode();
}

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneWriter::WriteMayaNode(const Ptr<MayaNode>& mayaNode)
{
    this->xmlWriter->BeginNode("MayaNode");
    this->xmlWriter->SetString("name", mayaNode->GetName().Value());
    this->xmlWriter->SetString("type", mayaNode->GetType().Value());
    this->xmlWriter->SetString("subtype", mayaNode->GetSubType().Value());
    if (mayaNode->HasParent())
    {
        this->xmlWriter->SetString("parent", mayaNode->GetParent()->GetName().Value());
    }
    IndexT attrIndex;
    SizeT numAttrs = mayaNode->GetNumAttrs();
    for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
    {
        this->WriteAttr(mayaNode, attrIndex);
    }
    this->xmlWriter->EndNode();
}

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneWriter::WriteAttr(const Ptr<MayaNode>& mayaNode, IndexT attrIndex)
{
    const String& name = mayaNode->GetAttrNameByIndex(attrIndex).Value();
    const Variant& value = mayaNode->GetAttrValueByIndex(attrIndex);
    this->xmlWriter->BeginNode("Attr");
    this->xmlWriter->SetString("name", name);
    this->xmlWriter->SetString("type", Variant::TypeToString(value.GetType()));
    switch (value.GetType())
    {
        IndexT i;
        case Variant::Int:
            this->xmlWriter->SetInt("value", value.GetInt());
            break;

        case Variant::Float:
            this->xmlWriter->SetFloat("value", value.GetFloat());
            break;

        case Variant::Bool:
            this->xmlWriter->SetBool("value", value.GetBool());
            break;

        case Variant::Float4:
            this->xmlWriter->SetFloat4("value", value.GetFloat4());
            break;

        case Variant::Matrix44:
            this->xmlWriter->SetMatrix44("value", value.GetMatrix44());
            break;

        case Variant::Guid:
            this->xmlWriter->SetString("value", value.GetGuid().AsString());
            break;

        case Variant::IntArray:
            this->xmlWriter->SetInt("size", value.GetIntArray().Size());
            for (i = 0; i < value.GetIntArray().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetInt("value", value.GetIntArray()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::FloatArray:
            this->xmlWriter->SetInt("size", value.GetFloatArray().Size());
            for (i = 0; i < value.GetFloatArray().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetFloat("value", value.GetFloatArray()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::BoolArray:
            this->xmlWriter->SetInt("size", value.GetBoolArray().Size());
            for (i = 0; i < value.GetBoolArray().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetBool("value", value.GetBoolArray()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::Float4Array:
            this->xmlWriter->SetInt("size", value.GetFloat4Array().Size());
            for (i = 0; i < value.GetFloat4Array().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetFloat4("value", value.GetFloat4Array()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::StringArray:
            this->xmlWriter->SetInt("size", value.GetStringArray().Size());
            for (i = 0; i < value.GetStringArray().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetString("value", value.GetStringArray()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::Matrix44Array:
            this->xmlWriter->SetInt("size", value.GetMatrix44Array().Size());
            for (i = 0; i < value.GetMatrix44Array().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetMatrix44("value", value.GetMatrix44Array()[i]);
                this->xmlWriter->EndNode();
            }
            break;

        case Variant::GuidArray:
            this->xmlWriter->SetInt("size", value.GetGuidArray().Size());
            for (i = 0; i < value.GetGuidArray().Size(); i++)
            {
                this->xmlWriter->BeginNode("elm");
                this->xmlWriter->SetString("value", value.GetGuidArray()[i].AsString());
                this->xmlWriter->EndNode();
            }
            break;
    }
    this->xmlWriter->EndNode();
}

} // namespace Maya