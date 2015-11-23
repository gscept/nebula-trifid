#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectSampler
    
    An effect sampler provides an interface to which one can alter samplers.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectsampler.h"
#endif

#include "EASTL/string.h"
#include "annotable.h"
namespace AnyFX
{
class InternalEffectSampler;
class EffectSampler : public Annotable
{
public:

	enum AddressMode
	{
		Wrap = 0,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,

		NumAddressModes
	};

	enum FilterMode
	{
		MinMagMipPoint = 0,
		MinMagMipLinear,
		MinMagPointMipLinear,
		MinMipPointMagLinear,
		MinPointMipMagLinear,
		MinLinearMipMagPoint,
		MinMipLinearMagPoint,
		MinMagLinearMipPoint,
		Anisotropic,
		Point,
		Linear,

		NumFilterModes
	};

	enum ComparisonFunc
	{
		Never = 0,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,		
		Always,

		NumDrawDepthFuncs
	};

	/// constructor
	EffectSampler();
	/// destructor
	virtual ~EffectSampler();

	/// discard program
	void Discard();

	/// get name of variable
    const eastl::string& GetName() const;

private:
	friend class EffectSamplerStreamLoader;
	friend class InternalEffectProgram;

	InternalEffectSampler* internalSampler;
}; 

} // namespace AnyFX
//------------------------------------------------------------------------------