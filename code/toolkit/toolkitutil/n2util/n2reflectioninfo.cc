//------------------------------------------------------------------------------
//  n2reflectioninfo.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n2reflectioninfo.h"

namespace ToolkitUtil
{
__ImplementClass(ToolkitUtil::N2ReflectionInfo, 'N2RI', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
N2ReflectionInfo::N2ReflectionInfo() :
    queryNodeTypeIndex(InvalidIndex),
    inBegin(false),
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
N2ReflectionInfo::~N2ReflectionInfo()
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
N2ReflectionInfo::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->SetupN2SceneNodeInfos();
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::Discard()
{
    n_assert(this->IsValid());
    this->nodeTypeInfos.Clear();
    this->nodeTypeIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
N2ReflectionInfo::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::BeginNodeType(const StringAtom& typeName)
{
    n_assert(!this->nodeTypeIndexMap.Contains(typeName));
    n_assert(!this->inBegin);
    this->inBegin = true;
    NodeTypeInfo newNodeTypeInfo;
    newNodeTypeInfo.name = typeName;
    this->nodeTypeInfos.Append(newNodeTypeInfo);
    this->nodeTypeIndexMap.Add(typeName, this->nodeTypeInfos.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::AddCmd(const String& cmdProto, FourCC cmdFourCC)
{
    n_assert(this->inBegin);

    NodeTypeInfo& nodeTypeInfo = this->nodeTypeInfos.Back();

    // split command proto string, format is like cmd protos
    // in the Nebula2 scripting system: o_name_iiii
    Array<String> tokens = cmdProto.Tokenize("_");
    n_assert(tokens.Size() == 3);
    StringAtom cmdName = tokens[1];
    const String& inputParams = tokens[2];
    n_assert(!nodeTypeInfo.cmdFourCCIndexMap.Contains(cmdFourCC))
    n_assert(!nodeTypeInfo.cmdNameIndexMap.Contains(cmdName));

    // create new cmd proto object
    CmdProto newCmdProto;
    newCmdProto.cmdName = cmdName;
    newCmdProto.cmdFourCC = cmdFourCC;
    IndexT i;
    for (i = 0; i < inputParams.Length(); i++)
    {
        switch (inputParams[i])
        {
            case 'i':   newCmdProto.inputTypes.Append(Variant::Int); break;
            case 'f':   newCmdProto.inputTypes.Append(Variant::Float); break;
            case 's':   newCmdProto.inputTypes.Append(Variant::String); break;
            case 'b':   newCmdProto.inputTypes.Append(Variant::Bool); break;
            default:    break;
        }
    }    
    
    // add cmd proto to node info
    nodeTypeInfo.cmdProtos.Append(newCmdProto);
    nodeTypeInfo.cmdFourCCIndexMap.Add(cmdFourCC, nodeTypeInfo.cmdProtos.Size() - 1);
    nodeTypeInfo.cmdNameIndexMap.Add(cmdName, nodeTypeInfo.cmdProtos.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::EndNodeType()
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
N2ReflectionInfo::HasNodeType(const StringAtom& typeName)
{
    n_assert(!this->inBegin);
    return this->nodeTypeIndexMap.Contains(typeName);
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::SetQueryNodeType(const StringAtom& typeName)
{
    n_assert(!this->inBegin);
    n_assert(this->HasNodeType(typeName));
    this->queryNodeTypeIndex = this->nodeTypeIndexMap[typeName];
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
N2ReflectionInfo::GetQueryNodeType() const
{
    n_assert(!this->inBegin);
    n_assert(this->queryNodeTypeIndex != InvalidIndex);
    return this->nodeTypeInfos[this->queryNodeTypeIndex].name;
}

//------------------------------------------------------------------------------
/**
*/
bool
N2ReflectionInfo::HasCmdProtoByFourCC(FourCC cmdFourCC) const
{
    n_assert(!this->inBegin);
    n_assert(this->queryNodeTypeIndex != InvalidIndex);
    const NodeTypeInfo& nodeTypeInfo = this->nodeTypeInfos[this->queryNodeTypeIndex];
    return nodeTypeInfo.cmdFourCCIndexMap.Contains(cmdFourCC);
}

//------------------------------------------------------------------------------
/**
*/
bool
N2ReflectionInfo::HasCmdProtoByName(const StringAtom& cmdName) const
{
    n_assert(!this->inBegin);
    n_assert(this->queryNodeTypeIndex != InvalidIndex);
    const NodeTypeInfo& nodeTypeInfo = this->nodeTypeInfos[this->queryNodeTypeIndex];
    return nodeTypeInfo.cmdNameIndexMap.Contains(cmdName);
}

//------------------------------------------------------------------------------
/**
*/
const N2ReflectionInfo::CmdProto&
N2ReflectionInfo::GetCmdProtoByFourCC(FourCC cmdFourCC) const
{
    n_assert(!this->inBegin);
    n_assert(this->queryNodeTypeIndex != InvalidIndex);
    const NodeTypeInfo& nodeTypeInfo = this->nodeTypeInfos[this->queryNodeTypeIndex];
    n_assert(nodeTypeInfo.cmdFourCCIndexMap.Contains(cmdFourCC));
    return nodeTypeInfo.cmdProtos[nodeTypeInfo.cmdFourCCIndexMap[cmdFourCC]];
}

//------------------------------------------------------------------------------
/**
*/
const N2ReflectionInfo::CmdProto&
N2ReflectionInfo::GetCmdProtoByName(const StringAtom& cmdName) const
{
    n_assert(!this->inBegin);
    n_assert(this->queryNodeTypeIndex != InvalidIndex);
    const NodeTypeInfo& nodeTypeInfo = this->nodeTypeInfos[this->queryNodeTypeIndex];
    n_assert(nodeTypeInfo.cmdNameIndexMap.Contains(cmdName));
    return nodeTypeInfo.cmdProtos[nodeTypeInfo.cmdNameIndexMap[cmdName]];
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutSceneNodeCmds()
{
    this->AddCmd("v_addanimator_s",         'ADDA');
    this->AddCmd("v_setlocalbox_ffffff",    'SLCB');
    this->AddCmd("v_setrenderpri_i",        'SRPR');
    this->AddCmd("v_addhints_i",            'ADDH');
    this->AddCmd("v_setintattr_si",         'SINA');
    this->AddCmd("v_setfloatattr_sf",       'SFLA');
    this->AddCmd("v_setboolattr_sb",        'SBOA');
    this->AddCmd("v_setstringattr_ss",      'SSTA');
    this->AddCmd("v_setvector3attr_sfff",   'SV3A');
    this->AddCmd("v_setvector4attr_sffff",  'SV4A');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutTransformNodeCmds()
{
    this->AddCmd("v_setactive_b",           'SACT');
    this->AddCmd("v_setlockviewer_b",       'SLKV');
    this->AddCmd("v_setbillboard_b",        'SBLB');
    this->AddCmd("v_setviewspace_b",        'SVSP');
    this->AddCmd("v_setposition_fff",       'SPOS');
    this->AddCmd("v_setquat_ffff",          'SQUT');
    this->AddCmd("v_setscale_fff",          'SSCL');
    this->AddCmd("v_setscalepivot_fff",     'SSCP');
    this->AddCmd("v_setrotatepivot_fff",    'SRTP');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutAbstractShaderNodeCmds()
{
    this->AddCmd("v_settexture_ss",   'STXT');
    this->AddCmd("v_setint_si",       'SINT');
    this->AddCmd("v_setfloat_sf",     'SFLT');
    this->AddCmd("v_setvector_sffff", 'SVEC');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutMaterialNodeCmds()
{
    this->AddCmd("v_setshader_s", 'SSHD');
    this->AddCmd("v_setmayashadername_s", 'SMSN');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutLodNodeCmds()
{
    this->AddCmd("v_appendthreshold_f", 'ATHR');
    this->AddCmd("v_setmindistance_f",  'SMID');
    this->AddCmd("v_setmaxdistance_f",  'SMAD');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutShapeNodeCmds()
{
    this->AddCmd("v_setmesh_s", 'SMSH');
    this->AddCmd("v_setgroupindex_i", 'SGRI');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutParticleShapeNode2Cmds()
{
    this->AddCmd("v_setemissionduration_f", 'SEMD');
    this->AddCmd("v_setloop_b", 'SLOP');
    this->AddCmd("v_setactivitydistance_f", 'SACD');
    this->AddCmd("v_setrenderoldestfirst_b", 'SROF');
    this->AddCmd("v_setbillboardorientation_b", 'SBBO');
    this->AddCmd("v_setstartrotationmin_f", 'SRMN');
    this->AddCmd("v_setstartrotationmax_f", 'SRMX');
    this->AddCmd("v_setgravity_f", 'SGRV');
    this->AddCmd("v_setparticlestretch_f", 'SPST');
    this->AddCmd("v_settiletexture_i", 'STTX');
    this->AddCmd("v_setstretchtostart_b", 'SSTS');
    this->AddCmd("v_setprecalctime_f", 'SPCT');
    this->AddCmd("v_setstretchdetail_i", 'SSDT');
    this->AddCmd("v_setviewanglefade_b", 'SVAF');
    this->AddCmd("v_setstartdelay_f", 'STDL');
    this->AddCmd("v_setemissionfrequency_ffffffffi", 'SCVA');
    this->AddCmd("v_setparticlelifetime_ffffffffi", 'SCVB');
    this->AddCmd("v_setparticlergb_ffffffffffffff", 'SCVC');
    this->AddCmd("v_setparticlespreadmin_ffffffffi", 'SCVD');
    this->AddCmd("v_setparticlespreadmax_ffffffffi", 'SCVE');
    this->AddCmd("v_setparticlestartvelocity_ffffffffi", 'SCVF');
    this->AddCmd("v_setparticlerotationvelocity_ffffffffi", 'SCVH');
    this->AddCmd("v_setparticlesize_ffffffffi", 'SCVJ');
    this->AddCmd("v_setparticlemass_ffffffffi", 'SCVL');
    this->AddCmd("v_settimemanipulator_ffffffffi", 'STMM');
    this->AddCmd("v_setparticlealpha_ffffffffi", 'SCVM');
    this->AddCmd("v_setparticlevelocityfactor_ffffffffi", 'SCVN');
    this->AddCmd("v_setparticleairresistance_ffffffffi", 'SCVQ');
    this->AddCmd("v_setemittervelocityinfluence_ffffffffi", 'SEVI');
    this->AddCmd("v_setparticlevelocityrandomize_f", 'SCVR');
    this->AddCmd("v_setparticlerotationrandomize_f", 'SCVS');
    this->AddCmd("v_setparticlesizerandomize_f", 'SCVT');
    this->AddCmd("v_setrandomrotdir_b", 'SCVU');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutParticleSkinShapeNode2Cmds()
{
    this->AddCmd("v_setemissionduration_f", 'SEMD');
    this->AddCmd("v_setloop_b", 'SLOP');
    this->AddCmd("v_setactivitydistance_f", 'SACD');
    this->AddCmd("v_setrenderoldestfirst_b", 'SROF');
    this->AddCmd("v_setbillboardorientation_b", 'SBBO');
    this->AddCmd("v_setstartrotationmin_f", 'SRMN');
    this->AddCmd("v_setstartrotationmax_f", 'SRMX');
    this->AddCmd("v_setgravity_f", 'SGRV');
    this->AddCmd("v_setparticlestretch_f", 'SPST');
    this->AddCmd("v_settiletexture_i", 'STTX');
    this->AddCmd("v_setstretchtostart_b", 'SSTS');
    this->AddCmd("v_setprecalctime_f", 'SPCT');
    this->AddCmd("v_setstretchdetail_i", 'SSDT');
    this->AddCmd("v_setviewanglefade_b", 'SVAF');
    this->AddCmd("v_setstartdelay_f", 'STDL');
    this->AddCmd("v_setemissionfrequency_ffffffffi", 'SCVA');
    this->AddCmd("v_setparticlelifetime_ffffffffi", 'SCVB');
    this->AddCmd("v_setparticlergb_ffffffffffffff", 'SCVC');
    this->AddCmd("v_setparticlespreadmin_ffffffffi", 'SCVD');
    this->AddCmd("v_setparticlespreadmax_ffffffffi", 'SCVE');
    this->AddCmd("v_setparticlestartvelocity_ffffffffi", 'SCVF');
    this->AddCmd("v_setparticlerotationvelocity_ffffffffi", 'SCVH');
    this->AddCmd("v_setparticlesize_ffffffffi", 'SCVJ');
    this->AddCmd("v_setparticlemass_ffffffffi", 'SCVL');
    this->AddCmd("v_settimemanipulator_ffffffffi", 'STMM');
    this->AddCmd("v_setparticlealpha_ffffffffi", 'SCVM');
    this->AddCmd("v_setparticlevelocityfactor_ffffffffi", 'SCVN');
    this->AddCmd("v_setparticleairresistance_ffffffffi", 'SCVQ');
    this->AddCmd("v_setemittervelocityinfluence_ffffffffi", 'SEVI');
    this->AddCmd("v_setparticlevelocityrandomize_f", 'SCVR');
    this->AddCmd("v_setparticlerotationrandomize_f", 'SCVS');
    this->AddCmd("v_setparticlesizerandomize_f", 'SCVT');
    this->AddCmd("v_setrandomrotdir_b", 'SCVU');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutCharacter3SkinAnimatorCmds()
{
    this->AddCmd("v_setanim_s",                 'SANM');
    this->AddCmd("v_beginjoints_i",             'BJNT');
    this->AddCmd("v_setjoint_iiffffffffffs",    'SJNT');
    this->AddCmd("v_setvariations_s",           'SVRT');
    this->AddCmd("v_beginvariations_i",         'BGVT');
    this->AddCmd("v_setvariation_iis",          'STVT');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutCharacter3SkinShapeNodeCmds()
{
    this->AddCmd("v_beginfragments_i",              'BGFR');
    this->AddCmd("v_setfraggroupindex_ii",          'SFGI');
    this->AddCmd("v_beginjointpalette_ii",          'BGJP');
    this->AddCmd("v_setjointindices_iiiiiiiiii",    'SJID');
    this->AddCmd("v_setskincategory_s",             'SSCT');
    this->AddCmd("s_getskincategory_v",             'GSCT');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutMultilayeredNodeCmds()
{
    this->AddCmd("v_setuvstretch_if",     'STUS');
    this->AddCmd("v_setspecintensity_if", 'SSPI');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutAnimatorCmds()
{
    this->AddCmd("v_setchannel_s",  'SCHN');
    this->AddCmd("v_setlooptype_s", 'SLPT');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutShaderAnimatorCmds()
{
    this->AddCmd("v_setparamname_s", 'SPNM');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutFloatAnimatorCmds()
{
    this->AddCmd("v_addkey_ff", 'ADDK');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutIntAnimatorCmds()
{
    this->AddCmd("v_addkey_fi", 'ADDK');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutTransformAnimatorCmds()
{
    this->AddCmd("v_addposkey_ffff",    'ADPK');
    this->AddCmd("v_addeulerkey_ffff",  'ADEK');
    this->AddCmd("v_addscalekey_ffff",  'ADSK');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutTransformCurveAnimatorCmds()
{
    this->AddCmd("v_setanimation_s",      'SANI');
    this->AddCmd("v_setanimationgroup_i", 'SAGR');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutUVAnimatorCmds()
{
    this->AddCmd("v_addposkey_ifff",    'ADPK');
    this->AddCmd("v_addeulerkey_ifff",  'ADEK');
    this->AddCmd("v_addscalekey_ifff",  'ADSK');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::PutVectorAnimatorCmds()
{
    this->AddCmd("v_addkey_fffff", 'ADDK');
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::SetupN2SceneNodeInfos()
{
    // ntransformnode
    this->BeginNodeType("ntransformnode");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->EndNodeType();

    // nlodnode
    this->BeginNodeType("nlodnode");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutLodNodeCmds();
    this->EndNodeType();

    // nshapenode
    this->BeginNodeType("nshapenode");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutAbstractShaderNodeCmds();
    this->PutMaterialNodeCmds();
    this->PutShapeNodeCmds();
    this->EndNodeType();

    // nmultilayerednode
    this->BeginNodeType("nmultilayerednode");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutAbstractShaderNodeCmds();
    this->PutMaterialNodeCmds();
    this->PutShapeNodeCmds();
    this->PutMultilayeredNodeCmds();
    this->EndNodeType();

    // nparticleshapenode2
    this->BeginNodeType("nparticleshapenode2");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutAbstractShaderNodeCmds();
    this->PutMaterialNodeCmds();
    this->PutShapeNodeCmds();
    this->PutParticleShapeNode2Cmds();
    this->EndNodeType();

    // nparticleskinshapenode2cmds
    this->BeginNodeType("nparticleskinshapenode2cmds");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutAbstractShaderNodeCmds();
    this->PutMaterialNodeCmds();
    this->PutShapeNodeCmds();
    this->PutCharacter3SkinShapeNodeCmds();    
    this->PutParticleSkinShapeNode2Cmds();
    this->EndNodeType();
    
    // ncharacter3node
    this->BeginNodeType("ncharacter3node");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->EndNodeType();

    // ncharacter3skinanimator
    this->BeginNodeType("ncharacter3skinanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutCharacter3SkinAnimatorCmds();
    this->EndNodeType();

    // ncharacter3skinshapenode
    this->BeginNodeType("ncharacter3skinshapenode");
    this->PutSceneNodeCmds();
    this->PutTransformNodeCmds();
    this->PutAbstractShaderNodeCmds();
    this->PutMaterialNodeCmds();
    this->PutShapeNodeCmds();
    this->PutCharacter3SkinShapeNodeCmds();    
    this->EndNodeType();

    // nfloatanimator
    this->BeginNodeType("nfloatanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutShaderAnimatorCmds();
    this->PutFloatAnimatorCmds();    
    this->EndNodeType();

    // nintanimator
    this->BeginNodeType("nintanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutShaderAnimatorCmds();
    this->PutIntAnimatorCmds();    
    this->EndNodeType();

    // ntransformanimator
    this->BeginNodeType("ntransformanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutTransformAnimatorCmds();
    this->EndNodeType();

    // ntransformcurveanimator
    this->BeginNodeType("ntransformcurveanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutTransformCurveAnimatorCmds();
    this->EndNodeType();

    // nuvanimator
    this->BeginNodeType("nuvanimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutUVAnimatorCmds();
    this->EndNodeType();

    // nvectoranimator
    this->BeginNodeType("nvectoranimator");
    this->PutSceneNodeCmds();
    this->PutAnimatorCmds();
    this->PutShaderAnimatorCmds();
    this->PutVectorAnimatorCmds();
    this->EndNodeType();
}

//------------------------------------------------------------------------------
/**
*/
void
N2ReflectionInfo::DumpReflectionInfo(Logger& logger)
{
    IndexT nodeTypeIndex;
    for (nodeTypeIndex = 0; nodeTypeIndex < this->nodeTypeInfos.Size(); nodeTypeIndex++)
    {
        const NodeTypeInfo& curNodeType = this->nodeTypeInfos[nodeTypeIndex];
        logger.Print("\n=== %s ===\n", curNodeType.name.Value());
        IndexT cmdIndex;
        for (cmdIndex = 0; cmdIndex < curNodeType.cmdProtos.Size(); cmdIndex++)
        {
            const CmdProto& cmdProto = curNodeType.cmdProtos[cmdIndex];
            String paramStr;
            IndexT paramIndex;
            for (paramIndex = 0; paramIndex < cmdProto.inputTypes.Size(); paramIndex++)
            {
                paramStr.Append(Variant::TypeToString(cmdProto.inputTypes[paramIndex]));
                if (paramIndex < (cmdProto.inputTypes.Size() - 1))
                {
                    paramStr.Append(", ");
                }
            }
            logger.Print("    %s(%s): %s\n", cmdProto.cmdName.Value(), paramStr.AsCharPtr(), cmdProto.cmdFourCC.AsString().AsCharPtr());
        }
    }
}

} // namespace ToolkitUtil