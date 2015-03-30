#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIGeometryBuffer

    Wrapper for the cegui geometry buffer.

    (C) 2009 Radon Labs GmbH
*/
//------------------------------------------------------------------------------
#include "cegui/include/CEGUIGeometryBuffer.h"
#include "cegui/include/CEGUIVector.h"
#include "cegui/include/CEGUIRect.h"
#include "cegui/include/CEGUIVertex.h"
#include "util/array.h"
#include "util/dictionary.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/primitivegroup.h"
#include "sui/ceui/ceuitexture.h"
#include "render/coregraphics/shadervariable.h"
#include "render/coregraphics/shaderinstance.h"

namespace CEUI
{

class CEUIGeometryBuffer : public CEGUI::GeometryBuffer
{
public:
    /// constructor
    CEUIGeometryBuffer();
    /// destructor
    virtual ~CEUIGeometryBuffer();

    // implementation of cegui GeometryBuffer

    //
    virtual void draw() const;
    //
    virtual void setTranslation(const CEGUI::Vector3& v);
    //
    virtual void setRotation(const CEGUI::Vector3& r);
    //
    virtual void setPivot(const CEGUI::Vector3& p);
    //
    virtual void setClippingRegion(const CEGUI::Rect& region);
    //
    virtual void appendVertex(const CEGUI::Vertex& vertex);
    //
    virtual void appendGeometry(const CEGUI::Vertex* const vbuff, CEGUI::uint vertex_count);
    //
    virtual void setActiveTexture(CEGUI::Texture* texture);
    //
    virtual CEGUI::Texture* getActiveTexture() const;
    //
    virtual CEGUI::uint getVertexCount() const;
    //
    virtual CEGUI::uint getBatchCount() const;
    //
    virtual void setRenderEffect(CEGUI::RenderEffect* effect);
    //
    virtual CEGUI::RenderEffect* getRenderEffect();
    //
    virtual void reset();

protected:
    /// Update vertex buffer
    void UpdateVertexBuffer();

    struct VertexBufferEntry
    {
        Math::point pos;
        Math::float4 col;
        Math::float2 uv;
    };

    typedef Util::KeyValuePair<Ptr<CoreGraphics::Texture>, CEGUI::uint> BatchInfo;
    Util::Array<BatchInfo> batches;
    Util::Array<VertexBufferEntry> vbEntries;
    Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
    CoreGraphics::PrimitiveGroup primGroup;
    CoreGraphics::ShaderVariable::Semantic guiTextureSemantic;
    Ptr<CoreGraphics::ShaderInstance> shdInst;

    CEGUI::Vector3 translation;
    CEGUI::Vector3 rotation;
    CEGUI::Vector3 pivot;
    CEGUI::Rect clippingRegion;
    CEGUI::RenderEffect* renderEffect;
    CEUITexture* activeTexture;

    SizeT vertexCount;

    CEGUI::Vector2 minUV;
    CEGUI::Vector2 maxUV;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIGeometryBuffer::setClippingRegion(const CEGUI::Rect& region)
{
    this->clippingRegion = region;
}

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::uint
CEUIGeometryBuffer::getVertexCount() const
{
    return this->vertexCount;
}

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::uint
CEUIGeometryBuffer::getBatchCount() const
{
    return batches.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIGeometryBuffer::setRenderEffect(CEGUI::RenderEffect* effect)
{
    this->renderEffect = effect;
}

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::RenderEffect*
CEUIGeometryBuffer::getRenderEffect()
{
    return this->renderEffect;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIGeometryBuffer::setActiveTexture(CEGUI::Texture* texture)
{
    this->activeTexture = static_cast<CEUITexture*>(texture);
}

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::Texture*
CEUIGeometryBuffer::getActiveTexture() const
{
    return this->activeTexture;
}

}