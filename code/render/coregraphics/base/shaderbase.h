#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ShaderBase
  
    A shader object groups render states required to render a piece of
    geometry. Shader objects can be used as a shader object, which can apply
    a complete shader pipeline.

    A shader also contains a list of variables, which can be set, thus changing
    the global state of the shader.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "resources/resource.h"
#include "vertexcomponentbase.h"
#include "coregraphics/shaderidentifier.h"
#include "coregraphics/shaderfeature.h"
#include "coregraphics/shadervariable.h"
#include "../shadervariation.h"

namespace CoreGraphics
{
class ShaderVariation;
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

    /// unload shader
    void Unload();
    /// create a shader instance from this shader
    Ptr<CoreGraphics::ShaderInstance> CreateShaderInstance();
    /// discard a shader instance
    void DiscardShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& inst);
    /// get all instances
    const Util::Array<Ptr<CoreGraphics::ShaderInstance>>& GetAllShaderInstances() const;
	/// get shader name
	const Util::StringAtom GetShaderName() const;
    /// get unique shader identifier code
    const CoreGraphics::ShaderIdentifier::Code& GetCode() const;

    /// return true if the shader instance has a variable by name
    bool HasVariableByName(const Base::ShaderVariableBase::Name& n) const;
    /// get number of variables
    SizeT GetNumVariables() const;
    /// get a variable by index
    const Ptr<CoreGraphics::ShaderVariable>& GetVariableByIndex(IndexT i) const;
    /// get a variable by name
    const Ptr<CoreGraphics::ShaderVariable>& GetVariableByName(const Base::ShaderVariableBase::Name& n) const;

    /// return true if variation exists by matching feature mask
    bool HasVariation(CoreGraphics::ShaderFeature::Mask featureMask) const;
    /// get number of variations in the shader
    SizeT GetNumVariations() const;
    /// get shader variation by index
    const Ptr<CoreGraphics::ShaderVariation>& GetVariationByIndex(IndexT i) const;
    /// get shader variation by feature mask
    const Ptr<CoreGraphics::ShaderVariation>& GetVariationByFeatureMask(CoreGraphics::ShaderFeature::Mask featureMask) const;
    /// select active variation by feature mask, return true if active variation has been changed
    bool SelectActiveVariation(CoreGraphics::ShaderFeature::Mask featureMask);
    /// get the currently active variation
    const Ptr<CoreGraphics::ShaderVariation>& GetActiveVariation() const;

    /// begin updating shader state
    void BeginUpdate();
    /// end updating shader state
    void EndUpdate();
    /// apply currently selected variation
    void Apply();
    /// commit pending changes to the shader (variables)
    void Commit();

protected:
    CoreGraphics::ShaderIdentifier::Code shaderIdentifierCode;
    Util::Array<Ptr<CoreGraphics::ShaderInstance>> shaderInstances;
    Ptr<CoreGraphics::ShaderVariation> activeVariation;
    bool inBeginUpdate;
	Util::StringAtom shaderName;

    Util::Array<Ptr<CoreGraphics::ShaderVariable>> variables;
    Util::Dictionary<Base::ShaderVariableBase::Name, Ptr<CoreGraphics::ShaderVariable>> variablesByName;
    Util::Dictionary<CoreGraphics::ShaderFeature::Mask, Ptr<CoreGraphics::ShaderVariation>> variations;
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
ShaderBase::GetCode() const
{
    return this->shaderIdentifierCode;
}
//------------------------------------------------------------------------------
/**
*/
inline SizeT
ShaderBase::GetNumVariations() const
{
    return this->variations.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariation>&
ShaderBase::GetVariationByIndex(IndexT i) const
{
    return this->variations.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderBase::HasVariation(CoreGraphics::ShaderFeature::Mask featureMask) const
{
    return this->variations.Contains(featureMask);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariation>&
ShaderBase::GetVariationByFeatureMask(CoreGraphics::ShaderFeature::Mask featureMask) const
{
    return this->variations[featureMask];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariation>&
ShaderBase::GetActiveVariation() const
{
    return this->activeVariation;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderBase::HasVariableByName(const Base::ShaderVariableBase::Name& n) const
{
    return this->variablesByName.Contains(n);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ShaderBase::GetNumVariables() const
{
    return this->variables.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>&
ShaderBase::GetVariableByIndex(IndexT i) const
{
    return this->variables[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>&
ShaderBase::GetVariableByName(const CoreGraphics::ShaderVariable::Name& n) const
{
#if NEBULA3_DEBUG
    if (!this->HasVariableByName(n))
    {
        n_error("Invalid shader variable name '%s' in shader '%s'",
            n.Value(), this->GetResourceId().Value());
    }
#endif
    return this->variablesByName[n];
}

} // namespace Base
//------------------------------------------------------------------------------

