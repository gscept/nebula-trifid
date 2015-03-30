#pragma once

#include "resources/resourceloader.h"

namespace Direct3D9
{
class D3D9MemoryTextureLoader : public Resources::ResourceLoader
{
	__DeclareClass(MemoryTextureLoader);
public:		
	void SetImageBuffer(const void* buffer, SizeT width, SizeT height, CoreGraphics::PixelFormat::Code format);		
	virtual bool OnLoadRequested();
private:
	IDirect3DTexture9* d3d9Texture;
};
}
