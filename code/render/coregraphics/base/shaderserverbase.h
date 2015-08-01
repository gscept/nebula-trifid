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
#include "coregraphics/shaderidentifier.h"

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
    void SetActiveShader(const Ptr<CoreGraphics::Shader>& shader);
    /// get currently active shader instance
    const Ptr<CoreGraphics::Shader>& GetActiveShader() const;

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

    /// get number of shared variables
    SizeT GetNumSharedVariables() const;
    /// get a shared variable by index
    const Ptr<CoreGraphics::ShaderVariable>& GetSharedVariableByIndex(IndexT i) const;
    /// get the shared shader
    const Ptr<CoreGraphics::Shader>& GetSharedShader();

protected:
    friend class CoreGraphics::ShaderIdentifier;
    friend class ShaderBase;

    CoreGraphics::ShaderIdentifier shaderIdentifierRegistry;
    CoreGraphics::ShaderFeature shaderFeature;
    CoreGraphics::ShaderFeature::Mask curShaderFeatureBits;
    Util::Dictionary<Resources::ResourceId,Ptr<CoreGraphics::Shader>> shaders;
    Ptr<CoreGraphics::Shader> sharedVariableShader;    
    Ptr<CoreGraphics::ShaderVariable> objectIdShaderVar;
    Ptr<CoreGraphics::Shader> activeShader;
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
ShaderServerBase::SetActiveShader(const Ptr<CoreGraphics::Shader>& shader)
{
    this->activeShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>&
ShaderServerBase::GetActiveShader() const
{
    return this->activeShader;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ShaderServerBase::GetNumSharedVariables() const
{
    if (this->sharedVariableShader.isvalid())
    {        
        return this->sharedVariableShader->GetNumVariables();
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
    n_assert(this->sharedVariableShader.isvalid());
    return this->sharedVariableShader->GetVariableByIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>&
ShaderServerBase::GetSharedShader()
{
    return this->sharedVariableShader;
}

} // namespace Base
//------------------------------------------------------------------------------

