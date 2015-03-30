//------------------------------------------------------------------------------
//  d3d9shadervariable.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9shadervariable.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9ShaderVariable, 'D9VR', Base::ShaderVariableBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderVariable::D3D9ShaderVariable() :
    d3d9Effect(0),
    hParam(0)
{
    // empty
}    

//------------------------------------------------------------------------------
/**
*/
D3D9ShaderVariable::~D3D9ShaderVariable()
{
    this->d3d9Effect = 0;
    this->hParam = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9ShaderVariable::Setup(ID3DXEffect* effect, D3DXHANDLE handle)
{
    n_assert(0 != effect);
    n_assert(0 != handle);
    n_assert(0 == this->d3d9Effect);
    n_assert(0 == this->hParam);
    this->d3d9Effect = effect;
    this->hParam = handle;

    // setup ourself from parameter desc
    D3DXPARAMETER_DESC desc = { 0 };
    HRESULT hr = this->d3d9Effect->GetParameterDesc(this->hParam, &desc);
    n_assert(SUCCEEDED(hr));
    this->SetName(Name(desc.Name));
    this->SetSemantic(Semantic(desc.Semantic));

    // crack the data type
    switch (desc.Class)
    {
        case D3DXPC_SCALAR:
            switch (desc.Type)
            {
                case D3DXPT_BOOL:
                    this->SetType(BoolType);
                    break;
                case D3DXPT_INT:
                    this->SetType(IntType);
                    break;
                case D3DXPT_FLOAT:
                    this->SetType(FloatType);
                    break;
                default:
                    this->SetType(UnknownType);
                    break;
            }
            break;

        case D3DXPC_VECTOR:
            switch (desc.Type)
            {
                case D3DXPT_FLOAT:
                    this->SetType(VectorType);
                    break;
                default:
                    this->SetType(UnknownType);
                    break;
            }
            break;

        case D3DXPC_MATRIX_ROWS:
        case D3DXPC_MATRIX_COLUMNS:
            switch (desc.Type)
            {
                case D3DXPT_FLOAT:                   
                    this->SetType(MatrixType);                   
                    break;
                default:
                    this->SetType(UnknownType);
                    break;
            }
            break;

        case D3DXPC_OBJECT:
            switch (desc.Type)
            {
                case D3DXPT_TEXTURE:
                case D3DXPT_TEXTURE1D:
                case D3DXPT_TEXTURE2D:
                case D3DXPT_TEXTURE3D:
                    this->SetType(TextureType);
                    break;
                default:
                    this->SetType(UnknownType);
            }
            break;

        default:
            this->SetType(UnknownType);
            break;
    }
}

} // namespace Direct3D9
