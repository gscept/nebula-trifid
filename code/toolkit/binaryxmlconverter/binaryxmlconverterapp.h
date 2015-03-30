#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::BinaryXmlConverterApp
    
    App wrapper to convert XML file into a generic binary format.
    
    (C) 2009 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"
#include "toolkitutil/binaryxmlconverter.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class BinaryXmlConverterApp : public ToolkitUtil::ToolkitApp
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

    ToolkitUtil::BinaryXmlConverter xmlConverter;        
};

} // namespace Toolkit
//------------------------------------------------------------------------------
    