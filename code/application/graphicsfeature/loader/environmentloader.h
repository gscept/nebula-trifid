#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::EnvironmentLoader
    
    Helper class which loads all the environment objects into a level.
    Called by BaseGameFeature::LevelLoader.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "basegamefeature/loader/entityloaderbase.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class EnvironmentLoader : public BaseGameFeature::EntityLoaderBase
{
    __DeclareClass(EnvironmentLoader);
public:
    /// load environment objects into the level
    virtual bool Load(const Util::Array<Util::String>& activeLayers);

private:
    /// update the progress indicator
    void UpdateProgressIndicator(const Util::String& resName);
};

} // namespace GraphicsFeature
//------------------------------------------------------------------------------
