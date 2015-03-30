#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaScene
    
    Container for MayaNodes. The MayaScene is populated with MayaNodes
    during export, and then saved to a generic XML file format.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "mayacore/mayanode.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaScene : public Core::RefCounted
{
    __DeclareClass(MayaScene);
public:
    /// constructor
    MayaScene();
    /// destructor
    virtual ~MayaScene();
    
    /// setup the object
    void Setup();
    /// discard the object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// add a node to the scene
    void AddNode(const Ptr<MayaNode>& node);
    /// get number of nodes in the scene
    SizeT GetNumNodes() const;
    /// get node by index
    const Ptr<MayaNode>& GetNodeByIndex(IndexT i) const;
    /// check if node exists by name
    bool HasNode(const Util::StringAtom& name) const;
    /// get node by name
    const Ptr<MayaNode>& GetNodeByName(const Util::StringAtom& name) const;

    /// get all nodes matching type
    Util::Array<Ptr<MayaNode>> GetNodesByType(const Util::StringAtom& type) const;
    /// get all nodes matching subtype
    Util::Array<Ptr<MayaNode>> GetNodesBySubType(const Util::StringAtom& type, const Util::StringAtom& subType) const;

private:
    Util::Array<Ptr<MayaNode>> nodes;
    Util::Dictionary<Util::StringAtom, IndexT> nameIndexMap;
    bool isValid;
};

} // namespace Maya
//------------------------------------------------------------------------------
    