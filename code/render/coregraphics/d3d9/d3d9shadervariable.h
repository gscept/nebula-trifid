#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9ShaderVariable
    
    D3D9 implementation of ShaderVariable.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/shadervariablebase.h"
#include "coregraphics/texture.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9ShaderVariable : public Base::ShaderVariableBase
{
    __DeclareClass(D3D9ShaderVariable);
public:
    /// constructor
    D3D9ShaderVariable();
    /// destructor
    virtual ~D3D9ShaderVariable();
    
    /// set int value
    void SetInt(int value);
    /// set int array values
    void SetIntArray(const int* values, SizeT count);
    /// set float value
    void SetFloat(float value);
    /// set float array values
    void SetFloatArray(const float* values, SizeT count);
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

private:
    friend class D3D9ShaderInstance;
    
    /// setup from D3DX effect and parameter handle
    void Setup(ID3DXEffect* effect, D3DXHANDLE handle);

    ID3DXEffect* d3d9Effect;
    D3DXHANDLE hParam;
};

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetInt(int value)
{
    this->d3d9Effect->SetInt(this->hParam, value);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetIntArray(const int* values, SizeT count)
{
    this->d3d9Effect->SetIntArray(this->hParam, values, count);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetFloat(float value)
{
    this->d3d9Effect->SetFloat(this->hParam, value);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetFloatArray(const float* values, SizeT count)
{
    this->d3d9Effect->SetFloatArray(this->hParam, values, count);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetFloat4(const Math::float4& value)
{
    this->d3d9Effect->SetVector(this->hParam, (CONST D3DXVECTOR4*) &value);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetFloat4Array(const Math::float4* values, SizeT count)
{
    this->d3d9Effect->SetVectorArray(this->hParam, (CONST D3DXVECTOR4*) values, count);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetMatrix(const Math::matrix44& value)
{
    this->d3d9Effect->SetMatrix(this->hParam, (CONST D3DXMATRIX*) &value);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetMatrixArray(const Math::matrix44* values, SizeT count)
{
    this->d3d9Effect->SetMatrixArray(this->hParam, (CONST D3DXMATRIX*) values, count);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetBool(bool value)
{
    this->d3d9Effect->SetBool(this->hParam, value);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetBoolArray(const bool* values, SizeT count)
{
    // hmm... Win32's BOOL is actually an int
    const int MaxNumBools = 128;
    n_assert(count < MaxNumBools);
    BOOL tmp[MaxNumBools];
    IndexT i;
    for (i = 0; i < count; i++)
    {
        tmp[i] = (BOOL) values[i];
    }
    this->d3d9Effect->SetBoolArray(this->hParam, tmp, count);
}

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9ShaderVariable::SetTexture(const Ptr<CoreGraphics::Texture>& value)
{
    this->d3d9Effect->SetTexture(this->hParam, value->GetD3D9BaseTexture());
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
    