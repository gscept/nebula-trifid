#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaEnvCmd
    
    Emulates the legacy rlEnv command.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include <maya/MPxCommand.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaEnvCmd : public MPxCommand
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

    
    