#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::GeneralParams
    
    General posteffect settings like blendtime and name
        
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "timing/time.h"
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class GeneralParams: public Core::RefCounted
{
	__DeclareClass(GeneralParams);
public:
    /// constructor
	GeneralParams();
    /// deconstructor
	virtual ~GeneralParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<GeneralParams>& rhs);
    
	/// get name of ParamSet
	const Util::String & GetName() const;
	/// set ParamSet name
	void SetName(const Util::String & name);

	/// get blend time
	const float GetBlendTime() const;
	/// set blend time
	void SetBlendTime(float time);

private:
	Util::String name;
	float blendTime;
};


//------------------------------------------------------------------------------
/**
*/
inline
const Util::String &
GeneralParams::GetName() const
{
	return this->name;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
GeneralParams::SetName(const Util::String & newName)
{
	this->name = newName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const float
GeneralParams::GetBlendTime() const
{
	return this->blendTime;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
GeneralParams::SetBlendTime(float time)
{
	this->blendTime = time;
}
}
//------------------------------------------------------------------------------
