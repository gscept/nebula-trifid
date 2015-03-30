//------------------------------------------------------------------------------
//  ceuiresourceprovider.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "sui/ceui/ceuirawdatacontainer.h"

namespace CEUI
{

//------------------------------------------------------------------------------
/**
*/
CEUIRawDataContainer::CEUIRawDataContainer() :
    RawDataContainer()
{
}

//------------------------------------------------------------------------------
/**
*/
CEUIRawDataContainer::~CEUIRawDataContainer() 
{
    this->release();
}

//------------------------------------------------------------------------------
/**
*/
void CEUIRawDataContainer::release()
{
    CEGUI::RawDataContainer::release();
}

} // namespace CEUI
