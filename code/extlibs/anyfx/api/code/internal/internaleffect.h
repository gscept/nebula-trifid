#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffect
    
    Effect backend, inherit this class to provide an implementation.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
namespace AnyFX
{
class InternalEffect
{
public:

	enum InternalEffectType
	{
		HLSL3,
		HLSL4,
		HLSL5,
		GLSL1,
		GLSL2,
		GLSL3,
		GLSL4,
		PS3,
		WII,

		NumInternalEffectTypes
	};

	/// constructor
	InternalEffect();
	/// destructor
	virtual ~InternalEffect();

private:
	friend class Effect;

	InternalEffectType type;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------