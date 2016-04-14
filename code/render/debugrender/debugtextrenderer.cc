//------------------------------------------------------------------------------
//  debugtextrenderer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "debugrender/debugtextrenderer.h"
#include "threading/thread.h"
#include "debugrender/debugrenderprotocol.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/cameraentity.h"

namespace Debug
{
__ImplementClass(Debug::DebugTextRenderer, 'DBTR', Core::RefCounted);
__ImplementSingleton(Debug::DebugTextRenderer);

using namespace Util;
using namespace Math;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Threading;

//------------------------------------------------------------------------------
/**
*/
DebugTextRenderer::DebugTextRenderer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DebugTextRenderer::~DebugTextRenderer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
DebugTextRenderer::DrawText(const String& text, const float4& color, const float2& pos)
{
    this->textElements.Append(TextElement(Thread::GetMyThreadId(), text, color, pos, 16));
}

//------------------------------------------------------------------------------
/**
*/
void
DebugTextRenderer::DrawText3D(const String& text, const float4& color, const point& pos)
{
    // calculate screen space position
    const Ptr<View>& view = Graphics::GraphicsServer::Instance()->GetDefaultView();
    const Ptr<CameraEntity> cam = view->GetCameraEntity();
    matrix44 viewProj = cam->GetViewProjTransform();
    Math::float4 screenPos = matrix44::transform(pos, viewProj);
    screenPos.x() /= screenPos.w();
    screenPos.y() /= screenPos.w();
    screenPos.x() = (screenPos.x() + 1.0f) * 0.5f;
    screenPos.y() = 1.0f - ((screenPos.y() + 1.0f) * 0.5f);

    this->DrawText(text,  color, Math::float2(screenPos.x(), screenPos.y()));
}

//------------------------------------------------------------------------------
/**
*/
void
DebugTextRenderer::OnFrame()
{
    // send a RenderDebugText message to the render thread, do this
    // once per frame, even if there are no text elements to be 
    // rendered, since this will also cleanup the text elements
    // from the previous frame in the render thread
    Ptr<RenderDebugText> msg = RenderDebugText::Create();
    msg->SetThreadId(Thread::GetMyThreadId());
    msg->SetTextElements(this->textElements);
    Graphics::GraphicsInterface::Instance()->Send(msg.cast<Messaging::Message>());
    this->textElements.Clear();
}

} // namespace Debug
