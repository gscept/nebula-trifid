//------------------------------------------------------------------------------
//  animconverterapp.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "animconverterapp.h"

namespace Toolkit
{
using namespace ToolkitUtil;

//------------------------------------------------------------------------------
/**
*/
bool
AnimConverterApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        this->animConverter.SetForceFlag(this->args.GetBoolFlag("-force"));
        this->category = this->args.GetString("-cat", "");
        this->animFileName = this->args.GetString("-anim", "");
        this->animConverter.SetAnimDrivenMotionFlag(this->args.GetBoolFlag("-animdrivenmotion"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimConverterApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        this->animConverter.SetPlatform(this->platform);
        this->animConverter.SetSrcDir(this->projectInfo.GetAttr("AnimSrcDir"));
        this->animConverter.SetDstDir(this->projectInfo.GetAttr("AnimDstDir")); 
        if (this->projectInfo.HasAttr("AnimDrivenMotionEnabled"))
        {
            this->animConverter.SetAnimDrivenMotionFlag(this->projectInfo.GetAttr("AnimDrivenMotionEnabled").AsBool());
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimConverterApp::ShowHelp()
{
    n_printf("Nebula3 animation processing tool.\n"
             "(C) Radon Labs GmbH 2009.\n"
             "-help       -- display this help\n"
             "-platform   -- select platform (win32, xbox360, wii, ps3)\n"
             "-waitforkey -- wait for key when complete\n"
             "-force      -- force export (don't check time stamps)\n"
             "-cat        -- select specific category\n"
             "-anim       -- select specific texture (also needs -cat)\n");
}

//------------------------------------------------------------------------------
/**
*/
void
AnimConverterApp::Run()
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
    this->animConverter.Setup(this->logger);

    // perform texture conversion
    bool result = true;
    if (this->category.IsValid())
    {
        if (this->animFileName.IsValid())
        {
            result = this->animConverter.ProcessAnimation(this->category, this->animFileName);
        }
        else
        {
            result = this->animConverter.ProcessCategory(this->category);
        }
    }
    else
    {
        result = this->animConverter.ProcessAll();
    }
    this->animConverter.Discard();

    if (this->waitForKey)
    {
        n_printf("Press <Enter> to continue!\n");
        while (!IO::Console::Instance()->HasInput())
        {
            Timing::Sleep(0.01);
        }
    }
    
}

} // namespace Toolkit