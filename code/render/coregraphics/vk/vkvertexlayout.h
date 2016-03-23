#pragma once
//------------------------------------------------------------------------------
/**
	Implements a vertex layout object used to construct a Vulkan pipeline.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/vertexlayoutbase.h"
#include "vkrenderdevice.h"
namespace Vulkan
{
class VkVertexLayout : public Base::VertexLayoutBase
{
	__DeclareClass(VkVertexLayout);
public:
	/// constructor
	VkVertexLayout();
	/// destructor
	virtual ~VkVertexLayout();

	/// setup the vertex layout
	void Setup(const Util::Array<CoreGraphics::VertexComponent>& c);
	/// discard the vertex layout object
	void Discard();

	/// set the vertex buffer associated with the stream index
	void SetStreamBuffer(IndexT streamIndex, VkBuffer vertexBuffer);

	/// applies layout before rendering
	void Apply();
private:
	VkGraphicsPipelineCreateInfo info;
	VkPipelineVertexInputStateCreateInfo vertexInfo;

	VkBuffer vertexStreams[VkRenderDevice::MaxNumVertexStreams];

	Util::FixedArray<VkVertexInputBindingDescription> binds;
	Util::FixedArray<VkVertexInputAttributeDescription> attrs;
};


//------------------------------------------------------------------------------
/**
*/
inline void
VkVertexLayout::SetStreamBuffer(IndexT streamIndex, VkBuffer vertexBuffer)
{
	this->vertexStreams[streamIndex] = vertexBuffer;
}

} // namespace Vulkan