#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterSkinSet
    
    A skin set contains the currently visible skin instances of a character
    instance.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/stringatom.h"
#include "util/array.h"
#include "models/modelnodeinstance.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkinLibrary;

class CharacterSkinSet
{
public:
    /// constructor
    CharacterSkinSet();
    /// destructor
    ~CharacterSkinSet();

    /// setup the skin set
    void Setup(const CharacterSkinLibrary& skinLib, const Ptr<Models::ModelInstance>& modelInst);
    /// discard the skin set
    void Discard();
    /// return true if skin set has been setup
    bool IsValid() const;

    /// add a skin to the skin set
    void AddSkin(const Util::StringAtom& skinName);
    /// remove a skin from the skin set
    void RemoveSkin(const Util::StringAtom& skinName);
    /// toggle skin visibility
    void ToggleSkin(const Util::StringAtom& skinName);
    /// clear all skins
    void Clear();
    /// apply a skin list (clear visible skins, and make skins in the skin list visible)
    void ApplySkinList(const Util::StringAtom& skinListName);
    /// return true if the skin set contains a skin
    bool HasSkin(const Util::StringAtom& skinName) const;
    /// get number of skins in the skin set
    SizeT GetNumSkins() const;
    /// get skin name by index
    const Util::StringAtom& GetSkin(IndexT index) const;
    /// get model node instance of skin by index
    const Ptr<Models::ModelNodeInstance>& GetSkinNodeInstanceByIndex(IndexT index) const;
    /// get model node instace of skin by name
    const Ptr<Models::ModelNodeInstance>& GetSkinNodeInstanceByName(const Util::StringAtom& name) const;

private:
    const CharacterSkinLibrary* skinLibrary;
    Ptr<Models::ModelInstance> modelInstance;
    Util::Dictionary<Util::StringAtom, Ptr<Models::ModelNodeInstance> > skins;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterSkinSet::IsValid() const
{
    return (0 != this->skinLibrary);
}

} // namespace Characters
//------------------------------------------------------------------------------
