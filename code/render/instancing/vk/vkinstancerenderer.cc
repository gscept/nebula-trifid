//------------------------------------------------------------------------------
// vkinstancerenderer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkinstancerenderer.h"
#include "coregraphics/shaderserver.h"

using namespace CoreGraphics;
namespace Vulkan
{

__ImplementClass(Vulkan::VkInstanceRenderer, 'VKIR', Base::InstanceRendererBase);
//------------------------------------------------------------------------------
/**
*/
VkInstanceRenderer::VkInstanceRenderer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkInstanceRenderer::~VkInstanceRenderer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkInstanceRenderer::Setup()
{
	InstanceRendererBase::Setup();

	Ptr<CoreGraphics::Shader> sharedShader = CoreGraphics::ShaderServer::Instance()->GetSharedShader();
	this->instancingBuffer = ConstantBuffer::Create();
	this->instancingBuffer->SetupFromBlockInShader(sharedShader, "InstanceBlock", 16);
	this->instancingBlockVar = sharedShader->GetVariableByName("InstanceBlock");
	this->instancingBlockVar->SetBufferHandle(this->instancingBuffer->GetHandle());

	this->modelArrayVar = this->instancingBuffer->GetVariableByName("ModelArray");
	//this->modelViewArrayVar = this->instancingBuffer->GetVariableByName("ModelViewArray");
	//this->modelViewProjectionArrayVar = this->instancingBuffer->GetVariableByName("ModelViewProjectionArray");
	this->idArrayVar = this->instancingBuffer->GetVariableByName("IdArray");
}

//------------------------------------------------------------------------------
/**
*/
void
VkInstanceRenderer::Close()
{
	this->modelArrayVar = 0;
	//this->modelViewArrayVar = 0;
	//this->modelViewProjectionArrayVar = 0;
	this->idArrayVar = 0;
	this->instancingBlockVar = 0;
	this->instancingBuffer->Discard();
	this->instancingBuffer = 0;

	InstanceRendererBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
VkInstanceRenderer::Render(const SizeT multiplier)
{

}

} // namespace Vulkan