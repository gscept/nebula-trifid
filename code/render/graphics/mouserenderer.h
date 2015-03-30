#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::MouseRenderer
    
    Main-thread proxy for the MouseRenderDevice (renders mouse pointers).
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/mousepointer.h"
#include "resources/resourceid.h"
#include "util/array.h"

//------------------------------------------------------------------------------
namespace Graphics
{
typedef CoreGraphics::MousePointer MousePointer;

class MouseRenderer : public Core::RefCounted
{
    __DeclareClass(MouseRenderer);
    __DeclareSingleton(MouseRenderer);
public:
    /// constructor
    MouseRenderer();
    /// destructor
    virtual ~MouseRenderer();
    
    /// pre-load mouse pointer textures
    void PreloadTextures(const Util::Array<Resources::ResourceId>& resIds);
    /// update mouse pointers
    void UpdatePointers(const Util::Array<MousePointer>& pointers);
};

} // namespace Graphics
//------------------------------------------------------------------------------
    