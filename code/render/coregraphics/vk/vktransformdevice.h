#pragma once
//------------------------------------------------------------------------------
/**
	Implements the transform device to manage object and camera transforms in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/transformdevicebase.h"
namespace Vulkan
{
class VkTransformDevice : public Base::TransformDeviceBase
{
	__DeclareClass(VkTransformDevice);
public:
	/// constructor
	VkTransformDevice();
	/// destructor
	virtual ~VkTransformDevice();

	/// set view matrix array
	void ApplyViewMatrixArray(const Math::matrix44* matrices, SizeT num);
private:
};
} // namespace Vulkan