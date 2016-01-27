#pragma once
//------------------------------------------------------------------------------
/**
    @class ToolkitUtil::N2ReflectionInfo
    
    Manages reflection information about Nebula2 scene nodes
    (types and attributes).
    
    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/stringatom.h"
#include "util/fourcc.h"
#include "util/dictionary.h"
#include "util/variant.h"
#include "toolkitutil/logger.h"

//------------------------------------------------------------------------------
namespace ToolkitUtil
{
class N2ReflectionInfo : public Core::RefCounted
{
    __DeclareClass(N2ReflectionInfo);
public:
    /// a command proto type
    struct CmdProto
    {
        Util::StringAtom cmdName;
        Util::FourCC cmdFourCC;
        Util::Array<Util::Variant::Type> inputTypes;
    };

    /// constructor
    N2ReflectionInfo();
    /// destructor
    virtual ~N2ReflectionInfo();

    /// setup the object
    void Setup();
    /// discard the object
    void Discard();
    /// check if object is valid
    bool IsValid() const;
    /// dump reflection info to provided logger
    void DumpReflectionInfo(Logger& logger);
    
    /// return true if node type exists
    bool HasNodeType(const Util::StringAtom& nodeType);
    /// set current query node type
    void SetQueryNodeType(const Util::StringAtom& nodeType);
    /// get current query node type
    const Util::StringAtom& GetQueryNodeType() const;
    /// return true if command proto exists by FourCC
    bool HasCmdProtoByFourCC(Util::FourCC cmdFourCC) const;
    /// return true if command proto exists by name
    bool HasCmdProtoByName(const Util::StringAtom& cmdName) const;
    /// get command proto by command fourCC
    const CmdProto& GetCmdProtoByFourCC(Util::FourCC cmdFourCC) const;
    /// get command proto by command name
    const CmdProto& GetCmdProtoByName(const Util::StringAtom& cmdName) const;

private:
    /// setup scene node infos for all accepted N2 classes
    void SetupN2SceneNodeInfos();
    /// begin node reflection info definion
    void BeginNodeType(const Util::StringAtom& className);
    /// add a command to the current node type
    void AddCmd(const Util::String& cmdProto, Util::FourCC cmdFourCC);
    /// end node reflection info
    void EndNodeType();
    
    /// put nscenenode cmds
    void PutSceneNodeCmds();
    /// put ntransformnode cmds
    void PutTransformNodeCmds();
    /// put nabstractshadernode cmds
    void PutAbstractShaderNodeCmds();
    /// put nmaterialnode cmds
    void PutMaterialNodeCmds();
    /// put nlodnode cmds
    void PutLodNodeCmds();
    /// put nshapenode cmds
    void PutShapeNodeCmds();
    /// put nparticleshapenode2 cmds
    void PutParticleShapeNode2Cmds();
    /// put nparticleskinshapenode2 cmds
    void PutParticleSkinShapeNode2Cmds();
    /// put ncharacter3skinanimator cmds
    void PutCharacter3SkinAnimatorCmds();
    /// put ncharacter3skinshapenode cmds
    void PutCharacter3SkinShapeNodeCmds();
    /// put nmultilayerednode cmds
    void PutMultilayeredNodeCmds();
    /// put nanimator cmds
    void PutAnimatorCmds();
    /// put nshaderanimator cmds
    void PutShaderAnimatorCmds();
    /// put nfloatanimator cmds
    void PutFloatAnimatorCmds();
    /// put nintanimator cmds
    void PutIntAnimatorCmds();
    /// put ntransformanimator cmds
    void PutTransformAnimatorCmds();
    /// put ntransformcurveanimator cmds
    void PutTransformCurveAnimatorCmds();
    /// put nuvanimator cmds
    void PutUVAnimatorCmds();
    /// put nvectoranimator cmds
    void PutVectorAnimatorCmds();

    /// node type info
    struct NodeTypeInfo
    {
        Util::StringAtom name;
        Util::Array<CmdProto> cmdProtos;
        Util::Dictionary<Util::FourCC,IndexT> cmdFourCCIndexMap;
        Util::Dictionary<Util::StringAtom,IndexT> cmdNameIndexMap;
    };

    Util::Array<NodeTypeInfo> nodeTypeInfos;
    Util::Dictionary<Util::StringAtom,IndexT> nodeTypeIndexMap;
    IndexT queryNodeTypeIndex;
    bool inBegin;
    bool isValid;
};

}
//------------------------------------------------------------------------------
