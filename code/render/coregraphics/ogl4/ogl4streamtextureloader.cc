//------------------------------------------------------------------------------
//  OGL4StreamTextureLoader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4streamtextureloader.h"
#include "image/imagefactory.h"
#include "coregraphics/texture.h"
#include "coregraphics/renderdevice.h"
#include "io/ioserver.h"
#include "coregraphics/ogl4/ogl4types.h"
#include <SOIL/SOIL.h>

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4StreamTextureLoader, 'O4TL', Resources::StreamResourceLoader);

using namespace Image;
using namespace CoreGraphics;
using namespace Resources;
using namespace IO;

//------------------------------------------------------------------------------
/**
    This method actually setups the Texture object from the data in the stream.
*/
bool
OGL4StreamTextureLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());
    n_assert(this->resource->IsA(Texture::RTTI));
    const Ptr<Texture>& res = this->resource.downcast<Texture>();
    n_assert(!res->IsLoaded());

    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
		void* srcData = stream->Map();
		uint srcDataSize = stream->GetSize();

		GLuint texture;
		unsigned int type = 0;
		unsigned int format = 0;
		unsigned int mips = 0;
		unsigned int bpp = 0;
		unsigned int data_type = 0;
		glGenTextures(1, &texture);
		SOIL_load_OGL_texture_from_memory((const unsigned char* const)srcData, srcDataSize, SOIL_LOAD_AUTO, texture, SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT, &type, &format, &mips, &bpp, &data_type);
		n_assert(GLSUCCESS);

		// convert format
		CoreGraphics::PixelFormat::Code nebFormat = OGL4Types::AsNebulaPixelFormat(format);
		GLint width, height;

		// setup texture appropriately
		if (type == GL_TEXTURE_2D)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
			glBindTexture(GL_TEXTURE_2D, 0);

			// calculate how many mips we will have	
			res->SetupFromOGL4Texture(texture, nebFormat, mips);
		}
		else if (type == GL_TEXTURE_CUBE_MAP)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
			glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_HEIGHT, &height);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			// calculate how many mips we will have	
			res->SetupFromOGL4CubeTexture(texture, nebFormat, mips);
		}
		else if (type == GL_TEXTURE_3D)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
			glBindTexture(GL_TEXTURE_3D, 0);

			// calculate how many mips we will have	
			res->SetupFromOGL4VolumeTexture(texture, nebFormat, mips);
		}

        stream->Unmap();
        stream->Close();
        return true;
    }
    return false;
}

} // namespace OpenGL4

