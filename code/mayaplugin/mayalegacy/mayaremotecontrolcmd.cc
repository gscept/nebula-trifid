//------------------------------------------------------------------------------
//  mayaremotecontrolcmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaremotecontrolcmd.h"
#include <maya/MSyntax.h>

namespace Maya
{

//------------------------------------------------------------------------------
/**
*/
bool
MayaRemoteControlCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaRemoteControlCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaRemoteControlCmd::creator()
{
    return new MayaRemoteControlCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaRemoteControlCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-addr", "-address");    // FIXME: short param may only be 3 chars!
    syntax.addFlag("-msg", "-message");
    syntax.addFlag("-cl", "-closeapp");
    return syntax;
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaRemoteControlCmd::doIt(const MArgList& args)
{
    // FIXME: just return successful, do nothing
    return MS::kSuccess;
}

} // namespace Maya