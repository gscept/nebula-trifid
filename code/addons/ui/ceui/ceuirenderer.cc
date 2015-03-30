//------------------------------------------------------------------------------
//  suiserver.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shaderserver.h"
#include "sui/ceui/ceuirenderer.h"
#include "sui/ceui/ceuiviewporttarget.h"
#include "cegui/include/CEGUIRenderingRoot.h"

namespace CEUI
{
__ImplementClass(CEUIRenderer, 'CERB', Core::RefCounted);
__ImplementSingleton(CEUIRenderer);

using namespace CoreGraphics;
using namespace Resources;

CEGUI::String CEUIRenderer::identifier("CEGUI::CEUIRenderer - Nebula 3 renderer module.");

//------------------------------------------------------------------------------
/**
*/
CEUIRenderer::CEUIRenderer() :
    displayDPI(96, 96)
{
    __ConstructSingleton;

    this->defaultTarget = n_new(CEUIViewportTarget(*this));
    this->defaultRoot = n_new(CEGUI::RenderingRoot(*this->defaultTarget));

    // get display size from DisplayMode
    const CoreGraphics::DisplayMode& dm = CoreGraphics::DisplayDevice::Instance()->GetDisplayMode();
    this->setDisplaySize(CEGUI::Size((float)dm.GetWidth(), (float)dm.GetHeight()));

    // get the shader semantic
#ifdef __DX11__
    this->guiTextureSemantic = ShaderVariable::Semantic("Texture");
#else
	this->guiTextureSemantic = ShaderVariable::Semantic("DiffMap0");
#endif
}

//------------------------------------------------------------------------------
/**
*/
CEUIRenderer::~CEUIRenderer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::GeometryBuffer&
CEUIRenderer::createGeometryBuffer()
{
    CEUIGeometryBuffer* b = n_new(CEUIGeometryBuffer);
    this->geometryBuffers.Append(b);
    return (*b);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyGeometryBuffer(const CEGUI::GeometryBuffer& buffer)
{
    IndexT geomId = this->geometryBuffers.FindIndex((CEUIGeometryBuffer*)&buffer);
    n_assert(InvalidIndex != geomId)
    n_delete(this->geometryBuffers[geomId]);
    this->geometryBuffers.EraseIndex(geomId);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyAllGeometryBuffers()
{
    IndexT geomId = 0;
    for (geomId = 0; geomId < this->geometryBuffers.Size(); geomId++)
    {
        n_delete(this->geometryBuffers[geomId]);
    }
    this->geometryBuffers.Clear();
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::TextureTarget*
CEUIRenderer::createTextureTarget()
{
    // we do not support texture targets
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyTextureTarget(CEGUI::TextureTarget* target)
{
    // empty
    n_error("We do not support texture targets. This method should not be called!\n");
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyAllTextureTargets()
{
    // empty
    n_error("We do not support texture targets. This method should not be called!\n");
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture&
CEUIRenderer::createTexture()
{
    CEUITexture* tex = n_new(CEUITexture);
    this->textures.Append(tex);
    return (*tex);
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture&
CEUIRenderer::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
    CEUITexture* tex = n_new(CEUITexture);
    tex->loadFromFile(filename, resourceGroup);
    this->textures.Append(tex);
    return (*tex);
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture&
CEUIRenderer::createTexture(const CEGUI::Size& size)
{
    n_error("NOT yet implemented!\n");
    CEUITexture* tex = n_new(CEUITexture);
    this->textures.Append(tex);
    return (*tex);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyTexture(CEGUI::Texture& texture)
{
    IndexT texId = this->textures.FindIndex((CEUITexture*)&texture);
    n_assert(InvalidIndex != texId)
    this->textures[texId]->Unload();
    n_delete(this->textures[texId]);
    this->textures.EraseIndex(texId);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::destroyAllTextures()
{
    IndexT texId = 0;
    for (texId = 0; texId < this->textures.Size(); texId++)
    {
        this->textures[texId]->Unload();
        n_delete(this->textures[texId]);
    }
    this->textures.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::beginRendering()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CEUIRenderer::endRendering()
{
    // empty
}

} // namespace CEUI
