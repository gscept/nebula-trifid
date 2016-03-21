#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EntityLoaderBase
    
    Abstract loader helper for game entities. 
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "core/refcounted.h"
#include "db/valuetable.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class EntityLoaderBase : public Core::RefCounted
{
public:
    /// construtcor
    EntityLoaderBase();
    /// destructor
    ~EntityLoaderBase();
    /// load entity objects into the level
    virtual bool Load(const Util::Array<Util::String>& activeLayers);
    /// is loader currently inside Load Function
    static bool IsLoading();

protected:
    /// return true if entity at current Db::Reader pos is in active layer
    bool EntityIsInActiveLayer(const Ptr<Db::ValueTable>& values, IndexT rowIndex, const Util::Array<Util::String>& levelActiveLayers) const;
    static bool insideLoading;

private:
    /// helper method to check if an entity is in an active layer
    bool IsInActiveLayer(const Util::String& entityLayers, const Util::Array<Util::String>& levelActiveLayers) const;
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
