#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GraphicsInterface
  
    Implements the base interface to the InternalGraphics subsystem. Usually
    the application doesn't call methods on the GraphicsInterface
    directly, but instead uses one of the Graphics objects to
    communicate with the graphics thread.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "graphics/graphicshandler.h"
#include "debugrender/debuggraphicshandler.h"
#include "messaging/handler.h"
#include "messaging/message.h"
#include "messaging/port.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class GraphicsInterface : public Messaging::Port
{
    __DeclareClass(GraphicsInterface);
    __DeclareInterfaceSingleton(GraphicsInterface);
public:
    /// constructor
    GraphicsInterface();
    /// destructor
    virtual ~GraphicsInterface();

    /// open the interface object
    void Open();
    /// close the interface object
    void Close();

    /// wait for pending resources
    void WaitForPendingResources();
	
	/// perform per-frame stuff
	void OnFrame();

protected:
    friend class GraphicsHandler;

    /// signal the resource loaded event, called by GraphicsHandler
    void SignalPendingResourceEvent();

    Threading::ThreadId mainThreadId;
    Threading::Event frameEvent;
    Ptr<GraphicsHandler> graphicsHandler;
	Ptr<Debug::DebugGraphicsHandler> debugGraphicsHandler;
};

} // namespace Graphics
//------------------------------------------------------------------------------
