//------------------------------------------------------------------------------
//  characterskinset.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterskinset.h"
#include "characters/characterskinlibrary.h"
#include "characters/characterskinlist.h"
#include "models/modelinstance.h"

namespace Characters
{
using namespace Util;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
CharacterSkinSet::CharacterSkinSet() :
    skinLibrary(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterSkinSet::~CharacterSkinSet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::Setup(const CharacterSkinLibrary& skinLib, const Ptr<ModelInstance>& modelInst)
{
    n_assert(!this->IsValid());
    this->skinLibrary = &skinLib;
    this->modelInstance = modelInst;

	// add first skin of skin lib to be rendered
	if (this->skinLibrary->GetNumSkins() > 0)
	{
		// add skin to skin set
		this->AddSkin(this->skinLibrary->GetSkin(0).GetName());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::Discard()
{
    n_assert(this->IsValid());
    this->skinLibrary = 0;
    this->skins.Clear();
    this->modelInstance = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::AddSkin(const StringAtom& skinName)
{
    n_assert(this->IsValid());
    if (!this->skins.Contains(skinName))
    {
        // need to create a new model node instance
        const CharacterSkin& skin = this->skinLibrary->GetSkinByName(skinName);
        const Ptr<ModelNode>& modelNode = skin.GetModelNode();
        Ptr<ModelNodeInstance> rootNodeInstance = modelNode->CreateNodeInstanceHierarchy(this->modelInstance);
        this->skins.Add(skinName, rootNodeInstance);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::RemoveSkin(const StringAtom& skinName)
{
    n_assert(this->IsValid());
    if (this->skins.Contains(skinName))
    {
        // discard node instance hierarchy of this skin
        this->skins[skinName]->DiscardHierarchy();
        this->skins.Erase(skinName);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::ToggleSkin(const StringAtom& skinName)
{
    n_assert(this->IsValid());
    if (this->skins.Contains(skinName))
    {
        this->RemoveSkin(skinName);
    }
    else
    {
        this->AddSkin(skinName);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::Clear()
{
    n_assert(this->IsValid());
    IndexT i;
    for (i = 0; i < this->skins.Size(); i++)
    {
        this->skins.ValueAtIndex(i)->DiscardHierarchy();
    }
    this->skins.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkinSet::ApplySkinList(const StringAtom& skinListName)
{
    n_assert(this->IsValid());
    n_assert(this->skinLibrary->HasSkinList(skinListName));

    // first discard all currently visible skins
    this->Clear();

    // add all skins from the skin list
    const Array<StringAtom>& skinListSkins = this->skinLibrary->GetSkinListByName(skinListName).GetSkins();
    IndexT i;
    for (i = 0; i < skinListSkins.Size(); i++)
    {
        this->AddSkin(skinListSkins[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
CharacterSkinSet::HasSkin(const StringAtom& skinName) const
{
    n_assert(this->IsValid());
    return this->skins.Contains(skinName);
}

//------------------------------------------------------------------------------
/**
*/
SizeT
CharacterSkinSet::GetNumSkins() const
{
    return this->skins.Size();
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
CharacterSkinSet::GetSkin(IndexT i) const
{
    return this->skins.KeyAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Models::ModelNodeInstance>&
CharacterSkinSet::GetSkinNodeInstanceByIndex(IndexT i) const
{
    return this->skins.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Models::ModelNodeInstance>&
CharacterSkinSet::GetSkinNodeInstanceByName(const StringAtom& name) const
{
    return this->skins[name];
}

} // namespace Characters
