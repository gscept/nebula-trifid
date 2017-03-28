#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterVariationSet
    
    Contains the currently active variation of a character.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/stringatom.h"
#include "util/array.h"
#include "util/fixedarray.h"
#include "characters/charjointcomponents.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterVariationLibrary;

class CharacterVariationSet
{
public:
    /// constructor
    CharacterVariationSet();
    /// destructor
    ~CharacterVariationSet();

    /// setup the skin set
    void Setup(const Util::StringAtom& defaultVariationName, const CharacterVariationLibrary& varLib);
    /// discard the skin set
    void Discard();
    /// return true if skin set has been setup
    bool IsValid() const;
    /// set active variation
    void SetActiveVariation(const Util::StringAtom& variationName);
    /// get active variation
    const Util::StringAtom& GetActiveVariation() const;
    /// get variation transformations
    const Util::FixedArray<CharJointComponents>& GetVariationCharJointComponents() const;
                
private:
    const CharacterVariationLibrary* variationLibrary;
    Util::StringAtom variationName;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterVariationSet::IsValid() const
{
    return (0 != this->variationLibrary && variationName.IsValid());
}

} // namespace Characters
//------------------------------------------------------------------------------
