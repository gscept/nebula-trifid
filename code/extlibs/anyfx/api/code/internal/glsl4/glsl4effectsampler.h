#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectSampler
    
    GLSL4 sampler implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectsampler.h"
#include "GL/glew.h"
namespace AnyFX
{
class GLSL4EffectSampler : public InternalEffectSampler
{
public:
	/// constructor
	GLSL4EffectSampler();
	/// destructor
	virtual ~GLSL4EffectSampler();

	/// setup using texture variable
	void Setup(const eastl::vector<InternalEffectVariable*>& texture);

private:

	/// commit sampler
	void Apply();

	GLuint sampler;

	GLuint* textures;
	unsigned numTextures;
}; 

struct GLSL4GlobalSamplerState
{
    static unsigned     samplerBinds[InternalEffectSampler::MaxNumSamplerBinds];
};


} // namespace AnyFX
//------------------------------------------------------------------------------