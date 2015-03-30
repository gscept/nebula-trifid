#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShaderParam
  
    A shader parameter of a MayaShader object.
  
    TODO: document the envelope curve default value array (see
    core_rlmaterialeditor.mel)

    (C) 2009 Radon Labs GmbH
*/    
#include "core/types.h"
#include "io/xmlreader.h"
#include "util/stringatom.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShaderParam 
{
public:
    /// shader param data types
    enum Type
    {
        Int,
        Bool,
        Float,
        Texture,
        Vector,
        Color,
        Envelope,
        ColorEnvelope,

        NumTypes,
        InvalidType,
    };

    enum ValueKind
    {
        Default,
        Min,
        Max,

        NumValueKinds,
    };

    /// constructor
    MayaShaderParam();

    /// setup the object from an XML stream
    void SetupFromXml(const Ptr<IO::XmlReader>& xmlReader);

    /// get the shader parameter id
    const Util::StringAtom& GetId() const;
    /// get shader parameter type
    Type GetType() const;
    /// get shader parameter UI label
    const Util::StringAtom& GetLabel() const;
    /// get shader param semantic
    const Util::StringAtom& GetSemantic() const;
    /// get a short description of the parameter
    const Util::String& GetDescription() const;
    /// get GUI flag (is visible in material editor)
    bool IsGuiParam() const;
    /// get export flag (true if this parameter is a Nebula3 shader parameter, 0 if special case)
    bool IsExportParam() const;

    /// get string for default value
    const Util::String& GetDefaultValue() const;
    /// get string for min value
    const Util::String& GetMinValue() const;
    /// get string for max value
    const Util::String& GetMaxValue() const;

    /// convert string to shader param type
    static Type StringToType(const Util::String& str);
    /// convert type to string
    static Util::String TypeToString(Type t);

private:
    Type type;
    Util::StringAtom id;
    Util::StringAtom label;
    Util::StringAtom semantic;
    Util::String description;
    Util::String defaultValue;
    Util::String minValue;
    Util::String maxValue;
    bool isGui;
    bool isExport;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShaderParam::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline MayaShaderParam::Type
MayaShaderParam::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShaderParam::GetLabel() const
{
    return this->label;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShaderParam::GetSemantic() const
{
    return this->semantic;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaShaderParam::GetDescription() const
{
    return this->description;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShaderParam::IsGuiParam() const
{
    return this->isGui;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShaderParam::IsExportParam() const
{
    return this->isExport;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaShaderParam::GetDefaultValue() const
{
    return this->defaultValue;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaShaderParam::GetMinValue() const
{
    return this->minValue;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaShaderParam::GetMaxValue() const
{
    return this->maxValue;
}

} // namespace Maya
//------------------------------------------------------------------------------
