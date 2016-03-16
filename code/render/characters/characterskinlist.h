#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterSkinList
    
    A skin list contains a set of visible skins for a character.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/stringatom.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkinList
{
public:
    /// default constructor
    CharacterSkinList();
    /// destructor
    ~CharacterSkinList();

    /// set the skin list's name
    void SetName(const Util::StringAtom& name);
    /// get the skin list's name
    const Util::StringAtom& GetName() const;
    /// set the skins in the skin list
    void SetSkins(const Util::Array<Util::StringAtom>& skins);
    /// get the skins 
    const Util::Array<Util::StringAtom>& GetSkins() const;

private:
    Util::StringAtom name;
    Util::Array<Util::StringAtom> skins;
};

} // namespace Characters
//------------------------------------------------------------------------------



    