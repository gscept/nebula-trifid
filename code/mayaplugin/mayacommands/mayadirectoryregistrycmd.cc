//------------------------------------------------------------------------------
//  mayadirectoryregistrycmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayadirectoryregistrycmd.h"
#include "mayamain/mayadirectoryregistry.h"
#include "mayamain/mayalogger.h"
#include "mayautil/mayatype.h"
#include <maya/MArgDatabase.h>

namespace Maya
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
bool
MayaDirectoryRegistryCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaDirectoryRegistryCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaDirectoryRegistryCmd::creator()
{
    return new MayaDirectoryRegistryCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaDirectoryRegistryCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-spp", "-setprojectpath", MSyntax::kString);
    syntax.addFlag("-gpp", "-getprojectpath");
    syntax.addFlag("-sep", "-setexportpath", MSyntax::kString);
	syntax.addFlag("-gep", "-getexportpath");
	syntax.addFlag("stp", "-settoolkitpath", MSyntax::kString);
	syntax.addFlag("-gtp", "-gettoolkitpath");
    
	
    return syntax;
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaDirectoryRegistryCmd::doIt(const MArgList& argList)
{
    MStatus status;
    MArgDatabase args(syntax(), argList, &status);
    if (MS::kSuccess == status)
    {
        MayaDirectoryRegistry* reg = MayaDirectoryRegistry::Instance();
        
        MString argString;
        if (args.isFlagSet("-spp"))
        {
            args.getFlagArgument("-spp", 0, argString);
            reg->SetProjectPath(MayaType::Cast<MString,String>(argString));
            return MS::kSuccess;
        }
        else if (args.isFlagSet("-gpp"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetProjectPath()));
            return MS::kSuccess;
        }
        else if (args.isFlagSet("-sep"))
        {
            args.getFlagArgument("-sep", 0, argString);
            reg->SetExportPath(MayaType::Cast<MString,String>(argString));
            return MS::kSuccess;
        }
        else if (args.isFlagSet("-gep"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetExportPath()));
            return MS::kSuccess;
        }
		else if (args.isFlagSet("-stp"))
		{
			args.getFlagArgument("-stp", 0, argString);
			reg->SetToolkitPath(MayaType::Cast<MString,String>(argString));
			return MS::kSuccess;
		}
        else if (args.isFlagSet("-gtp"))
        {
            this->setResult(MayaType::Cast<String,MString>(reg->GetToolkitPath()));
            return MS::kSuccess;
        }
        // fallthrough: an error occured
    }
    return MS::kFailure;
}

} // namespace Maya