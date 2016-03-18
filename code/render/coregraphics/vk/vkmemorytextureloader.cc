//------------------------------------------------------------------------------
// vkmemorytextureloader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkmemorytextureloader.h"
#include "../texture.h"

using namespace CoreGraphics;
using namespace Resources;
namespace Vulkan
{

__ImplementClass(Vulkan::VkMemoryTextureLoader, 'VKTO', Resources::ResourceLoader);
//------------------------------------------------------------------------------
/**
*/
void
VkMemoryTextureLoader::SetImageBuffer(const void* buffer, SizeT width, SizeT height, CoreGraphics::PixelFormat::Code format)
{
	// TODO: IMPLEMENT ME
}

//------------------------------------------------------------------------------
/**
*/
bool
VkMemoryTextureLoader::OnLoadRequested()
{
	n_assert(this->resource->IsA(Texture::RTTI));
	n_assert(this->image != 0);
	const Ptr<Texture>& res = this->resource.downcast<Texture>();
	n_assert(!res->IsLoaded());
	//res->SetupFromVkTexture(this->image, mem, this->format);
	res->SetState(Resource::Loaded);
	this->SetState(Resource::Loaded);
	return true;
}

} // namespace Vulkan