//------------------------------------------------------------------------------
//  synctoolapp.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "synctoolapp.h"
#include "toolkitutil/applauncher.h"
#include "timing/calendartime.h"

namespace Toolkit
{
using namespace Util;
using namespace ToolkitUtil;
using namespace IO;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
SyncToolApp::SyncToolApp() :
    opMask(0),
    waitForKey(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncToolApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        this->opMask = 0;
        this->opMask |= this->args.GetBoolFlag("-update") ? OpUpdate : 0;
        this->opMask |= this->args.GetBoolFlag("-sync")   ? OpSync : 0;
        this->opMask |= this->args.GetBoolFlag("-touch")  ? OpTouch : 0;
        this->opMask |= this->args.GetBoolFlag("-export") ? OpExport : 0;
        this->opMask |= this->args.GetBoolFlag("-buildremote") ? OpBuildRemoteRegistry : 0;
        this->opMask |= this->args.GetBoolFlag("-buildlocal") ? OpBuildLocalRegistry : 0;       
        if ((0 == this->opMask) || this->args.GetBoolFlag("-all"))
        {
            // no cmd line args specific: do everything
            this->opMask = OpUpdate | OpSync | OpTouch | OpExport;
        }
        this->waitForKey = this->args.GetBoolFlag("-waitforkey");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncToolApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        Array<String> updateTimeStampDirectories;
        updateTimeStampDirectories.Append("proj:");
        this->buildServerRoot = this->projectInfo.GetAttr("BuildServerRoot");
        this->srcDir = this->projectInfo.GetAttr("SrcDir");
        this->rcsWrapper.SetSVNToolLocation(this->projectInfo.GetPathAttr("SVNToolPath"));
        this->rcsWrapper.SetUpdateTimeStampDirectories(updateTimeStampDirectories);
        this->rcsWrapper.SetCommitTimeStampDirectories(this->projectInfo.GetAttr("AssetSourceDirs").Tokenize(";"));
        this->localFiles.SetRootPath("proj:");
        this->localFiles.SetRegistryFile(this->projectInfo.GetAttr("AssetRegistry"));
        this->localFiles.SetAssetDirectories(this->projectInfo.GetAttr("SyncDirs").Tokenize(";"));
        this->remoteFiles.SetRootPath("remote:");
        this->remoteFiles.SetRegistryFile(this->projectInfo.GetAttr("AssetRegistry"));
        this->remoteFiles.SetAssetDirectories(this->projectInfo.GetAttr("SyncDirs").Tokenize(";"));
        this->assetUpdater.SetLocalRootPath("proj:");
        this->assetUpdater.SetRemoteRootPath("remote:");
        this->gfxBatchTool = this->projectInfo.GetPathAttr("SyncGfxBatchTool");
        this->gfxBatchArgs = this->projectInfo.GetAttr("SyncGfxBatchArgs");
        this->gfxChar3BundleTool = this->projectInfo.GetPathAttr("SyncChar3BundleTool");
        this->gfxChar3BundleArgs = this->projectInfo.GetAttr("SyncChar3BundleArgs");
        this->texBatchTool = this->projectInfo.GetPathAttr("SyncTexBatchTool");
        this->texBatchArgs = this->projectInfo.GetAttr("SyncTexBatchArgs");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncToolApp::AskToContinue()
{
    // only in 'interactive' mode
    if (this->waitForKey)
    {
        // determine the build time
        URI uri = this->buildServerRoot + "\\" + this->projectInfo.GetAttr("AssetRegistry");
        IO::FileTime lastModification = IO::IoServer::Instance()->GetFileWriteTime(uri);
        Timing::CalendarTime date = Timing::CalendarTime::FileTimeToLocalTime(lastModification);

        // ask user if local build should be resumed or not
        n_printf("Last build: %s\n", Timing::CalendarTime::Format("{DAY}.{MONTH}.{YEAR} at {HOUR}:{MINUTE}",date).AsCharPtr());
        n_printf("Do you want to continue with Local Sync Build? [y/n]\n");
        while (!Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
        String answer = Console::Instance()->GetInput();
        answer.ToLower();
        return String("y") == answer;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
SyncToolApp::ShowHelp()
{
    n_printf("Nebula3 build sync tool.\n"
             "(C) 2008 Radon Labs GmbH\n"
             "-help   -- display this help\n"
             "-server -- run in server mode (updates build server asset registry)\n"
             "-all    -- perform all sync operations (default)\n"
             "-update -- perform CVS or SVN update operation\n"
             "-sync   -- perform asset sync operation\n"
             "-export -- perform asset export operation\n"
             "-buildremote -- re-build remote registry file only\n"
             "-buildlocal  -- re-build local registry file only\n"
             "-waitforkey  -- wait for key press after finished\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SyncToolApp::Run()
{
    Timing::Timer timer;
    timer.Start();

    // parse command line args
    if (!this->ParseCmdLineArgs())
    {
        goto done;
    }

    // setup the project info object
    if (!this->SetupProjectInfo())
    {
        goto done;
    }

    // ask user if local build should be resumed or not
    if (!this->AskToContinue())
    {
        goto done;
    }

    // setup the remote: assign
    AssignRegistry::Instance()->SetAssign(Assign("remote", this->buildServerRoot));

	// perform operations
    if (0 != (OpBuildRemoteRegistry & this->opMask))
    {
        // only re-build the registry file on the remote build server
        this->logger.Print("Build remote asset registry file...\n");
        if (!this->remoteFiles.UpdateLocal(this->logger))
        {
            this->SetReturnCode(10);
            goto done;
        }
    }
    else if (0 != (OpBuildLocalRegistry & this->opMask))
    {
        // only re-build the registry file on the local machine
        this->logger.Print("Build local asset registry file...\n");
        if (!this->localFiles.UpdateLocal(this->logger))
        {
            this->SetReturnCode(10);
            goto done;
        }
    }
    else
    {
        // perform Update operation
        if (0 != (OpUpdate & this->opMask))
        {
            this->logger.Print("CVS/SVN Update...\n");
            if (!this->rcsWrapper.Update(this->logger))
            {
                this->SetReturnCode(10);
                goto done;
            }
        }

        // perform Sync operation
        if (0 != (OpSync & this->opMask))
        {
            // update local asset registry
            if (!this->localFiles.UpdateLocal(this->logger))
            {
                this->SetReturnCode(10);
                goto done;
            }

            // load remote asset registry
            if (!this->remoteFiles.UpdateRemote(this->logger))
            {
                this->SetReturnCode(10);
                goto done;
            }

            // create a difference set from the remote and local registry
            if (!this->diffFiles.BuildDifference(this->logger, this->localFiles, this->remoteFiles))
            {
                this->SetReturnCode(10);
                goto done;
            }

            // save back local files repository (since time stamps have been updated)
            this->localFiles.SaveRegistry(this->logger);

            // realize the difference set actions
            if (!this->assetUpdater.Update(this->logger, this->diffFiles))
            {
                this->SetReturnCode(10);
                goto done;
            }
        }

        // perform Touch operation
        if (0 != (OpTouch & this->opMask))
        {
            this->logger.Print("Scan working copy for modified files and touch them...\n");
        
            if(!this->TouchModifiedFiles())
            {
                this->SetReturnCode(10);
                goto done;
            }
        }

        // perform Export operation
        if (0 != (OpExport & this->opMask))
        {
            // finally run the batch exporters
            if (!this->DoBatchExport())
            {
                this->SetReturnCode(10);
                goto done;
            }
        }
    }
    timer.Stop();
    int minutes = int(timer.GetTime() / 60.0);
    int seconds = int(timer.GetTime());
    if (minutes == 0)
    {
        this->logger.Print("SYNC DONE (%d seconds)\n", seconds);
    }
    else
    {
        this->logger.Print("SYNC DONE (%d minutes)\n", minutes);
    }

done:
    if (this->waitForKey)
    {
        n_printf("Press <Enter> to continue!\n");
        while (!Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncToolApp::DoBatchExport()
{
    this->logger.Print("> Batch exporting...\n");

    IoServer* ioServer = IoServer::Instance();

    // run graphics batch exporter
    if (this->gfxBatchTool.IsValid())
    {
        if (!ioServer->FileExists(this->gfxBatchTool))
        {
            this->logger.Error("Graphics batch tool not found: '%s'!\n", this->gfxBatchTool.AsCharPtr());
            return false;
        }

        AppLauncher appLauncher;
        appLauncher.SetExecutable(this->gfxBatchTool);
        appLauncher.SetWorkingDirectory("proj:");
        appLauncher.SetArguments(this->gfxBatchArgs);
        if (!appLauncher.LaunchWait())
        {
            this->logger.Error("Failed to launch '%s'!\n", this->gfxBatchTool.AsCharPtr());
            return false;
        }
    }
    // run char3 bundler in an own process
    if (this->gfxChar3BundleTool.IsValid())
    {
        if (!ioServer->FileExists(this->gfxChar3BundleTool))
        {
            this->logger.Error("Char3 bundle tool not found: '%s'!\n", this->gfxChar3BundleTool.AsCharPtr());
            return false;
        }

        AppLauncher appLauncher;
        appLauncher.SetExecutable(this->gfxChar3BundleTool);
        appLauncher.SetWorkingDirectory("proj:");
        appLauncher.SetArguments(this->gfxChar3BundleArgs);
        if (!appLauncher.LaunchWait())
        {
            this->logger.Error("Failed to launch '%s'!\n", this->gfxChar3BundleTool.AsCharPtr());
            return false;
        }
    }

    // run texture batch exporter
    if (this->texBatchTool.IsValid())
    {
        if (!ioServer->FileExists(this->texBatchTool))
        {
            this->logger.Error("Texture batch tool not found: '%s'!\n", this->texBatchTool.AsCharPtr());
            return false;
        }

        AppLauncher appLauncher;
        appLauncher.SetExecutable(this->texBatchTool);
        appLauncher.SetWorkingDirectory("proj:");
        appLauncher.SetArguments(this->texBatchArgs);
        if (!appLauncher.LaunchWait())
        {
            this->logger.Error("Failed to launch '%s'!\n", this->texBatchTool.AsCharPtr());
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncToolApp::TouchModifiedFiles()
{
    Array<String> files;
    if(this->rcsWrapper.GetModifiedFiles(this->logger, this->srcDir, files))
    {   
        IoServer* ioServer = IoServer::Instance();
        FileTime time = CalendarTime::LocalTimeToFileTime(CalendarTime::GetLocalTime());

        SizeT numFiles = files.Size(); 
        this->logger.Print("Found %d modified files!\n", numFiles);

        IndexT idx;
        for(idx = 0; idx < numFiles; idx++)
        {
            URI fileUri(this->srcDir + "/" + files[idx]);
            this->logger.Print("Touching: %s\n", fileUri.GetHostAndLocalPath().AsCharPtr());
            
            bool readOnly = ioServer->IsReadOnly(fileUri);
            if(readOnly)
            {
                ioServer->SetReadOnly(fileUri, false);
                ioServer->SetFileWriteTime(fileUri, time);
                ioServer->SetReadOnly(fileUri, true);
            }
            else
            {
                ioServer->SetFileWriteTime(fileUri, time);
            }
        }
        return true;
    }
    return false;
}

} // namespace Toolkit
