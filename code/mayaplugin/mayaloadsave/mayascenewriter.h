#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaSceneWriter
  
    A stream writer which saves the content of a MayaScene object to
    an XML-formatted stream.
    
    (C) 2009 Radon Labs GmbH
*/
#include "io/streamwriter.h"
#include "mayacore/mayascene.h"
#include "io/xmlwriter.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaSceneWriter : public IO::StreamWriter
{
    __DeclareClass(MayaSceneWriter);
public:
    /// constructor
    MayaSceneWriter();
    /// destructor
    virtual ~MayaSceneWriter();
    
    /// begin writing to the stream
    virtual bool Open();
    /// end writing to the stream
    virtual void Close();
    /// write MayaScene object to the scene
    void WriteMayaScene(const Ptr<MayaScene>& mayaScene);

private:
    /// write a single Maya node
    void WriteMayaNode(const Ptr<MayaNode>& mayaNode);
    /// write an attribute of a Maya node
    void WriteAttr(const Ptr<MayaNode>& mayaNode, IndexT attrIndex);

    Ptr<IO::XmlWriter> xmlWriter;
};

} // namespace Maya
//------------------------------------------------------------------------------
