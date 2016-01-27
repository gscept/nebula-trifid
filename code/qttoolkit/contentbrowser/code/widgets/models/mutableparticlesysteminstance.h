#pragma once
//------------------------------------------------------------------------------
/**
	@class Particles::MutableParticleSystemInstance
	
	Manage a mutable version of a ParticleSystemInstance which is used by the particle node handler to modify a particle effect.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "particles/particlesysteminstance.h"
#include "particles/emitterattrs.h"
#include "coregraphics/mesh.h"
namespace Particles
{
class MutableParticleSystemInstance : public ParticleSystemInstance
{
	__DeclareClass(MutableParticleSystemInstance);
public:
	/// constructor
	MutableParticleSystemInstance();
	/// destructor
	virtual ~MutableParticleSystemInstance();

	/// update an emitter
	void UpdateEmitter(const EmitterAttrs& newAttrs, IndexT primGroupIndex, const Ptr<CoreGraphics::Mesh>& emitterMesh);
};
} // namespace Particles