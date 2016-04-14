#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::Character
    
    Provides information about a character's animations.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "coreanimation/animresource.h"
#include "characters/charjointcomponents.h"
#include "characters/characterskeleton.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterVariationSet;

class CharacterVariationLibrary
{
public:
    /// constructor
    CharacterVariationLibrary();
    /// destructor
    ~CharacterVariationLibrary();

    /// setup the animation lib
    void Setup(const Ptr<CoreAnimation::AnimResource>& animResource, const CharacterSkeleton& skeleton);
    /// discard the animation lib
    void Discard();
    /// return true if object is valid
    bool IsValid() const;

    /// get pointer to the character's animation resource
    const Ptr<CoreAnimation::AnimResource>& GetAnimResource() const;
    /// get num variations
    SizeT GetNumVariations() const;
    /// get variation name at index
    const Util::StringAtom& GetVariationNameAtIndex(IndexT i) const;
                        
private:   
    friend class CharacterVariationSet;
    /// get variation by name	
    const Util::FixedArray<CharJointComponents>& GetVariationComponentsByName(const Util::StringAtom& variationName) const;
    /// check if variation exists
    bool HasVariation(const Util::StringAtom& name) const;
      
    Ptr<CoreAnimation::AnimResource> animResource;
    Util::Dictionary<Util::StringAtom, Util::FixedArray<CharJointComponents> > variationSets;    
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterVariationLibrary::IsValid() const
{
    return this->animResource.isvalid();
}  

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<CharJointComponents>& 
CharacterVariationLibrary::GetVariationComponentsByName(const Util::StringAtom& variationName) const
{
    n_assert(this->variationSets.Contains(variationName));
    return this->variationSets[variationName];
}   

//------------------------------------------------------------------------------
/**
*/
inline SizeT 
CharacterVariationLibrary::GetNumVariations() const
{
    return this->variationSets.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom& 
CharacterVariationLibrary::GetVariationNameAtIndex(IndexT i) const
{
    return this->variationSets.KeyAtIndex(i);
}

} // namespace Characters
//------------------------------------------------------------------------------
    