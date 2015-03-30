//------------------------------------------------------------------------------
//  ceuiserver.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuitexture.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/memorytextureloader.h"
#include "resources/resourceloader.h"
#include "resources/resourcemanager.h"
#include "io/assignregistry.h"

namespace CEUI
{
SizeT CEUITexture::texNum = 0;

using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
CEUITexture::CEUITexture() :
    texelScaling(0.0f, 0.0f),
    textureSize(0.0f, 0.0f)
{
    // initialize dummy texture for "no-textured" objects
    Resources::ResourceId dummyResId = "tex:system/ui_placeholder"NEBULA3_TEXTURE_EXTENSION;
    this->texture = ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, dummyResId).downcast<ManagedTexture>();  
    this->UpdateTextureSize();
}

//------------------------------------------------------------------------------
/**
*/
CEUITexture::CEUITexture(const CEGUI::Size& size) :
    texelScaling(0.0f, 0.0f),
    textureSize(0.0f, 0.0f)
{
    n_error("NOT yet implemented!\n");
    this->UpdateTextureSize();
    this->UpdateCachedScaleValues();
}

//------------------------------------------------------------------------------
/**
*/
CEUITexture::CEUITexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup) :
    texelScaling(0.0f, 0.0f),
    textureSize(0.0f, 0.0f)
{
    this->loadFromFile(filename, resourceGroup);
}

//------------------------------------------------------------------------------
/**
*/
CEUITexture::~CEUITexture()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CEUITexture::loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
    // build a full resource path
    Util::String path;
    path.Format("ceui:%s/%s", resourceGroup.c_str(), filename.c_str());
    path = IO::AssignRegistry::Instance()->ResolveAssignsInString(path);
    this->texture = ResourceManager::Instance()->CreateManagedResource(CoreGraphics::Texture::RTTI, path).downcast<ManagedTexture>();      
}

//------------------------------------------------------------------------------
/**
*/
void
CEUITexture::loadFromMemory(const void* buffer, const CEGUI::Size& buffer_size, PixelFormat pixel_format)
{
	
	    // create texture memory loader
    Ptr<CoreGraphics::MemoryTextureLoader> memoryTextureLoader = CoreGraphics::MemoryTextureLoader::Create();

    // select pixel format
    switch (pixel_format) 
    {
    case PF_RGB:
        {
            memoryTextureLoader->SetImageBuffer(buffer, (SizeT)buffer_size.d_width, (SizeT)buffer_size.d_height, CoreGraphics::PixelFormat::A8R8G8B8);
        }
        break;
    case PF_RGBA:
        {
            memoryTextureLoader->SetImageBuffer(buffer, (SizeT)buffer_size.d_width, (SizeT)buffer_size.d_height, CoreGraphics::PixelFormat::A8R8G8B8);
        }
        break;
    }  

    Util::String dummyResId;
    dummyResId.Format("CeuiMemoryTexture_%d", CEUITexture::texNum);

    this->texture = ResourceManager::Instance()->CreateManagedResource(
        CoreGraphics::Texture::RTTI, dummyResId, memoryTextureLoader.downcast<ResourceLoader>()).downcast<Resources::ManagedTexture>();      

    this->UpdateTextureSize();
    this->UpdateCachedScaleValues();

    CEUITexture::texNum++;
	
}

//------------------------------------------------------------------------------
/**
*/
void
CEUITexture::saveToMemory(void* buffer)
{
    n_error("Do NOT use this method!\n");
    n_assert(this->texture.isvalid());
    const Ptr<CoreGraphics::Texture> t = this->texture->GetTexture();
    CoreGraphics::Texture::MapInfo mapInfo;
    t->Map(0, Base::ResourceBase::MapWriteDiscard, mapInfo);
    Memory::Copy(buffer, mapInfo.data, sizeof(unsigned int) * (t->GetWidth() * t->GetHeight()));
    t->Unmap(0);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUITexture::UpdateTextureSize()
{
    n_assert(this->texture.isvalid());
    this->textureSize.d_width = (float)this->texture->GetTexture()->GetWidth();
    this->textureSize.d_height = (float)this->texture->GetTexture()->GetHeight();
    this->originalDataSize.d_width = this->textureSize.d_width;
    this->originalDataSize.d_height = this->textureSize.d_height;
}

//------------------------------------------------------------------------------
/**
*/
void 
CEUITexture::UpdateCachedScaleValues()
{
    //
    // calculate what to use for x scale
    //
    const float orgW = this->originalDataSize.d_width;
    const float texW = this->textureSize.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    this->texelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = this->originalDataSize.d_height;
    const float texH = this->textureSize.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    this->texelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//------------------------------------------------------------------------------
/**
*/
void
CEUITexture::Unload()
{
    n_assert(this->texture.isvalid());
    ResourceManager::Instance()->DiscardManagedResource(this->texture.cast<Resources::ManagedResource>());
    this->texture = 0;
    this->textureSize = CEGUI::Size(0,0);
    this->originalDataSize = CEGUI::Size(0,0);
    this->texelScaling = CEGUI::Vector2(0.0f,0.0f);
}

} // namespace CEUI
