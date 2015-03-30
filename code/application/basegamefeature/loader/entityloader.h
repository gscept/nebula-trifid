#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EntityLoader
    
    Loader helper for universal game entities. The properties which are
    attached to the entity are described in blueprints.xml, the attributes
    to attach come from the world database.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "loader/entityloaderbase.h"
#include "game/entity.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{

class EntityLoader : public EntityLoaderBase
{
    __DeclareClass(EntityLoader);
public:
    /// load entity objects into the level
    virtual bool Load(const Util::Array<Util::String>& activeLayers);

private:
    /// update the progress indicator
    void UpdateProgressIndicator(const Ptr<Game::Entity>& gameEntity);
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
