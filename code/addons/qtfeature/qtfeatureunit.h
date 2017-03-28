#pragma once
//------------------------------------------------------------------------------
/**
	@class QTFeature::QTFeatureUnit
    
    The FeatureUnit initializes QT and runs a qt eventloop
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include <QKeyEvent>
#include "game/featureunit.h"
#include "qtfeature/qtserver.h"
#include "qtfeature/qtinputproxy.h"
#include "input/key.h"

//------------------------------------------------------------------------------
namespace QtFeature
{

class QtFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(QtFeatureUnit);
    __DeclareSingleton(QtFeatureUnit);   

public:
    /// constructor
    QtFeatureUnit();
    /// destructor
    virtual ~QtFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
    
    /// called in the middle of the feature trigger cycle
	virtual void OnBeginFrame();

    /// called when game debug visualization is on
    virtual void OnRenderDebug();
    
    /// install input handler that sends keyboard events to qt app
    void RegisterQtInputProxy(QObject* receiver);
    /// remove input handler that sends keyboard events to qt app
    void DeregisterQtInputProxy();

	/// send an input event from Qt to Nebula
	void SendDownKeyEvent(QKeyEvent* event);
	/// send an input event from Qt to Nebula
	void SendUpKeyEvent(QKeyEvent* event);
protected:
    Ptr<QtServer> qtServer;
    Ptr<QtInputProxy> qtinputProxy;
};

}; // namespace QtFeature
//------------------------------------------------------------------------------