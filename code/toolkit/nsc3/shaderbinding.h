#pragma once
#ifndef TOOLS_SHADERBINDING_H
#define TOOLS_SHADERBINDING_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderBinding
    
    A shader binding defines a binding between a node output and another
    node's input parameter, or from/to a scoped parameter. The following
    scopes are valid:
    
    Vertex:     an input vertex component
    Pixel:      an vertex shader output/pixel shader input component
    Result:     a pixel shader result
    Global:     a global shader parameter
    Shared:     a shared global shader parameter
    Sampler:    a sampler parameter
    Fragment:   a fragment's in or out parameter
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "io/xmlreader.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderBinding : public Core::RefCounted
{
    __DeclareClass(ShaderBinding);
public:
    /// constructor
    ShaderBinding();
    /// destructor
    ~ShaderBinding();
    
    /// parse binding from XML stream
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);
    
    /// get the shader node parameter name
    const Util::String& GetParam() const;
    /// get binding node (extracted from binding string)
    const Util::String& GetBindNode() const;
    /// get binding param (extracted from binding string)
    const Util::String& GetBindParam() const;

private:
    Util::String param;
    Util::String bindNode;
    Util::String bindParam;
};

} // namespace Tools
//------------------------------------------------------------------------------
#endif
