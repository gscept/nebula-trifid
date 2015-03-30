//------------------------------------------------------------------------------
//  mayashaderutil.cc
//  (C) 2009 Radon Lab GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayashaderutil.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>

namespace Maya
{

//------------------------------------------------------------------------------
/**
    Lookup the actual shader node (which has the rl* attributes assigned)
    from a shader set object (which for instance is returned by
    MFnMesh::getConnectedShaders().
*/
MObject
MayaShaderUtil::LookupMayaShaderObject(const MObject& shadingEngine)
{
    n_assert(shadingEngine.apiType() == MFn::kShadingEngine);
    n_assert(shadingEngine.hasFn(MFn::kDependencyNode));
    MStatus status;
    MObject shaderObject;

    // ...follow bread-crumb trail...
    MFnDependencyNode fnShadingEngine(shadingEngine);
    MPlug surfaceShaderPlug = fnShadingEngine.findPlug("surfaceShader", &status);
    if ((MS::kSuccess == status) && surfaceShaderPlug.isConnected())
    {
        MPlugArray inputPlugArray;
        surfaceShaderPlug.connectedTo(inputPlugArray, true, false, &status);
        if ((MS::kSuccess == status) && (inputPlugArray.length() > 0))
        {
            shaderObject = inputPlugArray[0].node();
        }
    }
    return shaderObject;
}

} // namespace Maya