//------------------------------------------------------------------------------
//  d3d9vertexlayout.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/win360/d3d9vertexlayout.h"
#include "coregraphics/win360/d3d9types.h"
#include "coregraphics/renderdevice.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9VertexLayout, 'D9VL', Base::VertexLayoutBase);

using namespace CoreGraphics;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
D3D9VertexLayout::D3D9VertexLayout() :
    d3d9VertexDeclaration(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9VertexLayout::~D3D9VertexLayout()
{
    n_assert(0 == this->d3d9VertexDeclaration);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9VertexLayout::Setup(const Array<VertexComponent>& c)
{
    n_assert(0 == this->d3d9VertexDeclaration);

    // call parent class
    Base::VertexLayoutBase::Setup(c);

    // create a D3D9 vertex declaration object
    const SizeT maxElements = 32;
    n_assert(this->components.Size() < maxElements);
    D3DVERTEXELEMENT9 decl[maxElements] = { 0 };
    IndexT curOffset[RenderDevice::MaxNumVertexStreams] = { 0 };
    IndexT compIndex;
    for (compIndex = 0; compIndex < this->components.Size(); compIndex++)
    {
        const VertexComponent& component = this->components[compIndex];
        WORD streamIndex = (WORD) component.GetStreamIndex();
        n_assert(streamIndex < RenderDevice::MaxNumVertexStreams);
        decl[compIndex].Stream = streamIndex;
        decl[compIndex].Offset = (WORD) curOffset[streamIndex];
        decl[compIndex].Type   = D3D9Types::AsD3D9VertexDeclarationType(component.GetFormat());
        decl[compIndex].Method = D3DDECLMETHOD_DEFAULT;
        decl[compIndex].Usage  = (BYTE) D3D9Types::AsD3D9VertexDeclarationUsage(component.GetSemanticName());
        decl[compIndex].UsageIndex = (BYTE) component.GetSemanticIndex();
        curOffset[streamIndex] += component.GetByteSize();
    }
    decl[compIndex].Stream = 0xff;
    decl[compIndex].Type = (WORD) D3DDECLTYPE_UNUSED;

    IDirect3DDevice9* d3d9Dev = RenderDevice::Instance()->GetDirect3DDevice();
    HRESULT hr = d3d9Dev->CreateVertexDeclaration(decl, &this->d3d9VertexDeclaration);
    n_assert(SUCCEEDED(hr));
    n_assert(0 != this->d3d9VertexDeclaration);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9VertexLayout::Discard()
{
    n_assert(0 != this->d3d9VertexDeclaration);
    this->d3d9VertexDeclaration->Release();
    this->d3d9VertexDeclaration = 0;
    VertexLayoutBase::Discard();
}

} // namespace Win360
