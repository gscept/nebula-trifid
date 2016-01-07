#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::LevelLoader
    
    Helper class for loading a complete level from the world database.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class LevelLoader
{
public:
    /// load a complete level from the world database
    static bool Load(const Util::String& levelName, const Util::Array<Util::String>& activeLayers);    
    /// returns the layers from a level
    static Util::Array<Util::String> GetLayers(const Util::String& levelName);
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
