#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::SyncToolApp
    
    Application class for the sync tool. This will sync the local data
    with data from a build server and generally make sure that everything
    is uptodate on the client machine.
    
    In client-mode the tool will perform the following steps:
    
    - perform a CVS/SVN update on the source files
    - for each exported asset file, perform a status check:
        * REMOVED - the file has been deleted from the build
        * NEEDS_COPY - the file needs to be copied form the build server
        * NEEDS_EXPORT - the file must be locally exported        
    - delete all files with the REMOVED status, and copy all files with the
      NEEDS_COPY status
    - export all files with the NEEDS_EXPORT status
    
    (C) 2008 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"
#include "toolkitutil/rcswrapper.h"
#include "toolkitutil/assetregistry.h"
#include "toolkitutil/assetupdater.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class SyncToolApp : public ToolkitUtil::ToolkitApp
{
public:
    /// constructor
    SyncToolApp();
    /// run the application
    virtual void Run();
    
private:
    /// what to do
    enum OpFlag
    {
        OpUpdate = (1<<0),      // perform RCS update
        OpSync   = (1<<1),      // perform synchronization
        OpTouch  = (1<<2),      // perform touch on modified files in working copy
        OpExport = (1<<3),      // perform local export
        OpBuildRemoteRegistry = (1<<4), // only build remote (build server) registry
        OpBuildLocalRegistry = (1<<5),  // only build local registry
    };

    /// parse command line arguments
    virtual bool ParseCmdLineArgs();
    /// setup project info object
    virtual bool SetupProjectInfo();
    /// ask user to continue (only when waitForKey is true)
    virtual bool AskToContinue();
    /// print help text
    virtual void ShowHelp();
    /// run the batch exporter
    bool DoBatchExport();
    /// touch modified files in working copy
    bool TouchModifiedFiles();

    Util::String buildServerRoot;
    int opMask;
    ToolkitUtil::RCSWrapper rcsWrapper;
    ToolkitUtil::AssetRegistry remoteFiles;
    ToolkitUtil::AssetRegistry localFiles;
    ToolkitUtil::AssetRegistry diffFiles;
    ToolkitUtil::AssetUpdater assetUpdater;
    Util::String gfxBatchTool;
    Util::String gfxBatchArgs;
    Util::String gfxChar3BundleTool;
    Util::String gfxChar3BundleArgs;
    Util::String texBatchTool;
    Util::String texBatchArgs;
    Util::String srcDir;
    bool waitForKey;
};

}
//------------------------------------------------------------------------------
    