//------------------------------------------------------------------------------
//  characterskinlibrary.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterskinlibrary.h"

namespace Characters
{

//------------------------------------------------------------------------------
/**
*/
CharacterSkinLibrary::CharacterSkinLibrary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterSkinLibrary::~CharacterSkinLibrary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinLibrary::ReserveSkins(SizeT numSkins)
{
    n_assert(this->skins.IsEmpty());
    n_assert(this->skinIndexMap.IsEmpty());
    this->skins.Reserve(numSkins);
    this->skinIndexMap.Reserve(numSkins);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinLibrary::AddSkin(const CharacterSkin& skin)
{
    n_assert(!this->skinIndexMap.Contains(skin.GetName()));
    this->skins.Append(skin);
    this->skinIndexMap.Add(skin.GetName(), this->skins.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinLibrary::ReserveSkinLists(SizeT numSkinLists)
{
    n_assert(this->skinLists.IsEmpty());
    n_assert(this->skinListIndexMap.IsEmpty());
    this->skinLists.Reserve(numSkinLists);
    this->skinListIndexMap.Reserve(numSkinLists);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinLibrary::AddSkinList(const CharacterSkinList& skinList)
{
    n_assert(!this->skinListIndexMap.Contains(skinList.GetName()));
    this->skinLists.Append(skinList);
    this->skinListIndexMap.Add(skinList.GetName(), this->skinLists.Size() - 1);
}
//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinLibrary::Discard()
{
    this->skins.Clear();
    this->skinIndexMap.Clear();
    this->skinLists.Clear();
    this->skinListIndexMap.Clear();
}

//------------------------------------------------------------------------------
/**
*/
IndexT
CharacterSkinLibrary::GetSkinIndexByName(const Util::StringAtom& name) const
{
    IndexT i = this->skinIndexMap.FindIndex(name);
    if (InvalidIndex == i)
    {
        return InvalidIndex;
    }
    else
    {
        return this->skinIndexMap.ValueAtIndex(i);
    }
}

//------------------------------------------------------------------------------
/**
*/
IndexT
CharacterSkinLibrary::GetSkinListIndexByName(const Util::StringAtom& name) const
{
    IndexT i = this->skinListIndexMap.FindIndex(name);
    if (InvalidIndex == i)
    {
        return InvalidIndex;
    }
    else
    {
        return this->skinListIndexMap.ValueAtIndex(i);
    }
}

} // namespace Characters
