//------------------------------------------------------------------------------
// vkcmdevent.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkcmdevent.h"
#include "coregraphics/vk/vkrenderdevice.h"
namespace Vulkan
{

__ImplementClass(Vulkan::VkCmdEvent, 'VKEV', Base::EventBase);
//------------------------------------------------------------------------------
/**
*/
VkCmdEvent::VkCmdEvent()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkCmdEvent::~VkCmdEvent()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdEvent::Setup()
{
	VkEventCreateInfo info =
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		NULL,
		0
	};
	VkResult res = vkCreateEvent(VkRenderDevice::dev, &info, NULL, &this->event);

	// if this event should be created signaled, just set it directly
	if (this->createSignaled) vkSetEvent(VkRenderDevice::dev, this->event);
}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdEvent::Signal()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdEvent::Wait()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkCmdEvent::Reset()
{

}

} // namespace Vulkan