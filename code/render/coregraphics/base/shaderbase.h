#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ShaderBase
  
    A shader object groups render states required to render a piece of
    geometry. Shader objects are not used for rendering directly,
    instead ShaderInstances are created from a shader.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "resources/resource.h"
#include "vertexcomponentbase.h"
#include "../shaderidentifier.h"

namespace CoreGraphics
{
class ShaderInstance;
};

//------------------------------------------------------------------------------
namespace Base
{
class ShaderBase : public Resources::Resource
{
    __DeclareClass(ShaderBase);
public:
    /// constructor
    ShaderBase();
    /// destructor
    virtual ~ShaderBase();
    /// create a shader instance from this shader
    Ptr<CoreGraphics::ShaderInstance> CreateShaderInstance();
    /// discard a shader instance
    void DiscardShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& inst);
    /// get all instances
    const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& GetAllShaderInstances() const;
	/// get shader name
	const Util::StringAtom GetShaderName() const;
    /// get unique shader identifier code
    const CoreGraphics::ShaderIdentifier::Code& GetShaderCode() const;

protected:
    CoreGraphics::ShaderIdentifier::Code shaderIdentifierCode;
    Util::Array<Ptr<CoreGraphics::ShaderInstance>> shaderInstances;
	Util::StringAtom shaderName;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<CoreGraphics::ShaderInstance> >&
ShaderBase::GetAllShaderInstances() const
{
    return this->shaderInstances;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom 
ShaderBase::GetShaderName() const
{
	return this->shaderName;
}

//------------------------------------------------------------------------------
/**
*/
inline const CoreGraphics::ShaderIdentifier::Code&
ShaderBase::GetShaderCode() const
{
    return this->shaderIdentifierCode;
}

} // namespace Base
//------------------------------------------------------------------------------

