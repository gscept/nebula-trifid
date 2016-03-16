//------------------------------------------------------------------------------
//  n2sceneloader.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n2sceneloader.h"
#include "io/ioserver.h"
#include "io/stream.h"

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::N2SceneLoader, 'N2SL', Core::RefCounted);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
N2SceneLoader::N2SceneLoader() :
    validNodeType(false),
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
N2SceneLoader::~N2SceneLoader()
{
    if (this->IsValid())
    {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N2SceneLoader::Setup(const Ptr<N2ReflectionInfo>& reflInfo)
{
    n_assert(!this->IsValid());
    n_assert(reflInfo.isvalid());
    n_assert(!this->sceneNodeTree.isvalid());

    this->reflectionInfo = reflInfo;
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
N2SceneLoader::Discard()
{
    n_assert(this->IsValid());
    n_assert(this->sceneNodeStack.IsEmpty());

    this->reflectionInfo = 0;
    this->sceneNodeTree = 0;
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
N2SceneLoader::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<SceneNodeTree>
N2SceneLoader::Load(const URI& uri, Logger& logger)
{
    n_assert(this->IsValid());
    n_assert(!this->sceneNodeTree.isvalid());
    n_assert(!this->validNodeType);

    Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
    Ptr<BinaryReader> reader = BinaryReader::Create();
    reader->SetStream(stream);
    if (reader->Open())
    {
        // check magic number
        FourCC magic(reader->ReadUInt());
        if (magic != FourCC('NOB0'))
        {
            logger.Error("N2SceneLoader::Load(): '%s' is not a valid .n2 file (magic number mismatch)!\n",
                uri.AsString().AsCharPtr());
            return Ptr<SceneNodeTree>();
        }

        // skip source header data
        String headerData = reader->ReadString();
        
        // setup scene node tree object
        this->sceneNodeTree = SceneNodeTree::Create();
        this->sceneNodeTree->Setup();

        // parse the file
        while (!reader->Eof())
        {
            FourCC curFourCC = reader->ReadUInt();
            if (curFourCC == FourCC('_new'))
            {
                String objClass = reader->ReadString();
                String objName  = reader->ReadString();
                this->BeginNode(objClass, objName, logger);
            }
            else if (curFourCC == FourCC('_sel'))
            {
                // skip relative '..' string
                String dotDot = reader->ReadString();
                this->EndNode(logger);
            }
            else
            {
                // read current data tag
                this->ReadDataTag(curFourCC, reader, logger);
            }
        }
        reader->Close();
        this->validNodeType = false;

        Ptr<SceneNodeTree> result = this->sceneNodeTree;
        this->sceneNodeTree = 0;
        return result;
    }
    else
    {
        logger.Error("N2SceneLoader::Load(): failed to open file '%s'!\n", uri.AsString().AsCharPtr());
        return Ptr<SceneNodeTree>();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N2SceneLoader::BeginNode(const StringAtom& nodeType, const StringAtom& nodeName, Logger& logger)
{
    n_assert(this->sceneNodeTree.isvalid());
    n_assert(this->reflectionInfo.isvalid());

    // check if this is a known node type
    if (reflectionInfo->HasNodeType(nodeType))
    {
        this->validNodeType = true;
        Ptr<SceneNode> newSceneNode = SceneNode::Create();
        Ptr<SceneNode> parentNode;
        if (!this->sceneNodeStack.IsEmpty())
        {
            parentNode = this->sceneNodeStack.Peek();
        }
        newSceneNode->Setup(nodeName, nodeType, parentNode);
        this->sceneNodeTree->AddNode(newSceneNode);
        this->sceneNodeStack.Push(newSceneNode);
        this->reflectionInfo->SetQueryNodeType(nodeType);
    }
    else
    {
        this->validNodeType = false;
        logger.Warning("** Skipping unknown node (name='%s', type='%s')!\n", 
            nodeName.Value(), nodeType.Value());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N2SceneLoader::EndNode(Logger& logger)
{
    if (this->validNodeType)
    {
        n_assert(!this->sceneNodeStack.IsEmpty());
        this->sceneNodeStack.Pop();
    }
    if (!this->sceneNodeStack.IsEmpty())
    {
        this->validNodeType = true;
        this->reflectionInfo->SetQueryNodeType(this->sceneNodeStack.Peek()->GetType());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
N2SceneLoader::ReadDataTag(const FourCC& fourCC, const Ptr<BinaryReader>& reader, Logger& logger)
{
    ushort length = reader->ReadUShort();

    // if currently inside an unknown node type, skip data tag
    if (!this->validNodeType)
    {
        reader->GetStream()->Seek(length, Stream::Current);
    }
    else
    {
        // query attribute types from reflection info
        if (this->reflectionInfo->HasCmdProtoByFourCC(fourCC))
        {
            // get parameter types from reflection info and read data values
            const N2ReflectionInfo::CmdProto& cmdProto = this->reflectionInfo->GetCmdProtoByFourCC(fourCC);
            Array<Variant> values;
            IndexT valueIndex;
            for (valueIndex = 0; valueIndex < cmdProto.inputTypes.Size(); valueIndex++)
            {
                Variant::Type type = cmdProto.inputTypes[valueIndex];
                Variant value;
                switch (type)
                {
                    case Variant::Int:      value.SetInt(reader->ReadInt()); break;
                    case Variant::Float:    value.SetFloat(reader->ReadFloat()); break;
                    case Variant::Bool:     value.SetBool(reader->ReadBool()); break;
                    case Variant::String:   value.SetString(reader->ReadString()); break;
                    default:
                        n_error("N2SceneLoader::ReadDataTag(): invalid value type!\n");
                        break;
                }
                values.Append(value);
            }

            // add new attribute to current scene node
            this->sceneNodeStack.Peek()->AddAttr(cmdProto.cmdName, cmdProto.cmdFourCC, values);
        }
        else
        {
            // unknown cmd fourcc, skip data tag
            reader->GetStream()->Seek(length, Stream::Current);
            logger.Warning("  Skipping unknown data tag '%s' in node (name=%s, type=%s).\n", 
                fourCC.AsString().AsCharPtr(), 
                this->sceneNodeStack.Peek()->GetName().Value(),
                this->sceneNodeStack.Peek()->GetType().Value());
        }
    }
}

} // namespace ToolkitUtil