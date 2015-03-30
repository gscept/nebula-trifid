#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ShaderServerBase
    
    In Nebula3, all shaders required by an application are loaded at once
    by the central ShaderServer. The shader server loads all shaders in
    ShaderServer::Open() from the location defined by the "shaders:" assign.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/shader.h"
#include "coregraphics/shaderfeature.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shaderinstance.h"

namespace CoreGraphics
{
    class Shader;
}

//------------------------------------------------------------------------------
namespace Base
{
class ShaderServerBase : public Core::RefCounted
{
    __DeclareClass(ShaderServerBase);
    __DeclareSingleton(ShaderServerBase);
public:
    /// constructor
    ShaderServerBase();
    /// destructor
    virtual ~ShaderServerBase();
    
    /// open the shader server
    bool Open();
    /// close the shader server
    void Close();
    /// return true if the shader server is open
    bool IsOpen() const;

    /// return true if a shader exists
    bool HasShader(const Resources::ResourceId& resId) const;
    /// create a new shader instance
    Ptr<CoreGraphics::ShaderInstance> CreateShaderInstance(const Resources::ResourceId& resId);
    /// get all loaded shaders
    const Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::Shader> >& GetAllShaders() const;
	/// get shader by name
	const Ptr<CoreGraphics::Shader>& GetShader(Resources::ResourceId resId) const;
    /// set currently active shader instance
    void SetActiveShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& shaderInst);
    /// get currently active shader instance
    const Ptr<CoreGraphics::ShaderInstance>& GetActiveShaderInstance() const;

    /// reset the current feature bits
    void ResetFeatureBits();
    /// set shader feature by bit mask
    void SetFeatureBits(CoreGraphics::ShaderFeature::Mask m);
    /// clear shader feature by bit mask
    void ClearFeatureBits(CoreGraphics::ShaderFeature::Mask m);
    /// get the current feature mask
    CoreGraphics::ShaderFeature::Mask GetFeatureBits() const;
    /// convert a shader feature string into a feature bit mask
    CoreGraphics::ShaderFeature::Mask FeatureStringToMask(const Util::String& str);
    /// convert shader feature bit mask into string
    Util::String FeatureMaskToString(CoreGraphics::ShaderFeature::Mask mask);

    /// apply an object id
    void ApplyObjectId(IndexT i);

    /// return true if a shared variable exists by semantic
    bool HasSharedVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& sem) const;
    /// get number of shared variables
    SizeT GetNumSharedVariables() const;
    /// get a shared variable by index
    const Ptr<CoreGraphics::ShaderVariable>& GetSharedVariableByIndex(IndexT i) const;
    /// get a shared variable by semantic
    const Ptr<CoreGraphics::ShaderVariable>& GetSharedVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& sem) const;
	/// get a variable by semantic from the current vertex shader
	const Ptr<CoreGraphics::ShaderVariable>& GetDefaultShaderVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& sem) const;

protected:
    CoreGraphics::ShaderFeature shaderFeature;
    CoreGraphics::ShaderFeature::Mask curShaderFeatureBits;
    Util::Dictionary<Resources::ResourceId,Ptr<CoreGraphics::Shader> > shaders;
    Ptr<CoreGraphics::ShaderInstance> sharedVariableShaderInst;    
    Ptr<CoreGraphics::ShaderInstance> activeShaderInstance;
    Ptr<CoreGraphics::ShaderVariable> objectIdShaderVar;
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderServerBase::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderServerBase::HasShader(const Resources::ResourceId& resId) const
{
    return this->shaders.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::Shader> >&
ShaderServerBase::GetAllShaders() const
{
    return this->shaders;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>& 
ShaderServerBase::GetShader( Resources::ResourceId resId ) const
{
	return this->shaders[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::ShaderFeature::Mask
ShaderServerBase::FeatureStringToMask(const Util::String& str)
{
    return this->shaderFeature.StringToMask(str);
}

//------------------------------------------------------------------------------
/**
*/
inline Util::String
ShaderServerBase::FeatureMaskToString(CoreGraphics::ShaderFeature::Mask mask)
{
    return this->shaderFeature.MaskToString(mask);
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderServerBase::ResetFeatureBits()
{
    this->curShaderFeatureBits = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderServerBase::SetFeatureBits(CoreGraphics::ShaderFeature::Mask m)
{
    this->curShaderFeatureBits |= m;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderServerBase::ClearFeatureBits(CoreGraphics::ShaderFeature::Mask m)
{
    this->curShaderFeatureBits &= ~m;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::ShaderFeature::Mask
ShaderServerBase::GetFeatureBits() const
{
    return this->curShaderFeatureBits;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ShaderServerBase::SetActiveShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& shaderInst)
{
    this->activeShaderInstance = shaderInst;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
ShaderServerBase::GetActiveShaderInstance() const
{
    return this->activeShaderInstance;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ShaderServerBase::HasSharedVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& sem) const
{
    if (this->sharedVariableShaderInst.isvalid())
    {        
        return this->sharedVariableShaderInst->HasVariableBySemantic(sem);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ShaderServerBase::GetNumSharedVariables() const
{
    if (this->sharedVariableShaderInst.isvalid())
    {        
        return this->sharedVariableShaderInst->GetNumVariables();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>&
ShaderServerBase::GetSharedVariableByIndex(IndexT i) const
{
    n_assert(this->sharedVariableShaderInst.isvalid());
    return this->sharedVariableShaderInst->GetVariableByIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>&
ShaderServerBase::GetSharedVariableBySemantic(const CoreGraphics::ShaderVariable::Semantic& sem) const
{
    n_assert(this->sharedVariableShaderInst.isvalid());
    return this->sharedVariableShaderInst->GetVariableBySemantic(sem);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariable>& 
ShaderServerBase::GetDefaultShaderVariableBySemantic( const CoreGraphics::ShaderVariable::Semantic& sem ) const
{
	n_assert(this->activeShaderInstance.isvalid());
	return this->activeShaderInstance->GetVariableBySemantic(sem);
}

} // namespace Base
//------------------------------------------------------------------------------

