#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaAttrUtilCmd
    
    Implements the rlAttrUtil MEL command which allows access to the
    MayaAttrUtil class.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include "mayautil/mayaattrtype.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaAttrUtilCmd : public MPxCommand
{
public:
    /// command provides a syntax object
    virtual bool hasSyntax() const;
    /// command is not undoable
    virtual bool isUndoable() const;
    /// execute the command
    virtual MStatus doIt(const MArgList& args);
    /// object construction callback
    static void* creator();
    /// return a syntax object
    static MSyntax getSyntax();

private:
    /// check if attribute exists, and optionally its type
    bool CheckAttr(const MObject& node, const MString& attrName, MayaAttrType::Code type=MayaAttrType::InvalidAttrType);
};

} // namespace Maya
//------------------------------------------------------------------------------
