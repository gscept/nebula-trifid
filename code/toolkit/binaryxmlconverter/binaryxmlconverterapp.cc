//------------------------------------------------------------------------------
//  binaryxmlconverterapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "binaryxmlconverterapp.h"

namespace Toolkit
{

//------------------------------------------------------------------------------
/**
*/
bool
BinaryXmlConverterApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        this->xmlConverter.SetSrcDir(this->args.GetString("-srcdir"));
        this->xmlConverter.SetDstDir(this->args.GetString("-dstdir"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
BinaryXmlConverterApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        this->xmlConverter.SetPlatform(this->platform);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
BinaryXmlConverterApp::ShowHelp()
{
    n_printf("Nebula3 XML binary converter.\n"
             "(C) Radon Labs GmbH 2009\n"
             "-help     -- display this help\n"
             "-platform -- select platform (win32, xbox360, ps3, wii)\n"
             "-srcdir   -- source directory (recursively converts all XML files in dir)\n"
             "-dstdir   -- destination directory\n");
}

//------------------------------------------------------------------------------
/**
*/
void
BinaryXmlConverterApp::Run()
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

    // perform the conversion
    if (!this->xmlConverter.Convert(this->logger))
    {
        this->SetReturnCode(10);
        return;
    }
}

} // namespace Toolkit

