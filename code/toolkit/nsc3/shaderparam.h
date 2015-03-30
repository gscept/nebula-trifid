#ifndef TOOLS_SHADERPARAM_H
#define TOOLS_SHADERPARAM_H
//------------------------------------------------------------------------------
/**
    @class Tools::ShaderParam
  
    Describes a shader parameter for the shader compiler.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/types.h"
#include "io/xmlreader.h"

//------------------------------------------------------------------------------
namespace Tools
{
class ShaderParam
{
public:
    /// parse the shader parameter from an XML stream
    bool Parse(const Ptr<IO::XmlReader>& xmlReader);
    /// set the parameter name
    void SetName(const Util::String& s);
    /// get the parameter name
    const Util::String& GetName() const;
    /// set the parameter type (HLSL compatible)
    void SetType(const Util::String& s);
    /// get the parameter type
    const Util::String& GetType() const;
    /// set annotation string
    void SetAnnotation(const Util::String& s);
    /// get annotation string
    const Util::String& GetAnnotation() const;
    /// set the optional parameter default value
    void SetDefaultValue(const Util::String& s);
    /// get the optional default value
    const Util::String& GetDefaultValue() const;
    /// set the optional default binding
    void SetDefaultBinding(const Util::String& s);
    /// get the optional default binding
    const Util::String& GetDefaultBinding() const;

private:
    Util::String name;
    Util::String type;
    Util::String annotation;
    Util::String defaultValue;
    Util::String defaultBinding;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderParam::SetName(const Util::String& s)
{
    this->name = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderParam::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderParam::SetType(const Util::String& s)
{
    this->type = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderParam::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderParam::SetAnnotation(const Util::String& s)
{
    this->annotation = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderParam::GetAnnotation() const
{
    return this->annotation;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderParam::SetDefaultValue(const Util::String& s)
{
    this->defaultValue = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderParam::GetDefaultValue() const
{
    return this->defaultValue;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderParam::SetDefaultBinding(const Util::String& s)
{
    this->defaultBinding = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
ShaderParam::GetDefaultBinding() const
{
    return this->defaultBinding;
}

} // namespace Tools
//------------------------------------------------------------------------------
#endif
