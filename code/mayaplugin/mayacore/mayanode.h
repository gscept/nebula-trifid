#pragma once
//------------------------------------------------------------------------------
/**
    @class MayaNode
  
    A generic Maya export node. This can be anything, a transform node,
    a mesh node, particle node or anything else.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "util/variant.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaNode : public Core::RefCounted
{
    __DeclareClass(MayaNode);
public:
    /// constructor
    MayaNode();
    /// destructor
    virtual ~MayaNode();
    
    /// setup the node
    void Setup(const Util::StringAtom& name, const Util::StringAtom& type, const Util::StringAtom& subType, Ptr<MayaNode> parent);
    /// discard the node
    void Discard();
    /// return true if the node has been setup
    bool IsValid() const;

    /// get name of the node
    const Util::StringAtom& GetName() const;
    /// set type (post-setup)
    void SetType(const Util::StringAtom& t);
    /// get type of the node
    const Util::StringAtom& GetType() const;
    /// set subtype (post-setup)
    void SetSubType(const Util::StringAtom& s);
    /// get subtype of the node
    const Util::StringAtom& GetSubType() const;
    
    /// get the number of children of the node
    SizeT GetNumChildren() const;
    /// get child at index
    const Ptr<MayaNode>& GetChildByIndex(IndexT i) const;
    /// check if a child exists by name
    bool HasChild(const Util::StringAtom& name) const;
    /// lookup child by name
    const Ptr<MayaNode>& GetChildByName(const Util::StringAtom& name) const;
    /// check if node has a parent
    bool HasParent() const;
    /// get pointer to parent
    const Ptr<MayaNode>& GetParent() const;

    /// add an attribute to the node
    void AddAttr(const Util::StringAtom& name, const Util::Variant& value);
    /// get number of attributes on the node
    SizeT GetNumAttrs() const;
    /// get attribute name at index
    const Util::StringAtom& GetAttrNameByIndex(IndexT i) const;
    /// get attribute at index
    const Util::Variant& GetAttrValueByIndex(IndexT i) const;
    /// check if attribute exists by name
    bool HasAttr(const Util::StringAtom& name) const;
    /// return attribute value by name
    const Util::Variant& GetAttrByName(const Util::StringAtom& name) const;
    
private:
    /// add a child node (this is called from the Setup method of the child!)
    void AddChild(Ptr<MayaNode> child);

    Util::StringAtom name;
    Util::StringAtom type;
    Util::StringAtom subType;
    Ptr<MayaNode> parent;
    Util::Array<Ptr<MayaNode>> children;
    Util::Dictionary<Util::StringAtom,IndexT> childIndexMap;
    Util::Array<Util::KeyValuePair<Util::StringAtom, Util::Variant>> attrs;
    Util::Dictionary<Util::StringAtom,IndexT> attrIndexMap;
    bool isValid;
};

} // namespace Maya
//------------------------------------------------------------------------------
