#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::OGL4ShaderInstance
    
    OGL4 implementation of CoreGraphics::ShaderInstance.
    
    @todo lost/reset device handling

    (C) 2007 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "afxapi.h"
#include "coregraphics/base/shaderinstancebase.h"
#include "coregraphics/base/shaderbase.h"
#include "coregraphics/shaderfeature.h"

namespace OpenGL4
{
	
class OGL4ShaderInstance : public Base::ShaderInstanceBase
{
    __DeclareClass(OGL4ShaderInstance);
public:
    /// constructor
    OGL4ShaderInstance();
    /// destructor
    virtual ~OGL4ShaderInstance();    
    /// select active variation by feature mask
    bool SelectActiveVariation(CoreGraphics::ShaderFeature::Mask featureMask);
    /// begin rendering through the currently selected variation, returns no. passes
    SizeT Begin();
    /// begin pass
    void BeginPass(IndexT passIndex);
    /// commit changes before rendering
    void Commit();
    /// calls shading system post draw callback
    void PostDraw();
    /// end pass
    void EndPass();
    /// end rendering through variation
    void End();
	/// setup static texture bindings
	void SetupSharedTextures();

    /// sets the shader in wireframe mode
    void SetWireframe(bool b);

    /// return handle to AnyFX shader
    AnyFX::Effect* GetAnyFXEffect();
	
protected:
    friend class Base::ShaderBase;
    friend class OGL4Shader;

	/// sets up shader variables from shader handler
	void SetupVariables(const GLuint& shader);
    /// setup the shader instance from its original shader object
    virtual void Setup(const Ptr<CoreGraphics::Shader>& origShader);
	/// reload the shader instance from original shader object
	virtual void Reload(const Ptr<CoreGraphics::Shader>& origShader);


	/// reload the variables from reflection
	void ReloadVariables(const GLuint& shader);

    /// cleanup the shader instance
    virtual void Cleanup();
    /// called by ogl4 shader server when ogl4 device is lost
    void OnLostDevice();
    /// called by ogl4 shader server when ogl4 device is reset
    void OnResetDevice();

private:
    bool inWireframe;
	AnyFX::Effect* effect;
};

//------------------------------------------------------------------------------
/**
*/
inline AnyFX::Effect* 
OGL4ShaderInstance::GetAnyFXEffect()
{
    return this->effect;
}

} // namespace OpenGL4
//------------------------------------------------------------------------------

    