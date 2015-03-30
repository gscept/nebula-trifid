#pragma once
#ifndef TOOLS_SHADERFRAGMENT_H
#define TOOLS_SHADERFRAGMENT_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderFragment
  
    Describes a shader fragment. Part of the Nebula3 shader compiler.
    
    (C) 2007 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "util/dictionary.h"
#include "util/string.h"
#include "io/uri.h"
#include "io/xmlreader.h"
#include "shaderparam.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderFragment : public Core::RefCounted
{
    __DeclareClass(ShaderFragment);
public:
    /// constructor
    ShaderFragment();
    /// destructor
    virtual ~ShaderFragment();
    
    /// set the file URI this fragment has been defined in
    void SetFileURI(const IO::URI& uri);
    /// get the file URI this fragment has been defined in
    const IO::URI& GetFileURI() const;
    /// parse the fragment from an open XmlReader
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);
    /// duplicate the fragment object with a new name
    Ptr<ShaderFragment> Duplicate(const Util::String& newName) const;
    
    /// get the fragment's name
    const Util::String& GetName() const;
    /// get the fragment's category
    const Util::String& GetCategory() const;
    /// return whether this fragment is exclusive (may only show up once in a shader)
    bool IsExclusive() const;
    /// return the description string
    const Util::String& GetDesc() const;

    /// return true if this fragment has a vertex shader
    bool HasVertexShader() const;
    /// return true if this fragment has a pixel shader
    bool HasPixelShader() const;
    /// return true if this fragment has render state
    bool HasRenderStates() const;

    /// get vertex shader input parameters
    const Util::Dictionary<Util::String, ShaderParam>& GetVertexShaderInputs() const;
    /// get vertex shader output parameters
    const Util::Dictionary<Util::String, ShaderParam>& GetVertexShaderOutputs() const;
    /// get vertex shader code
    const Util::String& GetVertexShaderCode() const;
    /// return a function name for the vertex shader function
    Util::String GetVertexShaderFunctionName() const;

    /// get pixel shader input parameters
    const Util::Dictionary<Util::String, ShaderParam>& GetPixelShaderInputs() const;
    /// get pixel shader output parameters
    const Util::Dictionary<Util::String, ShaderParam>& GetPixelShaderOutputs() const;
    /// get pixel shader code
    const Util::String& GetPixelShaderCode() const;
    /// get a function name for the pixel shader function
    Util::String GetPixelShaderFunctionName() const;

    /// get state name/value pairs
    const Util::Dictionary<Util::String, Util::String>& GetRenderStates() const;

private:
    /// parse a vertex or pixel shader fragment definition
    void ParseShaderFragment(const Ptr<IO::XmlReader>& xmlReader, Util::Dictionary<Util::String,ShaderParam>& outShaderInputs, Util::Dictionary<Util::String,ShaderParam>& outShaderOutputs, Util::String& outShaderCode);
    /// parse a render state definition block
    void ParseRenderStates(const Ptr<IO::XmlReader>& xmlReader);

    IO::URI fileUri;
    Util::String name;
    Util::String category;
    Util::String desc;
    bool isExclusive;
    bool hasVertexShader;
    bool hasPixelShader;
    bool hasRenderStates;
    Util::Dictionary<Util::String, ShaderParam> vertexShaderInputs;
    Util::Dictionary<Util::String, ShaderParam> vertexShaderOutputs;
    Util::String vertexShaderCode;
    Util::Dictionary<Util::String, ShaderParam> pixelShaderInputs;
    Util::Dictionary<Util::String, ShaderParam> pixelShaderOutputs;
    Util::String pixelShaderCode;
    Util::Dictionary<Util::String, Util::String> renderStates;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderFragment::SetFileURI(const IO::URI& uri)
{
    this->fileUri = uri;
}

//------------------------------------------------------------------------------
/**
*/
inline const IO::URI&
ShaderFragment::GetFileURI() const
{
    return this->fileUri;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragment::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragment::GetCategory() const
{
    return this->category;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragment::GetDesc() const
{
    return this->desc;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragment::IsExclusive() const
{
    return this->isExclusive;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragment::HasVertexShader() const
{
    return this->hasVertexShader;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragment::HasPixelShader() const
{
    return this->hasPixelShader;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderFragment::HasRenderStates() const
{
    return this->hasRenderStates;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, ShaderParam>&
ShaderFragment::GetVertexShaderInputs() const
{
    return this->vertexShaderInputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, ShaderParam>&
ShaderFragment::GetVertexShaderOutputs() const
{
    return this->vertexShaderOutputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragment::GetVertexShaderCode() const
{
    return this->vertexShaderCode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, ShaderParam>&
ShaderFragment::GetPixelShaderInputs() const
{
    return this->pixelShaderInputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, ShaderParam>&
ShaderFragment::GetPixelShaderOutputs() const
{
    return this->pixelShaderOutputs;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderFragment::GetPixelShaderCode() const
{
    return this->pixelShaderCode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Util::String, Util::String>&
ShaderFragment::GetRenderStates() const
{
    return this->renderStates;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif
