//------------------------------------------------------------------------------
// vkpass.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkpass.h"
#include "vkrenderdevice.h"
#include "vktypes.h"
#include "../rendertexture.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkPass, 'VKFR', Base::PassBase);
//------------------------------------------------------------------------------
/**
*/
VkPass::VkPass()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkPass::~VkPass()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkPass::Setup()
{
	// setup base class
	PassBase::Setup();

	Util::FixedArray<VkSubpassDescription> subpassDescs;
	Util::FixedArray<Util::FixedArray<VkAttachmentReference>> subpassReferences;
	Util::FixedArray<Util::FixedArray<VkAttachmentReference>> subpassInputs;
	Util::FixedArray<Util::FixedArray<uint32_t>> subpassPreserves;
	Util::FixedArray<Util::FixedArray<VkAttachmentReference>> subpassResolves;
	Util::FixedArray<VkAttachmentReference> subpassDepthStencils;
	Util::Array<VkSubpassDependency> subpassDeps;

	// resize subpass contents
	subpassDescs.Resize(this->subpasses.Size());
	subpassReferences.Resize(this->subpasses.Size());
	subpassInputs.Resize(this->subpasses.Size());
	subpassPreserves.Resize(this->subpasses.Size());
	subpassResolves.Resize(this->subpasses.Size());
	subpassDepthStencils.Resize(this->subpasses.Size());

	IndexT i;
	for (i = 0; i < this->subpasses.Size(); i++)
	{
		const PassBase::Subpass& subpass = this->subpasses[i];

		VkSubpassDescription& vksubpass = subpassDescs[i];
		vksubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		// get references to fixed arrays
		Util::FixedArray<VkAttachmentReference>& references = subpassReferences[i];
		Util::FixedArray<VkAttachmentReference>& inputs = subpassInputs[i];
		Util::FixedArray<uint32_t>& preserves = subpassPreserves[i];
		Util::FixedArray<VkAttachmentReference>& resolves = subpassResolves[i];

		// resize arrays straight away since we already know the size
		references.Resize(this->colorAttachments.Size());
		inputs.Resize(subpass.inputs.Size());
		preserves.Resize(this->colorAttachments.Size() - subpass.attachments.Size());
		if (subpass.resolve) resolves.Resize(subpass.attachments.Size());

		// if subpass binds depth, the slot for the depth-stencil buffer is color attachments + 1
		if (subpass.bindDepth)
		{
			VkAttachmentReference& ds = subpassDepthStencils[i];
			ds.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			ds.attachment = this->colorAttachments.Size() + 1;
			vksubpass.pDepthStencilAttachment = &ds;
		}
		else
		{
			VkAttachmentReference& ds = subpassDepthStencils[i];
			ds.layout = VK_IMAGE_LAYOUT_UNDEFINED;
			ds.attachment = VK_ATTACHMENT_UNUSED;
			vksubpass.pDepthStencilAttachment = &ds;
		}

		IndexT idx = 0;
		IndexT j;
		for (j = 0; j < this->colorAttachments.Size(); j++)
		{
			// if we can find the attachment in the subpass, use it, otherwise bind it as unused
			if (subpass.attachments.FindIndex(j) != InvalidIndex)
			{
				VkAttachmentReference& ref = references[j];
				ref.attachment = subpass.attachments[j];
				ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				if (subpass.resolve) resolves[j] = ref;
			}
			else
			{
				VkAttachmentReference& ref = references[j];
				ref.attachment = VK_ATTACHMENT_UNUSED;
				ref.layout = VK_IMAGE_LAYOUT_UNDEFINED;
				preserves[idx++] = j;
			}			
		}

		for (j = 0; j < subpass.inputs.Size(); j++)
		{
			VkAttachmentReference& ref = inputs[j];
			ref.attachment = subpass.inputs[j];
			ref.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		for (j = 0; j < subpass.dependencies.Size(); j++)
		{
			VkSubpassDependency dep;
			dep.srcSubpass = i;
			dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dep.dstSubpass = subpass.dependencies[j];
			dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			dep.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			subpassDeps.Append(dep);
		}
		
		// set color attachments
		vksubpass.colorAttachmentCount = references.Size();
		vksubpass.pColorAttachments = references.Begin();

		// if we have subpass inputs, use them
		if (inputs.Size() > 0)
		{
			vksubpass.inputAttachmentCount = inputs.Size();
			vksubpass.pInputAttachments = inputs.Begin();
		}
		else
		{
			vksubpass.inputAttachmentCount = 0;
		}
		
		// the rest are automatically preserve
		if (preserves.Size() > 0)
		{ 
			vksubpass.preserveAttachmentCount = preserves.Size();
			vksubpass.pPreserveAttachments = preserves.Begin();
		}
		else
		{
			vksubpass.preserveAttachmentCount = 0;
		}
	}

	VkAttachmentLoadOp loadOps[] =
	{
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_LOAD_OP_LOAD,
	};

	VkAttachmentStoreOp storeOps[] =
	{
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
	};

	SizeT numUsedAttachments = this->colorAttachments.Size();
	Util::FixedArray<VkAttachmentDescription> attachments;
	attachments.Resize(this->colorAttachments.Size() + 1);
	for (i = 0; i < this->colorAttachments.Size(); i++)
	{
		VkFormat fmt = VkTypes::AsVkFormat(this->colorAttachments[i]->GetPixelFormat());
		VkAttachmentDescription& attachment = attachments[i];
		IndexT loadIdx = this->colorAttachmentFlags[i] & Load ? 2 : this->colorAttachmentFlags[i] & Clear ? 1 : 0;
		IndexT storeIdx = this->colorAttachmentFlags[i] & Store ? 1 : 0;
		attachment.flags = 0;
		attachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachment.format = fmt;
		attachment.loadOp = loadOps[loadIdx];
		attachment.storeOp = storeOps[storeIdx];
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.samples = this->colorAttachments[i]->GetEnableMSAA() ? VK_SAMPLE_COUNT_16_BIT : VK_SAMPLE_COUNT_1_BIT;
	}

	// use depth stencil attachments if pointer is not null
	if (this->depthStencilAttachment.isvalid())
	{
		VkAttachmentDescription* attachment = attachments.End();
		IndexT loadIdx = this->depthStencilFlags & Load ? 2 : this->depthStencilFlags & Clear ? 1 : 0;
		IndexT storeIdx = this->depthStencilFlags & Store ? 1 : 0;
		IndexT stencilLoadIdx = this->depthStencilFlags & LoadStencil ? 2 : this->depthStencilFlags & ClearStencil ? 1 : 0;
		IndexT stencilStoreIdx = this->depthStencilFlags & StoreStencil ? 1 : 0;
		attachment->flags = 0;
		attachment->initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		attachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		attachment->format = VkTypes::AsVkFormat(this->depthStencilAttachment->GetPixelFormat());
		attachment->loadOp = loadOps[loadIdx];
		attachment->storeOp = storeOps[storeIdx];
		attachment->stencilLoadOp = loadOps[stencilLoadIdx];
		attachment->stencilStoreOp = storeOps[stencilStoreIdx];
		attachment->samples = this->depthStencilAttachment->GetEnableMSAA() ? VK_SAMPLE_COUNT_16_BIT : VK_SAMPLE_COUNT_1_BIT;
		numUsedAttachments++;
	}
	
	// create render pass
	VkRenderPassCreateInfo info =
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		NULL,
		0,
		numUsedAttachments,
		attachments.Begin(),
		subpassDescs.Size(),
		subpassDescs.Begin(),
		subpassDeps.Size(),
		subpassDeps.Begin()
	};

	VkResult res = vkCreateRenderPass(VkRenderDevice::dev, &info, NULL, &this->pass);
	n_assert(res == VK_SUCCESS);

	// gather image views
	SizeT width = 0;
	SizeT height = 0;
	SizeT layers = 0;
	Util::FixedArray<VkImageView> images;
	images.Resize(this->colorAttachments.Size() + this->depthStencilAttachment.isvalid() ? 1 : 0);
	for (i = 0; i < this->colorAttachments.Size(); i++)
	{
		images[i] = this->colorAttachments[i]->GetVkImageView();
		width = Math::n_max(width, this->colorAttachments[i]->GetWidth());
		height = Math::n_max(height, this->colorAttachments[i]->GetHeight());
		layers = Math::n_max(layers, this->colorAttachments[i]->GetDepth());
	}
	if (this->depthStencilAttachment.isvalid()) images[i] = this->depthStencilAttachment->GetVkImageView();

	// create framebuffer
	VkFramebufferCreateInfo fbInfo =
	{
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		NULL,
		0,
		this->pass,
		images.Size(),
		images.Begin(),
		width,
		height,
		layers
	};
	res = vkCreateFramebuffer(VkRenderDevice::dev, &fbInfo, NULL, &this->framebuffer);
	n_assert(res == VK_SUCCESS);

	// setup info
	this->framebufferPipelineInfo.renderPass = this->pass;
	this->framebufferPipelineInfo.subpass = 0;
	this->framebufferPipelineInfo.pViewportState = &this->viewportInfo;
}

//------------------------------------------------------------------------------
/**
*/
void
VkPass::Discard()
{
	vkDestroyRenderPass(VkRenderDevice::dev, this->pass, NULL);
	vkDestroyFramebuffer(VkRenderDevice::dev, this->framebuffer, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkPass::Begin()
{
	PassBase::Begin();
	this->framebufferPipelineInfo.subpass = this->currentSubpass;
}

//------------------------------------------------------------------------------
/**
*/
void
VkPass::NextSubpass()
{
	PassBase::NextSubpass();
	this->framebufferPipelineInfo.subpass = this->currentSubpass;
}

//------------------------------------------------------------------------------
/**
*/
void
VkPass::End()
{
	PassBase::End();
}

} // namespace Vulkan