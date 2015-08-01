//------------------------------------------------------------------------------
//  drawquad.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "renderutil/drawquad.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/renderdevice.h"

namespace RenderUtil
{
using namespace Util;
using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
DrawQuad::DrawQuad() :
    isValid(false)
{
    // empty
}    

//------------------------------------------------------------------------------
/**
*/
DrawQuad::~DrawQuad()
{
    if (this->IsValid())
    {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DrawQuad::Setup(SizeT width, SizeT height)
{
    n_assert(!this->IsValid());
    this->isValid = true;

    // setup vertex components
    Array<VertexComponent> vertexComponents;
    vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
    vertexComponents.Append(VertexComponent(VertexComponent::TexCoord1, 0, VertexComponent::Float2));

    // compute screen rectangle coordinates
    Math::float4 pixelSize(1.0f / float(width), 1.0f / float(height), 0.0f, 0.0f);
    Math::float4 halfPixelSize = pixelSize * 0.5f;
    float left   = -1.0f - halfPixelSize.x();
    float right  = +1.0f - halfPixelSize.x();
    float top    = +1.0f + halfPixelSize.y();
    float bottom = -1.0f + halfPixelSize.y();

    // compute uv coordinates
    // FIXME: add half-pixel size here?
    float u0 = 0.0f;
    float u1 = 1.0f;
    float v0 = 0.0f;
    float v1 = 1.0f;

    // setup a vertex buffer with 2 triangles
    float v[6][5];

    // first triangle
    v[0][0] = left;  v[0][1] = top;    v[0][2] = 0.0f; v[0][3] = u0; v[0][4] = v0;
    v[1][0] = left;  v[1][1] = bottom; v[1][2] = 0.0f; v[1][3] = u0; v[1][4] = v1;
    v[2][0] = right; v[2][1] = top;    v[2][2] = 0.0f; v[2][3] = u1; v[2][4] = v0;

    // second triangle
    v[3][0] = left;  v[3][1] = bottom; v[3][2] = 0.0f; v[3][3] = u0; v[3][4] = v1;
    v[4][0] = right; v[4][1] = bottom; v[4][2] = 0.0f; v[4][3] = u1; v[4][4] = v1;
    v[5][0] = right; v[5][1] = top;    v[5][2] = 0.0f; v[5][3] = u1; v[5][4] = v0;

    // setup vertex buffer with memory-vertexbuffer-loader
    this->vertexBuffer = VertexBuffer::Create();
    Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
    vbLoader->Setup(vertexComponents, 6, v, 6 * 5 * sizeof(float), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
    this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
    this->vertexBuffer->SetAsyncEnabled(false);
    this->vertexBuffer->Load();
    n_assert(this->vertexBuffer->IsLoaded());
    this->vertexBuffer->SetLoader(0);

    // setup a primitive group object
    this->primGroup.SetBaseVertex(0);
    this->primGroup.SetNumVertices(6);
    this->primGroup.SetBaseIndex(0);
    this->primGroup.SetNumIndices(0);
    this->primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
}

//------------------------------------------------------------------------------
/**
*/
void
DrawQuad::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
    this->vertexBuffer->Unload();
    this->vertexBuffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
DrawQuad::Draw()
{
    RenderDevice* renderDevice = RenderDevice::Instance();
    renderDevice->SetStreamVertexBuffer(0, this->vertexBuffer, 0);
    renderDevice->SetVertexLayout(this->vertexBuffer->GetVertexLayout());
    renderDevice->SetPrimitiveGroup(this->primGroup);
    renderDevice->Draw();
}

} // namespace RenderUtil
