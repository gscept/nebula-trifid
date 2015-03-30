#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::SUIConverterApp

    Convert platform specific SUI resource files.

    (C) 2009 Radon Labs GmbH
*/
#include "toolkitutil/toolkitapp.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class SUIConverterApp : public ToolkitUtil::ToolkitApp
{
public:
    /// run the application
    virtual void Run();

private:
    /// setup project info object
    virtual bool SetupProjectInfo();
    /// print help text
    virtual void ShowHelp();
    /// convert all SUI resources
    bool ConvertAll();
    /// convert a single SUI resource
    bool ConvertSingle(const Util::String& itemName);
    /// convert a single SUI resource for the Wii platform
    bool ConvertSingleWii(const Util::String& itemName);

    Util::String srcDir;
    Util::String dstDir;
    Util::String nw4rToolPath;
    Util::String arcToolPath;
};

} // namespace Toolkit
//------------------------------------------------------------------------------
