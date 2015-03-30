#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::EnvironmentLoader
    
    Helper class which loads all the environment objects into a level.
    Called by BaseGameFeature::LevelLoader.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class EnvironmentLoader : public EntityLoaderBase
{
    __DeclareClass(EnvironmentLoader);
public:
    /// load environment objects into the level
    virtual bool Load(const Util::Array<Util::String>& activeLayers);

private:
    /// update the progress indicator
    void UpdateProgressIndicator(const Util::String& resName);
};

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
