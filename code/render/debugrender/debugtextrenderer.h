#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::DebugTextRenderer
    
    Client-side proxy for rendering debug text. Packs text rendering
    requests into RenderDebugText messages and sends them
    off once per frame to the render thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/string.h"
#include "math/float4.h"
#include "math/float2.h"
#include "math/point.h"
#include "coregraphics/textelement.h"

//------------------------------------------------------------------------------
namespace Debug
{
class DebugTextRenderer : public Core::RefCounted
{
    __DeclareClass(DebugTextRenderer);
    __DeclareSingleton(DebugTextRenderer);

public:
    /// constructor
    DebugTextRenderer();
    /// destructor
    virtual ~DebugTextRenderer();
    /// draw text
    void DrawText(const Util::String& text, const Math::float4& color, const Math::float2& pos);
    /// draw text
    void DrawText3D(const Util::String& text, const Math::float4& color, const Math::point& pos);
    /// call once per frame to send of accumulated draw commands
    void OnFrame();

private:
    Util::Array<CoreGraphics::TextElement> textElements;
};

} // namespace Debug
//------------------------------------------------------------------------------