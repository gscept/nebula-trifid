//------------------------------------------------------------------------------
//  suiconverterapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "suiconverterapp.h"
#include "toolkitutil/applauncher.h"

namespace Toolkit
{
using namespace IO;
using namespace Util;
using namespace ToolkitUtil;

//------------------------------------------------------------------------------
/**
*/
bool
SUIConverterApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {   
        this->srcDir = this->projectInfo.GetAttr("SrcDir");
        this->dstDir = this->projectInfo.GetAttr("DstDir");
        if (Platform::Wii == this->platform)
        {
            this->nw4rToolPath = this->projectInfo.GetPathAttr("NW4RLayoutConverterTool");
            this->arcToolPath  = this->projectInfo.GetPathAttr("ArchiverTool");
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SUIConverterApp::ShowHelp()
{
    n_printf("Nebula3 SUI resource converter.\n"
             "(C) Radon Labs GmbH 2009.\n"
             "-help       -- display this help\n"
             "-platform   -- select platform (win32, xbox360, wii, ps3)\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SUIConverterApp::Run()
{
    // parse command line args
    if (!this->ParseCmdLineArgs())
    {
        return;
    }

    // setup the project info object
    if (!this->SetupProjectInfo())
    {
        return;
    }

    // perform conversion
    if (!this->ConvertAll())
    {
        this->SetReturnCode(10);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
SUIConverterApp::ConvertAll()
{
    bool result = true;

    // the source directory is platform specific...
    String suiSrcDir;
    suiSrcDir.Format("%s/sui/%s", this->srcDir.AsCharPtr(), Platform::ToString(this->platform).AsCharPtr());
    Array<String> items = IoServer::Instance()->ListDirectories(suiSrcDir, "*");
    IndexT i;
    for (i = 0; i < items.Size(); i++)
    {
        if ((items[i] != "CVS") && (items[i] != ".svn"))
        {
            result &= this->ConvertSingle(items[i]);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool
SUIConverterApp::ConvertSingle(const String& itemName)
{
    switch (this->platform)
    {
        case Platform::Wii: return this->ConvertSingleWii(itemName);
        default: return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
SUIConverterApp::ConvertSingleWii(const String& itemName)
{
    IoServer* ioServer = IoServer::Instance();

    // setup file names
    String suiSrcDir, tmpDir, suiDstDir, dstArcFile;
    suiSrcDir.Format("%s/sui/wii/%s", this->srcDir.AsCharPtr(), itemName.AsCharPtr());
    suiSrcDir = AssignRegistry::Instance()->ResolveAssignsInString(suiSrcDir);
    tmpDir.Format("proj:intermediate_wii/sui/%s", itemName.AsCharPtr());
    tmpDir = AssignRegistry::Instance()->ResolveAssignsInString(tmpDir);
    suiDstDir.Format("%s/sui", this->dstDir.AsCharPtr());
    dstArcFile.Format("%s/sui/%s.arc", this->dstDir.AsCharPtr(), itemName.AsCharPtr());
    dstArcFile = AssignRegistry::Instance()->ResolveAssignsInString(dstArcFile);

    // first setup an intermediate directory
    if (ioServer->DirectoryExists(tmpDir))
    {
        ioServer->DeleteDirectory(tmpDir);
    }
    ioServer->CreateDirectory(tmpDir);

    // launch the NintendoWare converter tool    
    String convArgs;
    convArgs.Format("-g %s %s", suiSrcDir.AsCharPtr(), tmpDir.AsCharPtr());
    AppLauncher convAppLauncher;
    convAppLauncher.SetExecutable(this->nw4rToolPath);
    convAppLauncher.SetWorkingDirectory("proj:");
    convAppLauncher.SetArguments(convArgs);
    if (!convAppLauncher.LaunchWait())
    {
        this->logger.Error("Failed to launch '%s %s'!\n", this->nw4rToolPath.AsCharPtr(), convArgs.AsCharPtr());
        return false;
    }

    // finally, convert into single archive file using the RVL SDK archiver tool
    if (!ioServer->DirectoryExists(suiDstDir))
    {
        ioServer->CreateDirectory(suiDstDir);
    }
    if (ioServer->FileExists(dstArcFile))
    {
        ioServer->DeleteFile(dstArcFile);
    }

    String packArgs;
    packArgs.Format("-c . %s", dstArcFile.AsCharPtr());
    AppLauncher packAppLauncher;
    packAppLauncher.SetExecutable(this->arcToolPath);
    packAppLauncher.SetWorkingDirectory(tmpDir);
    packAppLauncher.SetArguments(packArgs);
    if (!packAppLauncher.LaunchWait())
    {
        this->logger.Error("Failed to launch '%s %s'!\n", this->arcToolPath.AsCharPtr(), packArgs.AsCharPtr());
        return false;
    }
    return true;
}

} // namespace Toolkit