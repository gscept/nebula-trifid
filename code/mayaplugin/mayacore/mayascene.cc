//------------------------------------------------------------------------------
//  mayascene.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayacore/mayascene.h"

namespace Maya
{
__ImplementClass(Maya::MayaScene, 'MAYS', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaScene::MayaScene() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MayaScene::~MayaScene()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
MayaScene::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaScene::Discard()
{
    n_assert(this->IsValid());

    // discard all nodes
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        this->nodes[i]->Discard();
    }
    this->nodes.Clear();
    this->nameIndexMap.Clear();

    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaScene::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaScene::AddNode(const Ptr<MayaNode>& node)
{
    n_assert(node.isvalid());
    n_assert(!this->HasNode(node->GetName()));
    this->nodes.Append(node);
    this->nameIndexMap.Add(node->GetName(), this->nodes.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
SizeT
MayaScene::GetNumNodes() const
{
    return this->nodes.Size();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<MayaNode>&
MayaScene::GetNodeByIndex(IndexT i) const
{
    return this->nodes[i];
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaScene::HasNode(const StringAtom& name) const
{
    return this->nameIndexMap.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<MayaNode>&
MayaScene::GetNodeByName(const StringAtom& name) const
{
    return this->nodes[this->nameIndexMap[name]];
}

//------------------------------------------------------------------------------
/**
*/
Array<Ptr<MayaNode>>
MayaScene::GetNodesByType(const StringAtom& type) const
{
    Array<Ptr<MayaNode>> result;
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        if (this->nodes[i]->GetType() == type)
        {
            result.Append(this->nodes[i]);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Array<Ptr<MayaNode>>
MayaScene::GetNodesBySubType(const StringAtom& type, const StringAtom& subType) const
{
    Array<Ptr<MayaNode>> result;
    IndexT i;
    for (i = 0; i < this->nodes.Size(); i++)
    {
        if ((this->nodes[i]->GetType() == type) &&
            (this->nodes[i]->GetSubType() == subType))
        {
            result.Append(this->nodes[i]);
        }
    }
    return result;
}

} // namespace Maya