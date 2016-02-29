#pragma once
//------------------------------------------------------------------------------
/**
	Texture creator class which uses texture information to construct a Vulkan texture.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "resources/streaming/resourcecreator.h"
namespace Vulkan
{
class VkTextureCreator : public Resources::ResourceCreator
{
	__DeclareClass(VkTextureCreator);
public:
	/// constructor
	VkTextureCreator();
	/// destructor
	virtual ~VkTextureCreator();
private:
};
} // namespace Vulkan