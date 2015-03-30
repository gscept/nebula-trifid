#pragma once
//------------------------------------------------------------------------------
/**
    @class CEUI::CEUIRawDataContainer

    Resource Provider wrapper for crazy eddies GUI system.

    (C) 2009 Radon Labs GmbH
*/
#include "cegui/include/CEGUIDataContainer.h"

namespace CEUI
{

class CEUIRawDataContainer : public CEGUI::RawDataContainer
{
public:
    /// constructor
    CEUIRawDataContainer();
    /// destructor
    virtual ~CEUIRawDataContainer();
    /// release
    virtual void release();
};

}

