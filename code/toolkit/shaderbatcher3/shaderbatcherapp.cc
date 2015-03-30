//------------------------------------------------------------------------------
//  shaderbatcherapp.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderbatcherapp.h"
#include "timing/time.h"

namespace Toolkit
{
using namespace ToolkitUtil;

//------------------------------------------------------------------------------
/**
*/
bool
ShaderBatcherApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        this->shaderCompiler.SetForceFlag(this->args.GetBoolFlag("-force"));
        this->shaderCompiler.SetDebugFlag(this->args.GetBoolFlag("-debug"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderBatcherApp::SetupProjectInfo()
{
    if (ToolkitApp::SetupProjectInfo())
    {
        this->shaderCompiler.SetPlatform(this->platform);
//        if (this->projectInfo.HasAttr("ShaderTool"))
//        {
//            this->shaderCompiler.SetToolPath(this->projectInfo.GetPathAttr("ShaderTool"));
//        }
        if (this->projectInfo.HasAttr("ShaderToolParams"))
        {
            this->shaderCompiler.SetAdditionalParams(this->projectInfo.GetAttr("ShaderToolParams"));
        }
        this->shaderCompiler.SetShaderSrcDir(this->projectInfo.GetAttr("ShaderSrcDir"));
        this->shaderCompiler.SetShaderDstDir(this->projectInfo.GetAttr("ShaderDstDir"));
        this->shaderCompiler.SetFrameShaderSrcDir(this->projectInfo.GetAttr("FrameShaderSrcDir"));
        this->shaderCompiler.SetFrameShaderDstDir(this->projectInfo.GetAttr("FrameShaderDstDir"));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBatcherApp::ShowHelp()
{
    n_printf("Nebula3 shader batch compiler.\n"
             "(C) Radon Labs GmbH 2008.\n"
             "-help       -- display this help\n"
             "-platform   -- select platform (win32, xbox360, wii, ps3)\n"
             "-waitforkey -- wait for key when complete\n"
             "-force      -- force recompile\n"
             "-debug      -- compile with debugging information\n"
             "-nebula2    -- compile in legacy N2 mode\n");
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBatcherApp::Run()
{
    bool success = true;
    // parse command line args
    if (success && !this->ParseCmdLineArgs())
    {
        success = false;
    }

    // setup the project info object
    if (success && !this->SetupProjectInfo())
    {
        success = false;
    }

    // call the shader compiler tool
    if (success && !this->shaderCompiler.CompileShaders())
    {
        success = false;
        this->SetReturnCode(10);
    }
    if (success && !this->shaderCompiler.CompileFrameShaders())
    {
        success = false;
        this->SetReturnCode(10);
    }

    // wait for user input
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
