#pragma once
//------------------------------------------------------------------------------
/**
	Inherits from CharacterSkeleton so that we may change it.

	This class is used by the CharacterNode handler and allows us to rename joint masks.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "characters/characterskeleton.h"
namespace Widgets
{
class MutableCharacterSkeleton : public Characters::CharacterSkeleton
{
public:
	/// constructor
	MutableCharacterSkeleton();
	/// destructor
	virtual ~MutableCharacterSkeleton();

	/// renames a joint mask
	void RenameMask(const Util::StringAtom& oldName, const Util::StringAtom& newName, Characters::CharacterJointMask* mask);
	/// erases a joint mask
	void EraseMask(Characters::CharacterJointMask* mask);
private:
};
} // namespace Widgets