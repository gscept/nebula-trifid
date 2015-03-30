#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9VertexLayout
  
    D3D9/Xbox360-implementation of vertex layout.
    
    (C) 2007 Radon Labs GmbH
*/    
#include "coregraphics/base/vertexlayoutbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9VertexLayout : public Base::VertexLayoutBase
{
    __DeclareClass(D3D9VertexLayout);
public:
    /// constructor
    D3D9VertexLayout();
    /// destructor
    virtual ~D3D9VertexLayout();

    /// setup the vertex layout
    void Setup(const Util::Array<CoreGraphics::VertexComponent>& c);
    /// discard the vertex layout object
    void Discard();
    /// set d3d9 vertex declaration pointer
    void SetD3D9VertexDeclaration(IDirect3DVertexDeclaration9* ptr);
    /// get pointer to d3d9 vertex declaration
    IDirect3DVertexDeclaration9* GetD3D9VertexDeclaration() const;
        
private:
    IDirect3DVertexDeclaration9* d3d9VertexDeclaration;
};

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9VertexLayout::SetD3D9VertexDeclaration(IDirect3DVertexDeclaration9* ptr)
{
    n_assert(0 != ptr);
    n_assert(0 == this->d3d9VertexDeclaration);
    this->d3d9VertexDeclaration = ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DVertexDeclaration9*
D3D9VertexLayout::GetD3D9VertexDeclaration() const
{
    n_assert(0 != this->d3d9VertexDeclaration);
    return this->d3d9VertexDeclaration;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
