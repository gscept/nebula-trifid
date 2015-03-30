#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::ResourceDictionaryApp
    
    Application wrapper for the ResourceDictionaryBuilder class.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013 Individual contributors, see AUTHORS file
*/
#include "toolkitutil/toolkitapp.h"
#include "toolkitutil/resourcedictionarybuilder.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class ResourceDictionaryApp : public ToolkitUtil::ToolkitApp
{
public:
    /// run the application
    virtual void Run();
    /// shutdown
    virtual void Close();

private:
    /// parse command line arguments
    virtual bool ParseCmdLineArgs();
    /// setup project info object
    virtual bool SetupProjectInfo();
    /// print help text
    virtual void ShowHelp();

    ToolkitUtil::ResourceDictionaryBuilder resDictBuilder;
};

} // namespace Toolkit 
//------------------------------------------------------------------------------
