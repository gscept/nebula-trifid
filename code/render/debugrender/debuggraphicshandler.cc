//------------------------------------------------------------------------------
//  debuggraphicshandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "debugrender/debuggraphicshandler.h"
#include "coregraphics/textrenderer.h"
#include "coregraphics/shaderserver.h"
#include "io/memorystream.h"
#include "graphics/graphicsserver.h"
#include "coregraphics/shadersemantics.h"
#include "debug/debugserver.h"
#include "threading/thread.h"

namespace Debug
{
__ImplementClass(Debug::DebugGraphicsHandler, 'DGRH', Interface::InterfaceHandlerBase);
__ImplementSingleton(Debug::DebugGraphicsHandler);

using namespace IO;
using namespace Util;
using namespace Messaging;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Debug;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
DebugGraphicsHandler::DebugGraphicsHandler():
    shaderDebugLayer(0),
    perfHudEnabled(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DebugGraphicsHandler::~DebugGraphicsHandler()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
DebugGraphicsHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(RenderDebugText::Id))
    {
        this->OnRenderDebugText(msg.cast<RenderDebugText>());
    }
    else if (msg->CheckId(RenderDebugShapes::Id))
    {
        this->OnRenderDebugShapes(msg.cast<RenderDebugShapes>());
    }
    else if (msg->CheckId(Debug::RenderDebugView::Id))
    {
        this->OnRenderDebugView(msg.cast<Debug::RenderDebugView>());
    }
    else if (msg->CheckId(Debug::IncrDebugShaderLayer::Id))
    {
        this->OnIncrDebugShaderLayer(msg.cast<Debug::IncrDebugShaderLayer>());
    }
    else if (msg->CheckId(Debug::SetFloatShaderVariable::Id))
    {
        this->OnSetFloatShaderVariable(msg.cast<Debug::SetFloatShaderVariable>());
    }
    else if (msg->CheckId(Debug::ControlPerfHUD::Id))
    {
        this->OnControlPerfHUD(msg.cast<Debug::ControlPerfHUD>());
    }
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
DebugGraphicsHandler::DoWork()
{
    // render debug hud if enabled
    if (this->perfHudEnabled)
    {
        DebugServer* dbgServer = DebugServer::Instance();

        // lookup debug counters
        if (!this->frameTime.isvalid())
        {
            this->frameTime = dbgServer->GetDebugTimerByName("GraphicsFrameTime");
        }
        if (!this->numPrimitives.isvalid())
        {
            this->numPrimitives = dbgServer->GetDebugCounterByName("RenderDeviceNumPrimitives");
        }
        if (!this->numDrawCalls.isvalid())
        {
            this->numDrawCalls = dbgServer->GetDebugCounterByName("RenderDeviceNumDrawCalls");
        }

        // draw debug text
        String str;
        str.Format("draw_calls: %d\nprimitives: %d\nframe_time: %.3f ms", this->numDrawCalls->GetSample(), this->numPrimitives->GetSample(), this->frameTime->GetSample());
        TextElement textElement(Threading::Thread::GetMyThreadId(), str, float4(1.0f, 0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f), 10);
        TextRenderer::Instance()->AddTextElement(textElement);
    }
    InterfaceHandlerBase::DoWork();
}

//------------------------------------------------------------------------------
/**
*/
void
DebugGraphicsHandler::OnRenderDebugText(const Ptr<RenderDebugText>& msg)
{
    TextRenderer::Instance()->DeleteTextElementsByThreadId(msg->GetThreadId());
    TextRenderer::Instance()->AddTextElements(msg->GetTextElements());
}

//------------------------------------------------------------------------------
/**
*/
void
DebugGraphicsHandler::OnRenderDebugShapes(const Ptr<RenderDebugShapes>& msg)
{
    ShapeRenderer::Instance()->DeleteShapesByThreadId(msg->GetThreadId());
    ShapeRenderer::Instance()->AddShapes(msg->GetShapes());
}

//------------------------------------------------------------------------------
/**
*/
void
DebugGraphicsHandler::OnRenderDebugView(const Ptr<Debug::RenderDebugView>& msg)
{
    GraphicsServer::Instance()->SetRenderDebug(msg->GetEnableDebugRendering());
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugGraphicsHandler::OnIncrDebugShaderLayer(const Ptr<Debug::IncrDebugShaderLayer>& msg)
{
    const Ptr<CoreGraphics::ShaderVariable>&  shdVar = ShaderServer::Instance()->GetSharedVariableBySemantic(ShaderVariable::Semantic(NEBULA3_SEMANTIC_DEBUGSHADERLAYER));
    n_assert(shdVar.isvalid());
    shaderDebugLayer++;
    if (shaderDebugLayer > 6) shaderDebugLayer = 0;
    shdVar->SetInt(shaderDebugLayer);
}

//------------------------------------------------------------------------------
/**
*/
void 
DebugGraphicsHandler::OnSetFloatShaderVariable(const Ptr<Debug::SetFloatShaderVariable> &msg)
{
    const Ptr<CoreGraphics::ShaderVariable>& shdVar = ShaderServer::Instance()->GetSharedVariableBySemantic(msg->GetSemantic());
    n_assert(shdVar.isvalid());
    shdVar->SetFloat(msg->GetValue());
}

//------------------------------------------------------------------------------
/**
*/
void
DebugGraphicsHandler::OnControlPerfHUD(const Ptr<ControlPerfHUD>& msg)
{
    if (msg->GetToggle())
    {
        this->TogglePerfHUD();
    }
    else
    {
        this->SetPerfHUDEnabled(msg->GetEnable());
    }
}

} // namespace Debug
