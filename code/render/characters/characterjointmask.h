#pragma once
//------------------------------------------------------------------------------
/**
	Contains masks appliable to animation tracks in order to weight animations dependent on joint.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/types.h"
#include "util/stringatom.h"
#include "util/fixedarray.h"

namespace Characters
{
class CharacterJointMask
{
public:
	/// constructor
	CharacterJointMask();
	/// destructor
	virtual ~CharacterJointMask();

	/// set the mask name
	void SetName(const Util::StringAtom& name);
	/// get the mask name
	const Util::StringAtom& GetName() const;
	/// set the weights
	void SetWeights(const Util::FixedArray<Math::scalar>& weights);
	/// set weight by index
	void SetWeight(const IndexT i, const Math::scalar weight);
	/// get the weights
	const Util::FixedArray<Math::scalar>& GetWeights() const;
	/// get the amount of weights used
	const SizeT GetNumWeights() const;
	/// get single weight
	const float GetWeight(const IndexT i) const;

	/// handle finding character joint masks
	bool operator==(const CharacterJointMask& rhs) const;

private:
	Util::StringAtom name;
	Util::FixedArray<Math::scalar> weights;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterJointMask::SetName(const Util::StringAtom& name)
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
CharacterJointMask::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterJointMask::SetWeights(const Util::FixedArray<Math::scalar>& weights)
{
	this->weights = weights;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterJointMask::SetWeight(const IndexT i, const Math::scalar weight)
{
	this->weights[i] = weight;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<Math::scalar>&
CharacterJointMask::GetWeights() const
{
	return this->weights;
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT
CharacterJointMask::GetNumWeights() const
{
	return this->weights.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const float
CharacterJointMask::GetWeight(const IndexT i) const
{
	return this->weights[i];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterJointMask::operator==(const CharacterJointMask& rhs) const
{
	return this->name == rhs.name;
}

} // namespace Characters