//------------------------------------------------------------------------------
//  d3d11textureinfo.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"

#ifdef __OGL4__
#include "coregraphics/pixelformat.h"
#include "resources/ogl4streaming/ogl4textureinfo.h"
#include "coregraphics/ogl4/ogl4types.h"
#include "coregraphics/texture.h"
#include "util/string.h"

using namespace Util;
using namespace CoreGraphics;

namespace Resources
{
//------------------------------------------------------------------------------
/**
*/
OGL4TextureInfo::OGL4TextureInfo(void) :
    width(0),
    height(0),
    depth(0),
    numMipLevels(0),
    pixelFormat(PixelFormat::DXT1),
    textureType(Texture::Texture2D)
{}

//------------------------------------------------------------------------------
/**
*/
OGL4TextureInfo::OGL4TextureInfo(const OGL4TextureInfo* info) :
    ResourceInfo((const ResourceInfo*)(info)),
    width(info->width),
    height(info->height),
    depth(info->depth),
    numMipLevels(info->numMipLevels),
    pixelFormat(info->pixelFormat),
    textureType(info->textureType)
{}

//------------------------------------------------------------------------------
/**
*/
void
OGL4TextureInfo::SetInfo(ILint image)
{
	// bind this image to IL
	ilBindImage(image);
	this->width = ilGetInteger(IL_IMAGE_WIDTH);
	this->height = ilGetInteger(IL_IMAGE_HEIGHT);
	this->depth = ilGetInteger(IL_IMAGE_DEPTH);

	GLenum format = ilGetInteger(IL_IMAGE_FORMAT);
    this->numMipLevels = ilGetInteger(IL_NUM_MIPMAPS);
	
    this->pixelFormat = OpenGL4::OGL4Types::AsNebulaPixelFormat(format);

	GLenum imageType = ilGetInteger(IL_IMAGE_TYPE);
    switch (imageType)
    {
    case GL_TEXTURE_2D:
		this->textureType = Texture::Texture2D;
		break;
	case GL_TEXTURE_CUBE_MAP:
		this->textureType = Texture::TextureCube;
		break;
	case GL_TEXTURE_3D:
        this->textureType = Texture::Texture3D;
        break;
    default:
        n_error("unknown texture-type: %i", imageType);
        break;
    }
}
} // namespace Resources
//------------------------------------------------------------------------------

#endif