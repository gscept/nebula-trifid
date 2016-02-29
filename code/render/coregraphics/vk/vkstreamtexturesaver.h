#pragma once
//------------------------------------------------------------------------------
/**
	Implements a method to save a texture from Vulkan to stream.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/streamtexturesaverbase.h"
namespace Vulkan
{
	class VkStreamTextureSaver : public Base::StreamTextureSaverBase
{
	__DeclareClass(VkStreamTextureSaver);
public:
	/// constructor
	VkStreamTextureSaver();
	/// destructor
	virtual ~VkStreamTextureSaver();
private:
};
} // namespace Vulkan