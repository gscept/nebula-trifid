#pragma once
//------------------------------------------------------------------------------
/**
    @class FxFeature::FxFeatureUnit

    (C) 2008 Radon Labs GmbH
*/
#include "game/featureunit.h"
#include "fx/fxfeature/fxmanager.h"
#include "fx/fxfeature/fxrendermodule.h"

namespace FxFeature
{
class FxFeatureUnit : public Game::FeatureUnit

{
__DeclareClass(FxFeatureUnit);
__DeclareSingleton(FxFeatureUnit); 
public:
    /// Constructor
    FxFeatureUnit();
    /// Destructor
    ~FxFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
    /// called after loading game state
    virtual void OnLoad();

    /// called on begin of frame
    virtual void OnBeginFrame();    
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();
    /// called at the end of the feature trigger cycle
    virtual void OnEndFrame();

private:
    Ptr<FxManager> fxManager;
	Ptr<FxRenderModule> renderModule;
    
};
}; // namespace FxFeatureUnit