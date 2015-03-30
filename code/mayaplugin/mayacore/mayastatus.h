#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaStatus
    
    Status codes for exporting / extraction operations.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaStatus
{
public:
    /// status enum
    enum Code
    {
        Error = 0,
        Success,
        NothingTodo,
    };
}; 
 
} // namespace Maya
//------------------------------------------------------------------------------
    