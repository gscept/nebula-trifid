//------------------------------------------------------------------------------
// vkparticlesysteminstance.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkparticlesysteminstance.h"
#include "../particlerenderer.h"
#include "coregraphics/renderdevice.h"

using namespace CoreGraphics;
using namespace Particles;
namespace Vulkan
{

__ImplementClass(Vulkan::VkParticleSystemInstance, 'VKPI', Particles::ParticleSystemInstanceBase);
//------------------------------------------------------------------------------
/**
*/
VkParticleSystemInstance::VkParticleSystemInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkParticleSystemInstance::~VkParticleSystemInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkParticleSystemInstance::UpdateVertexStreams()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkParticleSystemInstance::Render()
{
	ParticleRenderer* particleRenderer = ParticleRenderer::Instance();
	n_assert(!particleRenderer->IsInAttach());
	RenderDevice* renderDevice = RenderDevice::Instance();
	SizeT numParticles = renderInfo.GetNumVertices();

	if (numParticles > 0)
	{
		// setup vertex buffers and index buffers for rendering
		IndexT baseVertexIndex = renderInfo.GetBaseVertexIndex();

		renderDevice->SetStreamVertexBuffer(0, particleRenderer->GetCornerVertexBuffer(), 0);
		renderDevice->SetStreamVertexBuffer(1, particleRenderer->GetParticleVertexBuffer(), 0);
		renderDevice->SetVertexLayout(particleRenderer->GetVertexLayout());
		renderDevice->SetIndexBuffer(particleRenderer->GetCornerIndexBuffer());
		renderDevice->SetPrimitiveGroup(particleRenderer->GetPrimitiveGroup());
		renderDevice->DrawIndexedInstanced(numParticles, baseVertexIndex);
	}
}

} // namespace Vulkan