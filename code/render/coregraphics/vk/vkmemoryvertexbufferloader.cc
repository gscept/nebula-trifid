//------------------------------------------------------------------------------
// vkmemoryvertexbufferloader.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkmemoryvertexbufferloader.h"
#include "coregraphics/vertexlayoutserver.h"
#include "coregraphics/vertexbuffer.h"
#include "vkrenderdevice.h"

using namespace CoreGraphics;
using namespace Resources;
namespace Vulkan
{

__ImplementClass(Vulkan::VkMemoryVertexBufferLoader, 'VKVO', Base::MemoryVertexBufferLoaderBase);

//------------------------------------------------------------------------------
/**
	FIXME: Go through the transfer queue if possible to update the vertex data
*/
bool
VkMemoryVertexBufferLoader::OnLoadRequested()
{
	n_assert(this->GetState() == Resource::Initial);
	n_assert(this->resource.isvalid());
	n_assert(!this->resource->IsAsyncEnabled());
	n_assert(this->numVertices > 0);
	if (VertexBuffer::UsageImmutable == this->usage)
	{
		n_assert(0 != this->vertexDataPtr);
		n_assert(0 < this->vertexDataSize);
	}
	SizeT vertexSize = VertexLayoutServer::Instance()->CalculateVertexSize(this->vertexComponents);
	
	// start by creating buffer
	VkBufferCreateInfo bufinfo =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		0,					// use for sparse buffers
		vertexSize * this->numVertices,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,				// can only be accessed from the creator queue,
		1,														// number of queues in family
		&VkRenderDevice::Instance()->renderQueueFamily			// array of queues belonging to family
	};

	VkBuffer buf;
	VkResult err = vkCreateBuffer(VkRenderDevice::dev, &bufinfo, NULL, &buf);
	n_assert(err == VK_SUCCESS);

	// allocate a device memory backing for this
	VkDeviceMemory mem;
	uint32_t alignedSize;
	VkRenderDevice::Instance()->AllocateBufferMemory(buf, mem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, alignedSize);

	// now bind memory to buffer
	err = vkBindBufferMemory(VkRenderDevice::dev, buf, mem, 0);
	n_assert(err == VK_SUCCESS);

	if (this->vertexDataPtr != 0)
	{
		// map memory so we can initialize it
		void* data;
		err = vkMapMemory(VkRenderDevice::dev, mem, 0, alignedSize, 0, &data);
		n_assert(err == VK_SUCCESS);
		n_assert(this->vertexDataSize <= (int32_t)alignedSize);
		memcpy(data, this->vertexDataPtr, this->vertexDataSize);
		vkUnmapMemory(VkRenderDevice::dev, mem);
	}

	Ptr<VertexLayout> vertexLayout = VertexLayout::Create();
	vertexLayout->SetStreamBuffer(0, buf);
	vertexLayout->Setup(this->vertexComponents);
	if (0 != this->vertexDataPtr)
	{
		n_assert((this->numVertices * vertexLayout->GetVertexByteSize()) == this->vertexDataSize);
	}

	// setup our resource object
	const Ptr<VertexBuffer>& res = this->resource.downcast<VertexBuffer>();
	n_assert(!res->IsLoaded());
	res->SetUsage(this->usage);
	res->SetAccess(this->access);
	res->SetSyncing(this->syncing);
	res->SetVertexLayout(vertexLayout);
	res->SetNumVertices(this->numVertices);
	res->SetVkBuffer(buf, mem);

	// invalidate setup data (because we don't own our data)
	this->vertexDataPtr = 0;
	this->vertexDataSize = 0;

	this->SetState(Resource::Loaded);
	return true;
}

} // namespace Vulkan