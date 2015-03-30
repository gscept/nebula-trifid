//------------------------------------------------------------------------------
//  ceuigeometrybuffer.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuigeometrybuffer.h"
#include "sui/ceui/ceuiserver.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "resources/resourceloader.h"
#include "coregraphics/renderdevice.h"

namespace CEUI
{
using namespace CoreGraphics;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
CEUIGeometryBuffer::CEUIGeometryBuffer() :
    translation(0, 0, 0),
    rotation(0, 0, 0),
    pivot(0, 0, 0),
    renderEffect(0),
    vertexCount(0)
{
    // create vertex buffer
    this->vertexBuffer = VertexBuffer::Create();

    // MemoryVertexBufferLoader::Setup mit 0-Datapointern
    Util::Array<VertexComponent> vertexComponents;
    vertexComponents.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float3));
    vertexComponents.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4));
    vertexComponents.Append(VertexComponent(VertexComponent::TexCoord, 0, VertexComponent::Float2));

    Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
    vbLoader->Setup(vertexComponents, 
                    4096, 
                    0, 
                    4096 * 9 * sizeof(float),
                    Base::ResourceBase::UsageDynamic,
                    Base::ResourceBase::AccessWrite);
    this->vertexBuffer->SetLoader(vbLoader.upcast<ResourceLoader>());
    this->vertexBuffer->SetAsyncEnabled(false);
    this->vertexBuffer->Load();
    n_assert(this->vertexBuffer->IsLoaded());
    this->vertexBuffer->SetLoader(0);

    // set primitive group
    this->primGroup.SetBaseVertex(0);
    this->primGroup.SetNumVertices(4096);
    this->primGroup.SetBaseIndex(0);
    this->primGroup.SetNumIndices(0);

    this->primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
}

//------------------------------------------------------------------------------
/**
*/
CEUIGeometryBuffer::~CEUIGeometryBuffer()
{
    if (this->vertexBuffer.isvalid())
    {
        this->vertexBuffer->Unload();
        this->vertexBuffer = 0;
    }
    this->batches.Clear();
    this->vertexCount = 0;
    this->activeTexture = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::draw() const
{
    // render batches
    //in case of this assertion: increase the number of vertices (4096) in open-function
    n_assert(this->vertexBuffer.isvalid()); 
    n_assert((int)this->getVertexCount() < (int)this->vertexBuffer->GetNumVertices()); 
    
    // just do if one element min
    if (this->getBatchCount() <= 0)
    {
        return;
    }

    const_cast<CEUIGeometryBuffer*>(this)->UpdateVertexBuffer();

    // get shader instance
    const_cast<CEUIGeometryBuffer*>(this)->shdInst = CEUIRenderer::Instance()->GetShaderInstance();
    const_cast<CEUIGeometryBuffer*>(this)->guiTextureSemantic = CEUIRenderer::Instance()->GetTextureSemantic();

    // now simply draw the vertex buffer
    IndexT currentIndex = 0;
    IndexT index;
    for (index = 0; index < this->batches.Size(); index++)
    {
        const BatchInfo& batch = this->batches[index];

        //// apply shader variables
        shdInst->GetVariableBySemantic(this->guiTextureSemantic)->SetTexture(batch.Key());
#ifdef __DX11__
        shdInst->CommitAll();
#else
		shdInst->Commit();
#endif

        //// render the vertex buffer
        RenderDevice* renderDevice = RenderDevice::Instance();        
        const_cast<CEUIGeometryBuffer*>(this)->primGroup.SetBaseVertex(currentIndex);
        const_cast<CEUIGeometryBuffer*>(this)->primGroup.SetNumVertices(batch.Value());
        renderDevice->SetStreamSource(0, this->vertexBuffer, 0);
        renderDevice->SetVertexLayout(this->vertexBuffer->GetVertexLayout());
        renderDevice->SetPrimitiveGroup(this->primGroup);
        renderDevice->Draw();
        currentIndex += batch.Value();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::appendVertex(const CEGUI::Vertex& vertex)
{
    this->appendGeometry(&vertex, 1);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::appendGeometry(const CEGUI::Vertex* const vbuff, CEGUI::uint vertex_count)
{
    // create new batch if this is the first one or
    // active texture differs from last batch
    if (this->batches.IsEmpty() || this->batches.Back().Key() != this->activeTexture->GetTexture())
    {
        this->batches.Append(BatchInfo(this->activeTexture->GetTexture(), vertex_count));
    }
    else
    {
        this->batches.Back().Value() += vertex_count;
    }

    // transform to screen coordinates
    const CEGUI::Size& sz = CEUI::CEUIRenderer::Instance()->getDisplaySize();
    float posToScreenX = 1.0f / sz.d_width;
    float posToScreenY = 1.0f / sz.d_height;

    // append vertex buffer entries to this batch
    CEGUI::uint vertexId;
    for (vertexId = 0; vertexId < vertex_count; vertexId++)
    {
        VertexBufferEntry entry;

        entry.pos.set_x(((vbuff[vertexId].position.d_x * posToScreenX) * 2.0f)- 1.0f); 
        entry.pos.set_y(-(((vbuff[vertexId].position.d_y * posToScreenY) * 2.0f) -1.0f)); 
        entry.pos.set_z(vbuff[vertexId].position.d_z);

        entry.col.set_x(vbuff[vertexId].colour_val.getRed());
        entry.col.set_y(vbuff[vertexId].colour_val.getGreen());
        entry.col.set_z(vbuff[vertexId].colour_val.getBlue());
        entry.col.set_w(vbuff[vertexId].colour_val.getAlpha());

        // copy vertex uv
        entry.uv.set(vbuff[vertexId].tex_coords.d_x, vbuff[vertexId].tex_coords.d_y);

        this->vbEntries.Append(entry);
    }
    this->vertexCount += vertex_count;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::UpdateVertexBuffer()
{
    // map vertex buffer for appending vertices
    float* vertexPtr = (float*)this->vertexBuffer->Map(VertexBuffer::MapWriteDiscard);

    // append vertex buffer entries to this batch
    CEGUI::uint vertexId;
    for (vertexId = 0; vertexId < this->getVertexCount(); vertexId++)
    {
        // copy vertex position
        vertexPtr[0] = this->vbEntries[vertexId].pos.x() + this->translation.d_x;
        vertexPtr[1] = this->vbEntries[vertexId].pos.y() + this->translation.d_y;
        vertexPtr[2] = this->vbEntries[vertexId].pos.z() + this->translation.d_z;
        vertexPtr += 3;

        // copy vertex colour
        vertexPtr[0] = this->vbEntries[vertexId].col.x();
        vertexPtr[1] = this->vbEntries[vertexId].col.y();
        vertexPtr[2] = this->vbEntries[vertexId].col.z();
        vertexPtr[3] = this->vbEntries[vertexId].col.w();
        vertexPtr += 4;

        // copy vertex uv
        vertexPtr[0] = this->vbEntries[vertexId].uv.x();
        vertexPtr[1] = this->vbEntries[vertexId].uv.y();
        vertexPtr += 2;
    }

    // unmap vertex buffer
    this->vertexBuffer->Unmap();
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::reset()
{
    this->batches.Clear();
	this->vbEntries.Clear();
    this->vertexCount = 0;
    this->activeTexture = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::setTranslation(const CEGUI::Vector3& v)
{
    // transform to screen coordinates
    const CEGUI::Size& sz = CEUI::CEUIRenderer::Instance()->getDisplaySize();
    float posToScreenX = 1.0f / sz.d_width;
    float posToScreenY = 1.0f / sz.d_height;

    this->translation.d_x = v.d_x * posToScreenX * 2.0f;
    this->translation.d_y = -v.d_y * posToScreenY * 2.0f;
    this->translation.d_z = v.d_z;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::setRotation(const CEGUI::Vector3& r)
{
    this->rotation = r;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIGeometryBuffer::setPivot(const CEGUI::Vector3& p)
{
    this->pivot = p;
}

} // namespace CEUI