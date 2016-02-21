//------------------------------------------------------------------------------
// mutablecharacterskeleton.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablecharacterskeleton.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
MutableCharacterSkeleton::MutableCharacterSkeleton()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableCharacterSkeleton::~MutableCharacterSkeleton()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MutableCharacterSkeleton::RenameMask(const Util::StringAtom& oldName, const Util::StringAtom& newName, Characters::CharacterJointMask* mask)
{
	n_assert(this->maskIndexMap.Contains(oldName));
	this->maskIndexMap.Erase(oldName);
	const Characters::CharacterJointMask& constMask = *mask;
	this->maskIndexMap.Add(newName, this->maskArray.FindIndex(constMask));
	mask->SetName(newName);
}

//------------------------------------------------------------------------------
/**
*/
void
MutableCharacterSkeleton::EraseMask(Characters::CharacterJointMask* mask)
{
	this->maskIndexMap.Erase(mask->GetName());
	const Characters::CharacterJointMask& constMask = *mask;
	this->maskArray.EraseIndex(this->maskArray.FindIndex(constMask));
}

} // namespace Widgets