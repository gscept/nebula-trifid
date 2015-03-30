#pragma once
//------------------------------------------------------------------------------
/**
    Maya::MayaVariable
    
    Variable enums for MayaVariableRegistry singleton.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaVariable
{
public:
    /// variable enum
    enum Code
    {
        CategoryName,
        ObjectName,

        NumMayaVariables,
        InvalidMayaVariable,
    };
};

} // namespace Maya
//------------------------------------------------------------------------------

    