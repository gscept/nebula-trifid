#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterSkinLibrary
    
    Library of all available skins of a character.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/fixedarray.h"
#include "util/dictionary.h"
#include "util/stringatom.h"
#include "characters/characterskin.h"
#include "characters/characterskinlist.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkinSet;

class CharacterSkinLibrary
{
public:
    /// constructor
    CharacterSkinLibrary();
    /// destructor
    ~CharacterSkinLibrary();

    /// reserve skins (if number of skins is known beforehand)
    void ReserveSkins(SizeT numSkins);
    /// add a skin
    void AddSkin(const CharacterSkin& skin);
    /// reserve skin lists (if number of skins is known beforehand)
    void ReserveSkinLists(SizeT numSkinLists);
    /// add a skin list
    void AddSkinList(const CharacterSkinList& skinList);
    /// discard object (clears skins and skin lists)
    void Discard();
    
    /// get number of skins in the library
    SizeT GetNumSkins() const;
    /// get skin at index
    const CharacterSkin& GetSkin(IndexT i) const;
    /// return true if skin exists by name
    bool HasSkin(const Util::StringAtom& name) const;
    /// get skin by name
    const CharacterSkin& GetSkinByName(const Util::StringAtom& name) const;
    /// get skin index by name
    IndexT GetSkinIndexByName(const Util::StringAtom& name) const;

    /// get number of skin lists in the library
    SizeT GetNumSkinLists() const;
    /// get skin list at index
    const CharacterSkinList& GetSkinList(IndexT i) const;
    /// return true if skin list exists by name
    bool HasSkinList(const Util::StringAtom& name) const;
    /// get skin list by name
    const CharacterSkinList& GetSkinListByName(const Util::StringAtom& name) const;
    /// get skin list index by name
    IndexT GetSkinListIndexByName(const Util::StringAtom& name) const;

private:
    Util::Array<CharacterSkin> skins;
    Util::Dictionary<Util::StringAtom, IndexT> skinIndexMap;
    Util::Array<CharacterSkinList> skinLists;
    Util::Dictionary<Util::StringAtom, IndexT> skinListIndexMap;
};

//------------------------------------------------------------------------------
/**
*/
inline SizeT
CharacterSkinLibrary::GetNumSkins() const
{
    return this->skins.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterSkin&
CharacterSkinLibrary::GetSkin(IndexT index) const
{
    return this->skins[index];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterSkinLibrary::HasSkin(const Util::StringAtom& name) const
{
    return this->skinIndexMap.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterSkin&
CharacterSkinLibrary::GetSkinByName(const Util::StringAtom& name) const
{
    return this->skins[this->skinIndexMap[name]];
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
CharacterSkinLibrary::GetNumSkinLists() const
{
    return this->skinLists.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterSkinList&
CharacterSkinLibrary::GetSkinList(IndexT index) const
{
    return this->skinLists[index];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterSkinLibrary::HasSkinList(const Util::StringAtom& name) const
{
    return this->skinListIndexMap.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterSkinList&
CharacterSkinLibrary::GetSkinListByName(const Util::StringAtom& name) const
{
    return this->skinLists[this->skinListIndexMap[name]];
}

} // namespace Characters
//------------------------------------------------------------------------------

    