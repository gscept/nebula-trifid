//------------------------------------------------------------------------------
//  resourcedictionaryapp.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resourcedictionaryapp.h"

namespace Toolkit
{
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
bool
ResourceDictionaryApp::ParseCmdLineArgs()
{
    // hmm, nothing todo (yet)
    return ToolkitApp::ParseCmdLineArgs();
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceDictionaryApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        this->resDictBuilder.SetPlatform(this->platform);
        // @todo: change to upper folder here + in resDictReader (now set to texture)
        this->resDictBuilder.SetTextureDirectory(this->projectInfo.GetAttr("TextureDstDir"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceDictionaryApp::ShowHelp()
{
    n_printf("Nebula3 resource dictionary builder.\n"
             "(C) Radon Labs GmbH 2008.\n"
             "-help       -- display this help\n"
             "-platform   -- select platform (win32, xbox360, wii, ps3)\n"
             "-waitforkey -- wait for key when complete\n");
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceDictionaryApp::Run()
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

    // setup the texture converter
    if (!this->resDictBuilder.BuildDictionary())
    {
        n_printf("ERROR: failed to build dictionary file!\n");
        this->SetReturnCode(10);
        return;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceDictionaryApp::Close()
{
    this->resDictBuilder.Unload();
    ToolkitUtil::ToolkitApp::Close();
}
} // namespace Toolkit
