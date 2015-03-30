#include "stdneb.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/texture.h"
#include "coregraphics/renderdevice.h"
#include "MemoryTextureLoader.h"

#include "win360/d3d9types.h"
using namespace Win360;
using namespace Resources;

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9MemoryTextureLoader,'D9TL',Resources::ResourceLoader);

//------------------------------------------------------------------------------
/**
*/
void 
D3D9MemoryTextureLoader::SetImageBuffer(const void* buffer, SizeT width, SizeT height, CoreGraphics::PixelFormat::Code format)
{
	HRESULT hr;
	IDirect3DDevice9* d3d9Device = RenderDevice::Instance()->GetDirect3DDevice();
	n_assert(0 != d3d9Device);


	d3d9Texture = 0;
	hr = D3DXCreateTexture(d3d9Device,width,height,5,D3DUSAGE_DYNAMIC|D3DUSAGE_AUTOGENMIPMAP,D3D9Types::AsD3D9PixelFormat(format),D3DPOOL_DEFAULT,&d3d9Texture);

	if (FAILED(hr))
	{
		n_error("MemoryTextureLoader: D3DXCreateTexture() failed");
		return;
	}
	IDirect3DBaseTexture9 *baseTexture;
	hr=d3d9Texture->QueryInterface(IID_IDirect3DBaseTexture9, (void**) &(baseTexture));

	D3DLOCKED_RECT d3dLockedRect={ 0 };

	hr=d3d9Texture->LockRect(0, &d3dLockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE);

	for(int y=0; y<height; y++)
	{
		// pitch is measured in bytes
		memcpy(((unsigned char *)d3dLockedRect.pBits)+(y*d3dLockedRect.Pitch),
			((unsigned int*)buffer)+(y*width), sizeof(unsigned int)*width);
	}
	d3d9Texture->UnlockRect(0);
	baseTexture->SetAutoGenFilterType(D3DTEXF_LINEAR);
	baseTexture->GenerateMipSubLevels();

}

//------------------------------------------------------------------------------
/**
*/
bool 
D3D9MemoryTextureLoader::OnLoadRequested()
{
	n_assert(this->resource->IsA(Texture::RTTI));
	n_assert(this->d3d9Texture != NULL);
	const Ptr<Texture>& res = this->resource.downcast<Texture>();
	n_assert(!res->IsLoaded());
	res->SetupFromD3D9Texture(d3d9Texture);
	res->SetState(Resource::Loaded);
	SetState(Resource::Loaded);
	return true;		
}
}  // namespace Direct3D9

