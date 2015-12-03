#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ShaderInstanceBase
    
    A shader instance object is created from a shader and contains a local copy
    of the original shader state which can be modified through ShaderVariableInstance
    objects. Shader instance objects are created directly through the shader server.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/string.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/shaderfeature.h"
#include "coregraphics/shaderidentifier.h"
#include "coregraphics/shadervariableinstance.h"

namespace CoreGraphics
{
class Shader;
class ShaderVariable;
}

//------------------------------------------------------------------------------
namespace Base
{
class ShaderInstanceBase : public Core::RefCounted
{
    __DeclareClass(ShaderInstanceBase);
public:
    /// constructor
    ShaderInstanceBase();
    /// destructor
    virtual ~ShaderInstanceBase();
    
    /// discard the shader instance, must be called when instance no longer needed
    void Discard();
    /// return true if this object is valid
    bool IsValid() const;
    /// get pointer to original shader which created this instance
    const Ptr<CoreGraphics::Shader>& GetOriginalShader() const;
    /// get shader code from original shader
    const CoreGraphics::ShaderIdentifier::Code& GetCode() const;

    /// create a new variable instance which will be applied when this shader instance gets applied
    Ptr<CoreGraphics::ShaderVariableInstance> CreateVariableInstance(const Base::ShaderVariableBase::Name& n);
    /// get variable instance based on name, asserts it exists
    const Ptr<CoreGraphics::ShaderVariableInstance>& GetVariableInstance(const Base::ShaderVariableBase::Name& n);

    /// shortcut to select a shader variation through the original shader
    bool SelectActiveVariation(CoreGraphics::ShaderFeature::Mask mask);    

	/// begin all uniform buffers for a synchronous update
	void BeginUpdateSync();
	/// end buffer updates for all uniform buffers
	void EndUpdateSync();

    /// begin rendering through the currently selected variation, returns no. passes
    SizeT Begin();
    /// begin pass
    void BeginPass(IndexT passIndex);
    /// apply shader variables
    void Apply();
    /// commit changes before rendering
    void Commit();
    /// calls shading subsystem post draw callback
    void PostDraw();
	/// end pass
    void EndPass();
    /// end rendering through variation
    void End();
    
protected:   
    friend class ShaderBase;

    /// setup the shader instance from its original shader object
    virtual void Setup(const Ptr<CoreGraphics::Shader>& origShader);
    /// discard the shader instance
    virtual void Cleanup();

    bool inBegin;
    bool inBeginPass;
    Ptr<CoreGraphics::Shader> originalShader;
    Util::Array<Ptr<CoreGraphics::ShaderVariableInstance>> variableInstances;
    Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderVariableInstance>> variableInstancesByName;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>&
ShaderInstanceBase::GetOriginalShader() const
{
    return this->originalShader;
}

} // namespace CoreGraphics
//------------------------------------------------------------------------------


    