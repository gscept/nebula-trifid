#pragma once
#ifndef TOOLS_SHADERCODEGENERATOR_H
#define TOOLS_SHADERCODEGENERATOR_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderCodeGenerator
  
    Takes an URI and a shader object and generates an .fx file.
    
    (C) 2007 Radon Labs GmbH
*/
#include "shader.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderCodeGenerator
{
public:
    /// constructor
    ShaderCodeGenerator();
    /// set to true if shader structure should be dumped to file header
    void SetDumpShaderStructure(bool b);
    /// get dump shader structure flag
    bool GetDumpShaderStructure() const;
    /// generate shader source for shader
    bool GenerateSourceCode(const IO::URI& projDirectory, const Ptr<Shader>& shd);
private:
    /// write the source code file header
    void WriteFileHeader(const Ptr<IO::TextWriter>& textWriter);
    /// write a dump of the shader structure
    void WriteShaderStructureDump(const Ptr<IO::TextWriter>& textWriter);
    /// write the shader input/output declarations
    void WriteInputOutputDeclarations(const Ptr<IO::TextWriter>& textWriter);
    /// write constant declarations
    void WriteConstantDeclarations(const Ptr<IO::TextWriter>& textWriter);
    /// write texture sampler declarations
    bool WriteSamplerDeclarations(const Ptr<IO::TextWriter>& textWriter);
    /// write all fragment functions
    void WriteFragmentFunctions(const Ptr<IO::TextWriter>& textWriter);
    /// write fragment code for a single fragment
    void WriteFragmentFunction(const Ptr<IO::TextWriter>& textWriter, const Util::String& funcName, const Util::Dictionary<Util::String, ShaderParam>& inputs, const Util::Dictionary<Util::String, ShaderParam>& outputs, const Util::String& code);
    /// write the vertex shader functions
    void WriteVertexShader(const Ptr<IO::TextWriter>& textWriter);
    /// write the vertex shader functions
    void WritePixelShader(const Ptr<IO::TextWriter>& textWriter);
    /// write fragment call input variables
    void WriteInputVariables(const Ptr<IO::TextWriter>& textWriter, const Ptr<ShaderNode>& shaderNode, ShaderSlot::SlotType slotType);
    /// write fragment call output variables
    void WriteOutputVariables(const Ptr<IO::TextWriter>& textWriter, const Ptr<ShaderNode>& shaderNode, ShaderSlot::SlotType slotType);
    /// write vertex shader fragment call
    void WriteVertexShaderFragmentCall(const Ptr<IO::TextWriter>& textWriter, const Ptr<ShaderNode>& shaderNode);
    /// write pixel shader fragment call
    void WritePixelShaderFragmentCall(const Ptr<IO::TextWriter>& textWriter, const Ptr<ShaderNode>& shaderNode);
    /// write vertex/pixel shader return values
    void WriteShaderReturnValues(const Ptr<IO::TextWriter>& textWriter, const Ptr<ShaderNode>& shaderNode, const Util::String& outStruct, const Util::String& outName);

    Ptr<Shader> shader;
    bool dumpShaderStructure;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderCodeGenerator::SetDumpShaderStructure(bool b)
{
    this->dumpShaderStructure = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderCodeGenerator::GetDumpShaderStructure() const
{
    return this->dumpShaderStructure;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif
