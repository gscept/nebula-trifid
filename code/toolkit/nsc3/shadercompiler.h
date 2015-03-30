#pragma once
#ifndef TOOLS_SHADERCOMPILER_H
#define TOOLS_SHADERCOMPILER_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderCompiler
    
    Implements the Nebula3 shader compiler.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/uri.h"
#include "shaderfragmentmanager.h"
#include "shadersamplermanager.h"

//------------------------------------------------------------------------------
namespace Tools
{
class Shader;

class ShaderCompiler : public Core::RefCounted
{
    __DeclareClass(ShaderCompiler);
public:
    /// constructor
    ShaderCompiler();
    /// destructor
    virtual ~ShaderCompiler();
    /// set the project directory
    void SetProjectDirectory(const IO::URI& uri);
    /// get the project directory
    const IO::URI& GetProjectDirectory() const;
    /// set verbose flag (prints lots of information)
    void SetVerbose(bool b);
    /// get verbose flag
    bool IsVerbose() const;
    /// load fragment definitions
    bool LoadFragments();
    /// load sampler definitions
    bool LoadSamplers();
    /// load shader definition files matching pattern
    bool LoadShaders(const Util::String& pattern);
    /// compile the loaded shader definitions
    bool Compile();

private:
    IO::URI projDirectory;
    Ptr<ShaderFragmentManager> fragmentManager;
    Ptr<ShaderSamplerManager> samplerManager;
    Util::Dictionary<Util::String, Ptr<Shader>> shaders;
    bool verbose;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderCompiler::SetVerbose(bool b)
{
    this->verbose = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderCompiler::IsVerbose() const
{
    return this->verbose;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderCompiler::SetProjectDirectory(const IO::URI& dir)
{
    this->projDirectory = dir;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShaderCompiler::GetProjectDirectory() const
{
    return this->projDirectory;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif
    