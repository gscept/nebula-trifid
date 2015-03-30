//------------------------------------------------------------------------------
//  mayaexportscenecmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayacommands/mayaexportscenecmd.h"
#include "mayaextractors/mayasceneextractor.h"
#include "mayaloadsave/mayascenewriter.h"
#include "mayamain/mayalogger.h"
#include "mayautil/mayatype.h"
#include "mayacore/mayavariableregistry.h"
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
MayaExportSceneCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaExportSceneCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaExportSceneCmd::creator()
{
    return new MayaExportSceneCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaExportSceneCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-cat", "-category", MSyntax::kString);
    syntax.addFlag("-n", "-name", MSyntax::kString);
    syntax.addFlag("-loop", "-looping");    // OBSOLETE
    return syntax;
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaExportSceneCmd::doIt(const MArgList& argList)
{
    MStatus status;
    MArgDatabase args(syntax(), argList, &status);
    if (MS::kSuccess == status)
    {
        MString cat, name;
        if (args.isFlagSet("-cat"))
        {
            args.getFlagArgument("-cat", 0, cat);
        }
        else
        {
            MayaLogger::Instance()->Error("MayaExportSceneCmd: -category argument expected!\n");
            return MS::kFailure;
        }
        if (args.isFlagSet("-n"))
        {
            args.getFlagArgument("-n", 0, name);
        }
        else
        {
            MayaLogger::Instance()->Error("MayaExportSceneCmd: -name argument expected!\n");
            return MS::kFailure;
        }

        // setup global option variables
        MayaVariableRegistry* varRegistry = MayaVariableRegistry::Instance();
        varRegistry->Set<String>(MayaVariable::CategoryName, MayaType::Cast<MString,String>(cat));
        varRegistry->Set<String>(MayaVariable::ObjectName, MayaType::Cast<MString,String>(name));

        // create a Maya scene object which holds the extraction result
        Ptr<MayaScene> mayaScene = MayaScene::Create();
        mayaScene->Setup();

        // create the top level scene extractor
        Ptr<MayaSceneExtractor> sceneExtractor = MayaSceneExtractor::Create();
        sceneExtractor->Setup();
        MayaStatus::Code result = sceneExtractor->Extract(mayaScene);
    
        // FIXME: saving needs to be smarter!
        if (MayaStatus::Success == result)
        {
            IoServer* ioServer = IoServer::Instance();

            String dirName, fileName;
            dirName.Format("proj:intermediate/gfxlib/%s", cat.asChar());
            fileName.Format("%s/%s.xml", dirName.AsCharPtr(), name.asChar());

            ioServer->CreateDirectory(dirName);
            Ptr<Stream> stream = IoServer::Instance()->CreateStream(fileName);
            Ptr<MayaSceneWriter> writer = MayaSceneWriter::Create();
            writer->SetStream(stream);
            if (writer->Open())
            {
                writer->WriteMayaScene(mayaScene);
                writer->Close();
            }
        }
        sceneExtractor->Discard();
        mayaScene->Discard();
        if (result != MayaStatus::Error)
        {
            return MS::kSuccess;
        }
        else
        {
            MayaLogger::Instance()->Error("MayaExportSceneCmd: error during scene extraction!\n");
            return MS::kFailure;
        }
    }
    else
    {
        MayaLogger::Instance()->Error("MayaExportSceneCmd: error parsing args!\n");
        return MS::kFailure;
    }
}

} // namespace Maya