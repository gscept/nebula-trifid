//------------------------------------------------------------------------------
//  loader/entityloaderbase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loader/entityloaderbase.h"
#include "basegamefeature/basegameattr/basegameattributes.h"

namespace BaseGameFeature
{

    bool EntityLoaderBase::insideLoading = false;
//------------------------------------------------------------------------------
/**
*/
EntityLoaderBase::EntityLoaderBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EntityLoaderBase::~EntityLoaderBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityLoaderBase::Load(const Util::Array<Util::String>& activeLayers)
{    
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityLoaderBase::IsInActiveLayer(const Util::String& entityLayers, const Util::Array<Util::String>& levelActiveLayers) const
{
    Util::Array<Util::String> tokens = entityLayers.Tokenize(";");
    IndexT i;
    for (i = 0; i < tokens.Size(); i++)
    {
        if (InvalidIndex != levelActiveLayers.FindIndex(tokens[i]))
        {
            return true;
        }
    }
    return  false;
}

//------------------------------------------------------------------------------
/**
    Return true if the entity at the current Db::Reader position is
    in the active layer set.
*/
bool
EntityLoaderBase::EntityIsInActiveLayer(const Ptr<Db::ValueTable>& values, IndexT rowIndex, const Util::Array<Util::String>& levelActiveLayers) const
{
    if (values->HasColumn(Attr::_Layers))
    {
        Util::String entityLayers = values->GetString(Attr::_Layers, rowIndex);
        if (entityLayers.IsValid())
        {
            // check if entity layer is one of the active layer
            if (!this->IsInActiveLayer(entityLayers, levelActiveLayers))
            {
                return false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
EntityLoaderBase::IsLoading()
{
    return insideLoading;
}
} // namespace BaseGameFeature


