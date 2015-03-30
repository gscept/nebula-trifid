//------------------------------------------------------------------------------
//  mayaenvcmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaenvcmd.h"
#include "mayamain/mayadirectoryregistry.h"
#include "mayamain/mayalogger.h"
#include "mayautil/mayatype.h"
#include "util/string.h"
#include "io/ioserver.h"
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

namespace Maya
{
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
bool
MayaEnvCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaEnvCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaEnvCmd::creator()
{
    return new MayaEnvCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaEnvCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-gp", "-getproj");
    syntax.addFlag("-ge", "-getexport");
    syntax.addFlag("-gh", "-gethome");
    syntax.addFlag("-gt", "-gettemp");
    syntax.addFlag("-md", "-mkdir", MSyntax::kString);
    syntax.addFlag("-cf", "-copyfrom", MSyntax::kString);
    syntax.addFlag("-ct", "-copyto", MSyntax::kString);
    return syntax;
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaEnvCmd::doIt(const MArgList& argList)
{
    MStatus status;
    MArgDatabase args(syntax(), argList, &status);
    if (MS::kSuccess == status)
    {
        MayaDirectoryRegistry* reg = MayaDirectoryRegistry::Instance();

        // -getproj
        if (args.isFlagSet("-gp"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetProjectPath()));
            return MS::kSuccess;
        }

        // -getexport
        if (args.isFlagSet("-ge"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetExportPath()));
            return MS::kSuccess;
        }

        // -gethome
        if (args.isFlagSet("-gh"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetToolkitPath()));
            return MS::kSuccess;
        }

        // -gettemp
        if (args.isFlagSet("-gt"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetTempPath()));
            return MS::kSuccess;
        }

        // -mkdir
        if (args.isFlagSet("-md"))
        {
            MString argString;
            args.getFlagArgument("-md", 0, argString);
            String dir = MayaType::Cast<MString,String>(argString);
            if (IoServer::Instance()->CreateDirectory(dir))
            {
                return MS::kSuccess;
            }
            else
            {
                MayaLogger::Instance()->Error("Failed to create directory '%s'!\n", dir.AsCharPtr());
                return MS::kFailure;
            }
        }

        // -copyfrom -copyto
        if (args.isFlagSet("-cf") && args.isFlagSet("-ct"))
        {
            MString fromArg, toArg;
            args.getFlagArgument("-cf", 0, fromArg);
            args.getFlagArgument("-ct", 0, toArg);
            String from = MayaType::Cast<MString,String>(fromArg);
            String to = MayaType::Cast<MString,String>(toArg);
            if (IoServer::Instance()->CopyFile(from, to))
            {
                return MS::kSuccess;
            }
            else
            {
                MayaLogger::Instance()->Error("Failed to copy '%s' to '%s'!\n", from.AsCharPtr(), to.AsCharPtr());
                return MS::kFailure;
            }
        }
        MayaLogger::Instance()->Warning("rlEnv: nothing to do!\n");
        return MS::kSuccess;
    }
    else
    {
        MayaLogger::Instance()->Error("rlEnv: error parsing args!\n");
        return MS::kFailure;
    }
}

} // namespace Maya