#pragma once
#ifndef TOOLS_SHADERCOMPILERAPPLICATION_H
#define TOOLS_SHADERCOMPILERAPPLICATION_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderCompilerApplication
    
    Application class for the Nebula3 shader compiler. This is just a wrapper
    around the class ShaderCompiler.

    (C) 2007 Radon Labs GmbH
*/
#include "app/consoleapplication.h"
#include "shadercompiler.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderCompilerApplication : public App::ConsoleApplication
{
public:
    /// constructor
    ShaderCompilerApplication();
    /// destructor
    ~ShaderCompilerApplication();
    /// run the application
    virtual void Run();

private:
    /// parse command line arguments
    bool ParseCmdLineArgs();
    
    Ptr<ShaderCompiler> shaderCompiler;
    Util::String filePattern;
};

} // namespace Tools
//------------------------------------------------------------------------------
#endif