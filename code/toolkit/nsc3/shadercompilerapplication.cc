//------------------------------------------------------------------------------
//  shadercompilerapplication.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadercompilerapplication.h"
#include "system/win32/win32registry.h"

namespace Tools
{
using namespace System;
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderCompilerApplication::ShaderCompilerApplication()
{
    this->shaderCompiler = ShaderCompiler::Create();
}

//------------------------------------------------------------------------------
/**
*/
ShaderCompilerApplication::~ShaderCompilerApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderCompilerApplication::ParseCmdLineArgs()
{
    bool help = this->args.GetBool("-help");
    if (help)
    {
        n_printf("Nebula3 shader compiler.\n"
                 "(C) Radon Labs GmbH 2007.\n"
                 "Compiles Nebula3 shaders.\n\n"
                 "-help: display this help\n"
                 "-verbose: print lots of information\n");
        return false;
    }
    this->filePattern = this->args.GetString("-shd", "*.xml");
    this->shaderCompiler->SetVerbose(this->args.GetBool("-verbose"));
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderCompilerApplication::Run()
{
    // parse command line args
    if (!this->ParseCmdLineArgs())
    {
        return;
    }

    // get shaderlib base directory
    URI projDirectory;
    if (Win32Registry::Exists(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir"))
    {
        String str = "file:///";
        str.Append(Win32Registry::ReadString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir"));
        projDirectory = str;
    }
    else
    {
        projDirectory = "home:";
    }

    // load shader fragments and samplers
    this->shaderCompiler->SetProjectDirectory(projDirectory);
    if (!this->shaderCompiler->LoadFragments())
    {
        this->SetReturnCode(10);
        return;
    }
    if (!this->shaderCompiler->LoadSamplers())
    {
        this->SetReturnCode(10);
        return;
    }

    // load shader definitions
    if (!this->shaderCompiler->LoadShaders(this->filePattern))
    {
        this->SetReturnCode(10);
        return;
    }

    // compile shaders
    if (!this->shaderCompiler->Compile())
    {
        this->SetReturnCode(10);
        return;
    }
}

} // namespace Tools
