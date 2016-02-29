//------------------------------------------------------------------------------
// vktransformdevice.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vktransformdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkTransformDevice, 'VKTD', Base::TransformDeviceBase);
//------------------------------------------------------------------------------
/**
*/
VkTransformDevice::VkTransformDevice()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkTransformDevice::~VkTransformDevice()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkTransformDevice::ApplyViewMatrixArray(const Math::matrix44* matrices, SizeT num)
{
	// TODO: implement me!
}

} // namespace Vulkan