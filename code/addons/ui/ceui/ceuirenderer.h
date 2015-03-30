#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIRenderer

    The render module for crazy eddies GUI system.

    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/array.h"
#include "render/coregraphics/vertexbuffer.h"
#include "render/coregraphics/primitivegroup.h"
#include "render/coregraphics/shadervariable.h"
#include "cegui/include/CEGUIRenderer.h"
#include "cegui/include/CEGUISize.h"
#include "cegui/include/CEGUIVector.h"
#include "sui/ceui/ceuitexture.h"
#include "sui/ceui/ceuigeometrybuffer.h"
#include "cegui/include/CEGUIRenderTarget.h"


//------------------------------------------------------------------------------
namespace CEUI
{
class CEUIRenderer : public CEGUI::Renderer, public Core::RefCounted
{
    __DeclareClass(CEUIRenderer);
    __DeclareSingleton(CEUIRenderer);

public:
    /// constructor
    CEUIRenderer();
    /// destructor
    virtual ~CEUIRenderer();

    // implement cegui renderer interface

    // Returns the default rendering root for the renderer
    virtual CEGUI::RenderingRoot& getDefaultRenderingRoot();
    // Create a new GeometryBuffer and return a reference to it
    virtual CEGUI::GeometryBuffer& createGeometryBuffer();
    // Destroy a GeometryBuffer that was returned when calling the createGeometryBuffer function
    virtual void destroyGeometryBuffer(const CEGUI::GeometryBuffer& buffer);
    // Destroy all GeometryBuffer objects created by this Renderer
    virtual void destroyAllGeometryBuffers();
    // Create a TextureTarget that can be used to cache imagery
    virtual CEGUI::TextureTarget* createTextureTarget();
    // Function that cleans up TextureTarget objects created with the createTextureTarget function
    virtual void destroyTextureTarget(CEGUI::TextureTarget* target);
    // Destory all TextureTarget objects created by this Renderer
    virtual void destroyAllTextureTargets();
    // Create a 'null' Texture object
    virtual CEGUI::Texture& createTexture();
    // Create a Texture object using the given image file
    virtual CEGUI::Texture& createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup);
    // Create a Texture object with the given pixel dimensions as specified by size
    virtual CEGUI::Texture& createTexture(const CEGUI::Size& size);
    // Destroy a Texture object that was previously created by calling the createTexture functions
    virtual void destroyTexture(CEGUI::Texture& texture);
    // Destroy all Texture objects created by this Renderer
    virtual void destroyAllTextures();
    // Perform any operations required to put the system into a state ready for rendering operations to begin
    virtual void beginRendering();
    // Perform any operations required to finalise rendering
    virtual void endRendering();
    // Set the size of the display or host window in pixels for this Renderer object
    virtual void setDisplaySize(const CEGUI::Size& size);
    // Return the size of the display or host window in pixels  
    virtual const CEGUI::Size& getDisplaySize() const;
    // Return the resolution of the display or host window in dots per inch
    virtual const CEGUI::Vector2& getDisplayDPI() const;
    // Return the pixel size of the maximum supported texture
    virtual CEGUI::uint getMaxTextureSize() const;
    // Return identification string for the renderer module
    virtual const CEGUI::String& getIdentifierString() const;

    /// Sets the shader instance
    void SetShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& shdInst);
    /// Returns the shader instance
    const Ptr<CoreGraphics::ShaderInstance>& GetShaderInstance() const;
    /// Returns the gui texture semantic
    CoreGraphics::ShaderVariable::Semantic GetTextureSemantic() const;
    /// Check if all textures are loaded (not only the placeholder)
    bool CheckAllTextureReady() const;

protected:
    static CEGUI::String identifier;
    CEGUI::Size displaySize;
    CEGUI::Vector2 displayDPI;
    CEGUI::RenderingRoot* defaultRoot;
    CEGUI::RenderTarget* defaultTarget;
    Util::Array<CEUI::CEUIGeometryBuffer*> geometryBuffers;
    Util::Array<CEUI::CEUITexture*> textures;

    CoreGraphics::ShaderVariable::Semantic guiTextureSemantic;
    Ptr<CoreGraphics::ShaderInstance> guiShaderInst;
};

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::RenderingRoot&
CEUIRenderer::getDefaultRenderingRoot()
{
    return (*this->defaultRoot);
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIRenderer::setDisplaySize(const CEGUI::Size& size)
{
    this->displaySize = size;
    // FIXME: This is probably not the right thing to do in all cases.
    CEGUI::Rect area(this->defaultTarget->getArea());
    area.setSize(size);
    this->defaultTarget->setArea(area);
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Size&
CEUIRenderer::getDisplaySize() const
{
    return this->displaySize;
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::Vector2&
CEUIRenderer::getDisplayDPI() const
{
    return this->displayDPI;
}

//------------------------------------------------------------------------------
/**
*/
inline CEGUI::uint
CEUIRenderer::getMaxTextureSize() const
{
    return 1024;
}

//------------------------------------------------------------------------------
/**
*/
inline const CEGUI::String&
CEUIRenderer::getIdentifierString() const
{
    return identifier;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CEUIRenderer::SetShaderInstance(const Ptr<CoreGraphics::ShaderInstance>& shdInst)
{
    this->guiShaderInst = shdInst;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderInstance>&
CEUIRenderer::GetShaderInstance() const
{
    n_assert(this->guiShaderInst.isvalid());
    return this->guiShaderInst;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::ShaderVariable::Semantic
CEUIRenderer::GetTextureSemantic() const
{
    return this->guiTextureSemantic;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CEUIRenderer::CheckAllTextureReady() const
{
    IndexT texId;
    for (texId = 0; texId < this->textures.Size(); texId++)
    {
        if (this->textures[texId]->GetManagedTexture()->IsPlaceholder())
        {
			//FIXME
            return false;
        }
        else
        {
            this->textures[texId]->UpdateTexture();
        }
    }
    return true;
}

} // namespace CEUI
//------------------------------------------------------------------------------