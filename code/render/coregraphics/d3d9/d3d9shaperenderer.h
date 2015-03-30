#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9ShapeRenderer
  
    D3D9/Xbox360 implementation of ShapeRenderer.
    
    (C) 2007 Radon Labs GmbH
*/    
#include "coregraphics/base/shaperendererbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9ShapeRenderer : public Base::ShapeRendererBase
{
    __DeclareClass(D3D9ShapeRenderer);
    __DeclareSingleton(D3D9ShapeRenderer);
public:
    /// constructor
    D3D9ShapeRenderer();
    /// destructor
    virtual ~D3D9ShapeRenderer();
    
    /// open the shape renderer
    void Open();
    /// close the shape renderer
    void Close();

    /// draw attached shapes and clear deferred stack, must be called inside render loop
    void DrawShapes();

private:
    /// draw a shape
    void DrawSimpleShape(const Math::matrix44& modelTransform, CoreGraphics::RenderShape::Type shapeType, const Math::float4& color);
    /// draw primitives
    void DrawPrimitives(const Math::matrix44& modelTransform, CoreGraphics::PrimitiveTopology::Code topology, SizeT numPrimitives, const void* vertices, SizeT vertexWidth, const Math::float4& color);
    /// draw indexed primitives
    void DrawIndexedPrimitives(const Math::matrix44& modelTransform, CoreGraphics::PrimitiveTopology::Code topology, SizeT numPrimitives, const void* vertices, SizeT numVertices, SizeT vertexWidth, const void* indices, CoreGraphics::IndexType::Code indexType, const Math::float4& color);
    
    Ptr<CoreGraphics::ShaderInstance> shapeShader;
    ID3DXMesh* shapeMeshes[CoreGraphics::RenderShape::NumShapeTypes];
    Ptr<CoreGraphics::ShaderVariable> modelViewProj;
    Ptr<CoreGraphics::ShaderVariable> diffuseColor;
};

} // namespace Win360
//------------------------------------------------------------------------------
