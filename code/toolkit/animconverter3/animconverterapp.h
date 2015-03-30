#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::AnimConverterApp
    
    N3 anim file conversion and processing tool.
    
    (C) 2009 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"
#include "toolkitutil/animutil/animconverter.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class AnimConverterApp : public ToolkitUtil::ToolkitApp
{
public:
    /// run the application
    virtual void Run();

private:
    /// parse command line arguments
    virtual bool ParseCmdLineArgs();
    /// setup project info object
    virtual bool SetupProjectInfo();
    /// print help text
    virtual void ShowHelp();

    ToolkitUtil::AnimConverter animConverter;
    Util::String category;
    Util::String animFileName;
};

} // namespace Toolkit
//------------------------------------------------------------------------------

    
    