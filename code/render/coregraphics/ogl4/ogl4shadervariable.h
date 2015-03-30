#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::OGL4ShaderVariable
    
    OGL4 implementation of ShaderVariable.
    
    (C) 2013 Gustav Sterbrant
*/
#include "afxapi.h"
#include "coregraphics/base/shadervariablebase.h"
#include "coregraphics/texture.h"
#include "util/variant.h"

//------------------------------------------------------------------------------

namespace OpenGL4
{
class OGL4ShaderInstance;
	
class OGL4ShaderVariable : public Base::ShaderVariableBase
{
    __DeclareClass(OGL4ShaderVariable);
public:
    /// constructor
    OGL4ShaderVariable();
    /// destructor
    virtual ~OGL4ShaderVariable();
    
    /// set int value
    void SetInt(int value);
    /// set int array values
    void SetIntArray(const int* values, SizeT count);
    /// set float value
    void SetFloat(float value);
    /// set float array values
    void SetFloatArray(const float* values, SizeT count);
	/// set vector value
	void SetFloat2(const Math::float2& value);
	/// set vector array values
	void SetFloat2Array(const Math::float2* values, SizeT count);
    /// set vector value
    void SetFloat4(const Math::float4& value);
    /// set vector array values
    void SetFloat4Array(const Math::float4* values, SizeT count);
    /// set matrix value
    void SetMatrix(const Math::matrix44& value);
    /// set matrix array values
    void SetMatrixArray(const Math::matrix44* values, SizeT count);    
    /// set bool value
    void SetBool(bool value);
    /// set bool array values
    void SetBoolArray(const bool* values, SizeT count);
    /// set texture value
    void SetTexture(const Ptr<CoreGraphics::Texture>& value);
	/// sets buffer handle
	void SetBufferHandle(void* handle);

	/// special handling of textures
	Ptr<CoreGraphics::Texture> GetTexture();

	/// cleanup
	void Cleanup();
private:
    friend class OpenGL4::OGL4ShaderInstance;

	/// setup from AnyFX variable
	void Setup(AnyFX::EffectVariable* var);
	/// setup from AnyFX varbuffer
	void Setup(AnyFX::EffectVarbuffer* var);

	Ptr<OpenGL4::OGL4ShaderInstance> parentInstance;
	Ptr<CoreGraphics::Texture> texture;

	AnyFX::EffectVariable* effectVar;
	AnyFX::EffectVarbuffer* effectBuffer;
	bool reload;
};


//------------------------------------------------------------------------------
/**
*/
inline Ptr<CoreGraphics::Texture>
OGL4ShaderVariable::GetTexture()
{
	return texture;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
    