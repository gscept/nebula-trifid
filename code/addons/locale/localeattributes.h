#pragma once
//------------------------------------------------------------------------------
/**
    @file localeattributes.h
    
    Declare locale database attributes.
    
    (C) 2009 Radon Labs GmbH
*/
#include "attr/attrid.h"
#include "attr/attributedefinition.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(LocaId, 'LOID', ReadOnly);
    DeclareString(LocaText, 'LOTE', ReadOnly);
} // namespace Attr
//------------------------------------------------------------------------------
