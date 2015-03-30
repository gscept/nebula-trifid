//------------------------------------------------------------------------------
//  ogl4texturecreator.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ogl4texturecreator.h"
#include "resources/streaming/textureinfo.h"
#include "coregraphics/texture.h"

namespace Resources
{
__ImplementClass(Resources::OGL4TextureCreator, 'WTCR', Resources::ResourceCreator);

//------------------------------------------------------------------------------
/**
*/
OGL4TextureCreator::OGL4TextureCreator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4TextureCreator::~OGL4TextureCreator()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
OGL4TextureCreator::CreateResource(const TextureInfo* texInfo)
{
    n_assert(0 != texInfo);
    Ptr<CoreGraphics::Texture> returnResource = CoreGraphics::Texture::Create();
    if (CoreGraphics::Texture::Texture2D == texInfo->GetType())
    {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// create texture
		glTexImage2D(GL_TEXTURE_2D, 
				0, 
				OpenGL4::OGL4Types::AsOGL4PixelComponents(texInfo->GetPixelFormat()), 
				texInfo->GetWidth(), 
				texInfo->GetHeight(), 
				GL_FALSE, 
				OpenGL4::OGL4Types::AsOGL4PixelFormat(texInfo->GetPixelFormat()),
				OpenGL4::OGL4Types::AsOGL4PixelType(texInfo->GetPixelFormat()),
				NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
        returnResource->SetupFromOGL4Texture(texture, texInfo->GetPixelFormat(), false);
    }
	else if (CoreGraphics::Texture::Texture3D == texInfo->GetType())
    {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_3D, texture);

		// create t exture
		glTexImage3D(GL_TEXTURE_3D, 
			0, 
			OpenGL4::OGL4Types::AsOGL4PixelComponents(texInfo->GetPixelFormat()), 
			texInfo->GetWidth(), 
			texInfo->GetHeight(),
			texInfo->GetDepth(),
			GL_FALSE, 
			OpenGL4::OGL4Types::AsOGL4PixelFormat(texInfo->GetPixelFormat()),
			OpenGL4::OGL4Types::AsOGL4PixelType(texInfo->GetPixelFormat()),
			NULL);

		glBindTexture(GL_TEXTURE_3D, 0);
		returnResource->SetupFromOGL4VolumeTexture(texture, texInfo->GetPixelFormat(), false);
    }
    returnResource->SetAsyncEnabled(true);
    returnResource->SetResourceId("not set");
    return Ptr<Resource>(returnResource);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Resource>
OGL4TextureCreator::CreateResource(const ResourceInfo* resInfo)
{
    return this->CreateResource((const TextureInfo*)resInfo);
}
} // namespace Resources
//------------------------------------------------------------------------------