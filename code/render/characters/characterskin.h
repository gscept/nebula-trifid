#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterSkin
    
    Describes a single skin in a CharacterSkinLibrary.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "core/types.h"
#include "util/stringatom.h"
#include "models/modelnode.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkin
{
public:
    /// default constructor
    CharacterSkin();
    /// constructor
    CharacterSkin(const Ptr<Models::ModelNode>& modelNode, const Util::StringAtom& category, const Util::StringAtom& name);
    /// destructor
    ~CharacterSkin();

    /// get pointer to skin's model node
    const Ptr<Models::ModelNode>& GetModelNode() const;
    /// get skin name
    const Util::StringAtom& GetName() const;
    /// get skin category
    const Util::StringAtom& GetCategory() const;

private:
    Ptr<Models::ModelNode> modelNode;
    Util::StringAtom category;
    Util::StringAtom name;
};    

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ModelNode>&
CharacterSkin::GetModelNode() const
{
    return this->modelNode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
CharacterSkin::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
CharacterSkin::GetCategory() const
{
    return this->category;
}

} // namespace Characters
//------------------------------------------------------------------------------

