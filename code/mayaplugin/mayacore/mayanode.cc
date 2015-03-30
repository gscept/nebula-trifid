//------------------------------------------------------------------------------
//  mayanode.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayacore/mayanode.h"

namespace Maya
{
__ImplementClass(Maya::MayaNode, 'MAYN', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaNode::MayaNode() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MayaNode::~MayaNode()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::Setup(const StringAtom& name_, const StringAtom& type_, const StringAtom& subType_, Ptr<MayaNode> parent_)
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->name = name_;
    this->type = type_;
    this->subType = subType_;
    this->parent = parent_;
    if (this->parent.isvalid())
    {
        this->parent->AddChild(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::Discard()
{
    n_assert(this->IsValid());
    this->parent = 0;
    this->children.Clear();
    this->childIndexMap.Clear();
    this->attrs.Clear();
    this->attrIndexMap.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::AddChild(Ptr<MayaNode> newChild)
{
    n_assert(newChild.isvalid());
    n_assert(!this->HasChild(newChild->GetName()))
    this->children.Append(newChild);
    this->childIndexMap.Add(newChild->GetName(), this->children.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaNode::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
MayaNode::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::SetType(const StringAtom& t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
MayaNode::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::SetSubType(const StringAtom& s)
{
    this->subType = s;
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
MayaNode::GetSubType() const
{
    return this->subType;
}

//------------------------------------------------------------------------------
/**
*/
SizeT
MayaNode::GetNumChildren() const
{
    return this->children.Size();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<MayaNode>&
MayaNode::GetChildByIndex(IndexT i) const
{
    return this->children[i];
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaNode::HasChild(const StringAtom& n) const
{
    return this->childIndexMap.Contains(n);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<MayaNode>&
MayaNode::GetChildByName(const StringAtom& n) const
{
    return this->children[this->childIndexMap[n]];
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaNode::HasParent() const
{
    return this->parent.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<MayaNode>&
MayaNode::GetParent() const
{
    return this->parent;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaNode::AddAttr(const StringAtom& n, const Variant& val)
{
    n_assert(!this->HasAttr(n));
    this->attrs.Append(KeyValuePair<StringAtom,Variant>(n, val));
    this->attrIndexMap.Add(n, this->attrs.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
SizeT
MayaNode::GetNumAttrs() const
{
    return this->attrs.Size();
}

//------------------------------------------------------------------------------
/**
*/
const StringAtom&
MayaNode::GetAttrNameByIndex(IndexT i) const
{
    return this->attrs[i].Key();
}

//------------------------------------------------------------------------------
/**
*/
const Variant&
MayaNode::GetAttrValueByIndex(IndexT i) const
{
    return this->attrs[i].Value();
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaNode::HasAttr(const StringAtom& n) const
{
    return this->attrIndexMap.Contains(n);
}

//------------------------------------------------------------------------------
/**
*/
const Variant&
MayaNode::GetAttrByName(const StringAtom& n) const
{
    return this->attrs[this->attrIndexMap[n]].Value();
}

} // namespace Maya