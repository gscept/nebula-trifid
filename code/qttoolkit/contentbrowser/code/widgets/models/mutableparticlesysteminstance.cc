//------------------------------------------------------------------------------
//  mutableparticlesysteminstance.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutableparticlesysteminstance.h"

namespace Particles
{
__ImplementClass(Particles::MutableParticleSystemInstance, 'MPSI', Particles::ParticleSystemInstance);

//------------------------------------------------------------------------------
/**
*/
MutableParticleSystemInstance::MutableParticleSystemInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableParticleSystemInstance::~MutableParticleSystemInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MutableParticleSystemInstance::UpdateEmitter(const EmitterAttrs& newAttrs, IndexT primGroupIndex, const Ptr<CoreGraphics::Mesh>& emitterMesh)
{
	bool wasPlaying = (ParticleSystemState::Playing & this->stateMask) || (ParticleSystemState::Playing & this->stateChangeMask);
	this->emitterAttrs = newAttrs;
	this->primGroupIndex = primGroupIndex;
	this->emitterMesh = emitterMesh;
	this->Discard();
	this->Setup(this->emitterMesh, this->primGroupIndex, this->emitterAttrs);
	this->Start();
}

} // namespace Particles