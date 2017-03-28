#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::NavMeshLoader
    
    Helper class which loads all the navigation meshes in a level
    Called by BaseGameFeature::LevelLoader.
        
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class NavMeshLoader : public BaseGameFeature::EntityLoaderBase
{
    __DeclareClass(NavMeshLoader);
public:
    /// load navmeshes objects into the level
    virtual bool Load(const Util::Array<Util::String>& activeLayers);
};

} // namespace Navigation
//------------------------------------------------------------------------------
