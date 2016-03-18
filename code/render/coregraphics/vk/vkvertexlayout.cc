//------------------------------------------------------------------------------
// vkvertexlayout.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkvertexlayout.h"
#include "vkrenderdevice.h"
#include "vktypes.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkVertexLayout, 'VKVL', Base::VertexLayoutBase);
//------------------------------------------------------------------------------
/**
*/
VkVertexLayout::VkVertexLayout()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkVertexLayout::~VkVertexLayout()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexLayout::Setup(const Util::Array<CoreGraphics::VertexComponent>& c)
{
	// call parent class
	Base::VertexLayoutBase::Setup(c);

	VkPipelineVertexInputStateCreateInfo info = 
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		NULL,
		0,
		this->components.Size(),
		NULL,
		this->components.Size(),
		NULL
	};

	// create binds
	this->binds = new VkVertexInputBindingDescription[this->components.Size()];
	this->attrs = new VkVertexInputAttributeDescription[this->components.Size()];

	IndexT curOffset[VkRenderDevice::MaxNumVertexStreams] = { 0 };

	IndexT compIndex;
	for (compIndex = 0; compIndex < this->components.Size(); compIndex++)
	{
		const CoreGraphics::VertexComponent& component = this->components[compIndex];
		VkVertexInputBindingDescription* bind = &this->binds[compIndex];
		VkVertexInputAttributeDescription* attr = &this->attrs[compIndex];

		bind->binding = component.GetStreamIndex();
		bind->inputRate = component.GetStrideType() == CoreGraphics::VertexComponent::PerInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
		bind->stride = component.GetStride();

		attr->binding = component.GetStreamIndex();
		attr->location = component.GetSemanticName();
		attr->format = VkTypes::AsVkVertexType(component.GetFormat());
		attr->offset = curOffset[component.GetStreamIndex()];
		curOffset[component.GetStreamIndex()] += component.GetByteSize();
	}

	this->vertexInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		NULL,
		0,
		this->components.Size(),
		this->binds,
		this->components.Size(),
		this->attrs
	};

	// finish up the info struct
	this->info.pVertexInputState = &this->vertexInfo;
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexLayout::Discard()
{
	VertexLayoutBase::Discard();
	delete[] this->binds;
	delete[] this->attrs;
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexLayout::Apply()
{
	VkRenderDevice::Instance()->SetVertexLayoutPipelineInfo(this->info);
}

} // namespace Vulkan