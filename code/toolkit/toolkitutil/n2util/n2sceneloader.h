#pragma once
//------------------------------------------------------------------------------
/**
    @class ToolkitUtil::N2SceneLoader
    
    Loads a .n2 file into a SceneNodeTree using a N2ReflectionInfo object
    as template.
    
    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "toolkitutil/n2util/n2reflectioninfo.h"
#include "toolkitutil/scenenodetree.h"
#include "io/uri.h"
#include "io/binaryreader.h"
#include "util/stack.h"

//------------------------------------------------------------------------------
namespace ToolkitUtil
{
class N2SceneLoader : public Core::RefCounted
{
    __DeclareClass(N2SceneLoader);
public:
    /// constructor
    N2SceneLoader();
    /// destructor
    virtual ~N2SceneLoader();

    /// setup the loader
    void Setup(const Ptr<N2ReflectionInfo>& reflectionInfo);
    /// discard the loader
    void Discard();
    /// return true if loader has been setup
    bool IsValid() const;

    /// load a .n2 file into a SceneNodeTree
    Ptr<SceneNodeTree> Load(const IO::URI& uri, Logger& logger);

private:
    /// begin a new node
    void BeginNode(const Util::StringAtom& nodeType, const Util::StringAtom& nodeName, Logger& logger);
    /// read a data tag from the N2 file
    void ReadDataTag(const Util::FourCC& fourCC, const Ptr<IO::BinaryReader>& reader, Logger& logger);
    /// end current node
    void EndNode(Logger& logger);

    Ptr<N2ReflectionInfo> reflectionInfo;
    Ptr<SceneNodeTree> sceneNodeTree;
    Util::Stack<Ptr<SceneNode> > sceneNodeStack;
    bool isValid;
    bool validNodeType;
};

} // namespace ToolkitUtil
//------------------------------------------------------------------------------
