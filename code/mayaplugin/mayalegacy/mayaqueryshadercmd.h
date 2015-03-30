#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaQueryShaderCmd
    
    Implements the legacy queryShader MEL command used by the material editor
    window of the toolkit.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include <maya/MPxCommand.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaQueryShaderCmd : public MPxCommand
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
};

}
//------------------------------------------------------------------------------

    