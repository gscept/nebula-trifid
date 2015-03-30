#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::DebugGraphicsHandler
    
    Handles debug-visualization messages in the graphics thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "debugrender/debugrenderprotocol.h"
#include "debug/debugcounter.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Debug
{
class DebugGraphicsHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(DebugGraphicsHandler);
    __DeclareSingleton(DebugGraphicsHandler);
public:
    /// constructor
    DebugGraphicsHandler();
    /// destructor
    virtual ~DebugGraphicsHandler();    
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// optional "per-frame" DoWork method for continuous handlers
    virtual void DoWork();

    /// enable/disable the debug perf-hud
    void SetPerfHUDEnabled(bool b);
    /// toggle the perf-hud
    void TogglePerfHUD();
    /// return true if debug perf-hud is enabled
    bool IsPerfHUDEnabled() const;

private:
    /// handle RenderDebugText message
    void OnRenderDebugText(const Ptr<RenderDebugText>& msg);
    /// handle RenderDebugShapes message
    void OnRenderDebugShapes(const Ptr<RenderDebugShapes>& msg);
    /// handle render debug message
    void OnRenderDebugView(const Ptr<RenderDebugView>& msg);
    /// handle increment layer index of debug shader
    void OnIncrDebugShaderLayer(const Ptr<IncrDebugShaderLayer>& msg);
    /// handle set shader variable 
    void OnSetFloatShaderVariable(const Ptr<SetFloatShaderVariable>& msg);
    /// enable/disable performance hud
    void OnControlPerfHUD(const Ptr<ControlPerfHUD>& msg);

    int shaderDebugLayer;
    bool perfHudEnabled;

    // debug counters/timers for perf-hud
    Ptr<Debug::DebugCounter> numPrimitives;
    Ptr<Debug::DebugCounter> numDrawCalls;
    Ptr<Debug::DebugTimer> frameTime;
};

//------------------------------------------------------------------------------
/**
*/
inline void
DebugGraphicsHandler::SetPerfHUDEnabled(bool b)
{
    this->perfHudEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DebugGraphicsHandler::IsPerfHUDEnabled() const
{
    return this->perfHudEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DebugGraphicsHandler::TogglePerfHUD()
{
    this->perfHudEnabled = !this->perfHudEnabled;
}

} // namespace Debug
//------------------------------------------------------------------------------
