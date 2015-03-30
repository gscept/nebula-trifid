#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShaderUtil
    
    Utility methods to work with Maya shader nodes.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include <maya/MObject.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShaderUtil
{
public:
    /// lookup the actual shader node from a shader set
    static MObject LookupMayaShaderObject(const MObject& shadingEngine);
};

} // namespace Maya
//------------------------------------------------------------------------------
