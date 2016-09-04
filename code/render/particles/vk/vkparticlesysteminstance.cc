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
	ParticleSystemInstanceBase::UpdateVertexStreams();
	Ptr<ParticleRenderer> particleRenderer = ParticleRenderer::Instance();

	// get the particle ring buffer from
	IndexT baseVertexIndex = particleRenderer->GetCurParticleIndex();

	float* ptr = (float*)particleRenderer->GetCurVertexPtr();
	Math::float4 tmp;
	IndexT i;
	SizeT num = this->particles.Size();

	// this is the size of our particles, the maximum is tripled because we use triple buffering
	for (i = 0; (i < num) && (particleRenderer->GetCurParticleIndex() < MaxNumRenderedParticles); i++)
	{
		const Particle& particle = this->particles[i];

		if (particle.relAge < 1.0f)
		{
			// NOTE: it's important to write in order here, since the writes
			// go to write-combined memory!
			particle.position.stream(ptr); ptr += 4;
			particle.stretchPosition.stream(ptr); ptr += 4;
			particle.color.stream(ptr); ptr += 4;
			particle.uvMinMax.stream(ptr); ptr += 4;
			tmp.set(particle.rotation, particle.size, particle.particleId, 0.0f);
			tmp.stream(ptr); ptr += 4;
			particleRenderer->AddCurParticleIndex(1);
		}
	}
	particleRenderer->SetCurVertexPtr(ptr);
	SizeT numVertices = particleRenderer->GetCurParticleIndex() - baseVertexIndex;
	this->renderInfo = ParticleRenderInfo(baseVertexIndex, numVertices);
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

		renderDevice->SetVertexLayout(particleRenderer->GetVertexLayout());
		renderDevice->SetPrimitiveGroup(particleRenderer->GetPrimitiveGroup());
		renderDevice->SetStreamVertexBuffer(0, particleRenderer->GetCornerVertexBuffer(), 0);
		renderDevice->SetStreamVertexBuffer(1, particleRenderer->GetParticleVertexBuffer(), 0);
		renderDevice->SetIndexBuffer(particleRenderer->GetCornerIndexBuffer());
		renderDevice->DrawIndexedInstanced(numParticles, baseVertexIndex);
	}
}

} // namespace Vulkan