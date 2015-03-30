//------------------------------------------------------------------------------
//  ogl4streamshaderloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effectfactory.h"
#include "coregraphics/ogl4/ogl4streamshaderloader.h"
#include "coregraphics/ogl4/ogl4shader.h"
#include "coregraphics/ogl4/ogl4renderdevice.h"
#include "coregraphics/ogl4/ogl4shaderserver.h"
#include "io/ioserver.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4StreamShaderLoader, 'D1SL', Resources::StreamResourceLoader);

using namespace Resources;
using namespace CoreGraphics;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
bool
OGL4StreamShaderLoader::CanLoadAsync() const
{
    // no asynchronous loading supported for shader
    return false;
}

//------------------------------------------------------------------------------
/**
    Loads a uncompiled shader files from a stream into 2-5 shader programs which are then compiled and linked
*/
bool
OGL4StreamShaderLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());
    n_assert(this->resource->IsA(OGL4Shader::RTTI));
    const Ptr<OGL4Shader>& res = this->resource.downcast<OGL4Shader>();
    n_assert(!res->IsLoaded());
    
    // map stream to memory
    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {		
        void* srcData = stream->Map();
        uint srcDataSize = stream->GetSize();

		AnyFX::Effect* effect = AnyFX::EffectFactory::Instance()->CreateEffectFromMemory(srcData, srcDataSize);

		// catch any potential GL error coming from AnyFX
		n_assert(GLSUCCESS);
		if (!effect)
		{
			n_error("OGL4StreamShaderLoader: failed to load shader '%s'!", 
				res->GetResourceId().Value());
			return false;
		}
		
		res->ogl4Effect = effect;
		res->shaderName = res->GetResourceId().AsString();

		return true;
	}
    return false;
}


} // namespace OpenGL4

