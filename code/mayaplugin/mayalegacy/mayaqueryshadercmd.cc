//------------------------------------------------------------------------------
//  mayaqueryshadercmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaqueryshadercmd.h"
#include "util/string.h"
#include "util/stringatom.h"
#include "mayashaders/mayashaderregistry.h"
#include "mayamain/mayalogger.h"
#include "mayautil/mayatype.h"
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

namespace Maya
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
bool
MayaQueryShaderCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaQueryShaderCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaQueryShaderCmd::creator()
{
    return new MayaQueryShaderCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaQueryShaderCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-si",     "-shaderindex", MSyntax::kLong);
    syntax.addFlag("-pi",     "-paramindex",  MSyntax::kLong);
    syntax.addFlag("-sn",     "-shadername",  MSyntax::kString);
    syntax.addFlag("-pn",     "-paramname",   MSyntax::kString);
    syntax.addFlag("-gns",    "-getnumshaders");
    syntax.addFlag("-gnp",    "-getnumparams");
    syntax.addFlag("-gsi",    "-getshaderindex");
    syntax.addFlag("-gpi",    "-getparamindex");
    syntax.addFlag("-gsf",    "-getshaderfile");
    syntax.addFlag("-gsn",    "-getshadername");
    syntax.addFlag("-gpt",    "-getparamtype");
    syntax.addFlag("-gpn",    "-getparamname");
    syntax.addFlag("-gpl",    "-getparamlabel");
    syntax.addFlag("-gpg",    "-getparamgui");
    syntax.addFlag("-gpe",    "-getparamexport");
    syntax.addFlag("-gmn",    "-getparammin");
    syntax.addFlag("-gmx",    "-getparammax");
    syntax.addFlag("-gdf",    "-getparamdefault");
    syntax.addFlag("-obs",    "-isobsolete");
    return syntax;
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaQueryShaderCmd::doIt(const MArgList& argList)
{
    MStatus status;
    MArgDatabase args(syntax(), argList, &status);
    if (MS::kSuccess == status)
    {
        MayaShaderRegistry* shdRegistry = MayaShaderRegistry::Instance();

        // lookup shader index
        IndexT shaderIndex = InvalidIndex;
        if (args.isFlagSet("-si"))
        {
            args.getFlagArgument("-si", 0, shaderIndex);
            if ((shaderIndex < 0) || (shaderIndex >= shdRegistry->GetNumShaders()))
            {
                MayaLogger::Instance()->Error("MayaQueryShaderCmd: invalid shader index '%d'!\n", shaderIndex);
                return MS::kFailure;
            }
        }
        else if (args.isFlagSet("-sn"))
        {
            MString shaderName;
            args.getFlagArgument("-sn", 0, shaderName);
            shaderIndex = shdRegistry->GetShaderIndexById(MayaType::Cast<MString,StringAtom>(shaderName));
            if (InvalidIndex == shaderIndex)
            {
                MayaLogger::Instance()->Error("MayaQueryShaderCmd: invalid shader name '%s'!\n", shaderName.asChar());
                return MS::kFailure;
            }
        }

        // lookup shader param index
        IndexT paramIndex = InvalidIndex;
        if (args.isFlagSet("-pi") || args.isFlagSet("-pn"))
        {
            if (InvalidIndex == shaderIndex)
            {
                MayaLogger::Instance()->Error("MayaQueryShaderCmd: shader index or name required!\n");
                return MS::kFailure;
            }
            const MayaShader& shd = shdRegistry->GetShaderByIndex(shaderIndex);
            if (args.isFlagSet("-pi"))
            {
                args.getFlagArgument("-pi", 0, paramIndex);
                if ((paramIndex < 0) || (paramIndex >= shd.GetNumParams()))
                {
                    MayaLogger::Instance()->Error("MayaQueryShaderCmd: invalid param index '%d' for shader '%s'!\n", paramIndex, shd.GetId().Value());
                    return MS::kFailure;
                }
            }
            else if (args.isFlagSet("-pn"))
            {
                MString paramName;
                args.getFlagArgument("-pn", 0, paramName);
                paramIndex = shd.GetParamIndexById(MayaType::Cast<MString,StringAtom>(paramName));
                if (InvalidIndex == paramIndex)
                {
                    MayaLogger::Instance()->Error("MayaQueryShaderCmd: param '%s' not found on shader '%s'!\n", paramName.asChar(), shd.GetId().Value());
                    return MS::kFailure;
                }
            }
        }

        // -getnumshaders
        if (args.isFlagSet("-gns"))
        {
            this->setResult(shdRegistry->GetNumShaders());
            return MS::kSuccess;
        }

        // -getshaderindex
        if (args.isFlagSet("-gsi"))
        {
            this->setResult(shaderIndex);
            return MS::kSuccess;
        }

        // -getparamindex
        if (args.isFlagSet("-gpi"))
        {
            this->setResult(paramIndex);
            return MS::kSuccess;
        }

        // shader query stuff
        if (args.isFlagSet("-gnp") ||
            args.isFlagSet("-obs") ||
            args.isFlagSet("-gsf") ||
            args.isFlagSet("-gsn"))
        {
            if (InvalidIndex == shaderIndex)
            {
                MayaLogger::Instance()->Error("MayaQueryShaderCmd: shader name or index required!\n");
                return MS::kFailure;
            }
            const MayaShader& shd = shdRegistry->GetShaderByIndex(shaderIndex);

            // -getnumparams
            if (args.isFlagSet("-gnp"))
            {
                this->setResult(shd.GetNumParams());
                return MS::kSuccess;
            }

            // -isobsolete
            if (args.isFlagSet("-obs"))
            {
                this->setResult(shd.IsObsolete());
                return MS::kSuccess;
            }

            // -getshaderfile
            if (args.isFlagSet("-gsf"))
            {
                this->setResult(MayaType::Cast<StringAtom,MString>(shd.GetNebulaShaderName()));
                return MS::kSuccess;
            }

            // -getshadername
            if (args.isFlagSet("-gsn"))
            {
                this->setResult(MayaType::Cast<StringAtom,MString>(shd.GetId()));
                return MS::kSuccess;
            }
        }

        // parameter queries
        if (args.isFlagSet("-gpt") ||
            args.isFlagSet("-gpn") ||
            args.isFlagSet("-gpl") ||
            args.isFlagSet("-gpg") ||
            args.isFlagSet("-gpe") ||
            args.isFlagSet("-gmn") ||
            args.isFlagSet("-gmx") ||
            args.isFlagSet("-gdf"))
        {
            if (InvalidIndex == paramIndex)
            {
                MayaLogger::Instance()->Error("MayaQueryShaderCmd: param name or index required!\n");
                return MS::kFailure;
            }
            n_assert(InvalidIndex != shaderIndex);
            const MayaShader& shd = shdRegistry->GetShaderByIndex(shaderIndex);
            const MayaShaderParam& param = shd.GetParamByIndex(paramIndex);

            // -getparamtype
            if (args.isFlagSet("-gpt"))
            {
                String paramType = MayaShaderParam::TypeToString(param.GetType());
                this->setResult(MayaType::Cast<String,MString>(paramType));
                return MS::kSuccess;
            }

            // -getparamname
            if (args.isFlagSet("-gpn"))
            {
                const StringAtom& paramName = param.GetId();
                this->setResult(MayaType::Cast<StringAtom,MString>(paramName));
                return MS::kSuccess;
            }

            // -getparamlabel
            if (args.isFlagSet("-gpl"))
            {
                const StringAtom& paramLabel = param.GetLabel();
                this->setResult(MayaType::Cast<StringAtom,MString>(paramLabel));
                return MS::kSuccess;
            }

            // -getparamgui
            if (args.isFlagSet("-gpg"))
            {
                this->setResult(param.IsGuiParam());
                return MS::kSuccess;
            }

            // -getparamexport
            if (args.isFlagSet("-gpe"))
            {
                this->setResult(param.IsExportParam());
                return MS::kSuccess;
            }

            // -getparammin
            if (args.isFlagSet("-gmn"))
            {
                const String& minValue = param.GetMinValue();
                this->setResult(MayaType::Cast<StringAtom,MString>(minValue));
                return MS::kSuccess;
            }

            // -getparammax
            if (args.isFlagSet("-gmx"))
            {
                const String& maxValue = param.GetMaxValue();
                this->setResult(MayaType::Cast<StringAtom,MString>(maxValue));
                return MS::kSuccess;
            }

            // -getparamdefault
            if (args.isFlagSet("-gdf"))
            {
                const String& defValue = param.GetDefaultValue();
                Array<String> tokens = defValue.Tokenize("\t ,");
                MayaShaderParam::Type paramType = param.GetType();

                switch (paramType)
                {
                    case MayaShaderParam::Vector:
                    case MayaShaderParam::Color:
                        {
                            MDoubleArray res;
                            IndexT i;
                            for (i = 0; i < 4; i++)
                            {
                                res.append(tokens[0].AsFloat());
                            }
                            this->setResult(res);
                        }
                        return MS::kSuccess;

                    case MayaShaderParam::Envelope:
                        {
                            MDoubleArray res;
                            IndexT i;
                            for (i = 0; i < 8; i++)
                            {
                                res.append(tokens[0].AsFloat());
                            }
                            this->setResult(res);
                        }
                        return MS::kSuccess;

                    default:
                        this->setResult(MayaType::Cast<String,MString>(defValue));
                        return MS::kSuccess;
                }
            }
        }
        MayaLogger::Instance()->Warning("MayaQueryShaderCmd: nothing to do!\n");
        return MS::kSuccess;
    }
    else
    {
        MayaLogger::Instance()->Error("MayaQueryShaderCmd: error parsing args!\n");
        return MS::kFailure;
    }
}

} // namespace Maya