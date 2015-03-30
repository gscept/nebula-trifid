#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShader
    
    A material shader description as defined in the n3shaders.xml file
    and managed by the MayaShaderRegistry singleton.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "io/xmlreader.h"
#include "util/stringatom.h"
#include "coregraphics/vertexcomponent.h"
#include "mayashaders/mayashaderparam.h"
#include "mayashaders/mayashaderfilter.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShader
{
public:
    /// constructor
    MayaShader();
    /// destructor
    ~MayaShader();

    /// setup the shader from an XML node
    void SetupFromXml(const Ptr<IO::XmlReader>& xmlReader);
    
    /// get the id of the shader
    const Util::StringAtom& GetId() const;
    /// get the display name of the shader
    const Util::StringAtom& GetDisplayName() const;
    /// get name of Nebula shader
    const Util::StringAtom& GetNebulaShaderName() const;
    /// get the Nebula node class string
    const Util::StringAtom& GetNodeClass() const;    
    /// get description of the shader
    const Util::String& GetDescription() const;
    /// get shader filter
    MayaShaderFilter::Code GetShaderFilter() const;
    /// return true if this shader is obsolete
    bool IsObsolete() const;

    /// get the vertex components of the shader
    const Util::Array<CoreGraphics::VertexComponent>& GetVertexComponents() const;
    /// get uv-set mapping for a vertex components
    const Util::StringAtom& GetUvSetMapping(IndexT texCoordIndex) const;
    
    /// return true if a MayaShaderParam exists by id
    bool HasParam(const Util::StringAtom& id) const;
    /// get number of shader params
    SizeT GetNumParams() const;
    /// get shader param by index
    const MayaShaderParam& GetParamByIndex(IndexT i) const;
    /// get shader param by id
    const MayaShaderParam& GetParamById(const Util::StringAtom& id) const;
    /// get shader param index by id (return InvalidIndex if not found)
    IndexT GetParamIndexById(const Util::StringAtom& id) const;

private:
    Util::StringAtom id;
    Util::StringAtom displayName;
    Util::StringAtom shaderName;
    Util::StringAtom nodeClass;
    Util::String description;
    MayaShaderFilter::Code shaderFilter;
    bool isObsolete;
    Util::Array<CoreGraphics::VertexComponent> vertexComponents;
    Util::FixedArray<Util::StringAtom> uvSetMapping;
    Util::Array<MayaShaderParam> params;    
    Util::Dictionary<Util::StringAtom, IndexT> paramIndexMap;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShader::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShader::GetDisplayName() const
{
    return this->displayName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShader::GetNebulaShaderName() const
{
    return this->shaderName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShader::GetNodeClass() const
{
    return this->nodeClass;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaShader::GetDescription() const
{
    return this->description;
}

//------------------------------------------------------------------------------
/**
*/
inline MayaShaderFilter::Code
MayaShader::GetShaderFilter() const
{
    return this->shaderFilter;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShader::IsObsolete() const
{
    return this->isObsolete;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<CoreGraphics::VertexComponent>&
MayaShader::GetVertexComponents() const
{
    return this->vertexComponents;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
MayaShader::GetUvSetMapping(IndexT texCoordIndex) const
{
    return this->uvSetMapping[texCoordIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaShader::HasParam(const Util::StringAtom& id) const
{
    return this->paramIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
MayaShader::GetNumParams() const
{
    return this->params.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const MayaShaderParam&
MayaShader::GetParamByIndex(IndexT i) const
{
    return this->params[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const MayaShaderParam&
MayaShader::GetParamById(const Util::StringAtom& id) const
{
    return this->params[this->paramIndexMap[id]];
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
MayaShader::GetParamIndexById(const Util::StringAtom& id) const
{
    if (this->paramIndexMap.Contains(id))
    {
        return this->paramIndexMap[id];
    }
    else
    {
        return InvalidIndex;
    }
}

} // namespace Maya
//------------------------------------------------------------------------------
    