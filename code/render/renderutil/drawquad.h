#pragma once
//------------------------------------------------------------------------------
/**
    @class RenderUtil::DrawQuad
    
    Util class for rendering a quad.
    
    (C) 2013 Gustav Sterbrant
*/
#include "coregraphics/shaderstate.h"
#include "coregraphics/shadervariable.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/primitivegroup.h"

//------------------------------------------------------------------------------
namespace RenderUtil
{
class DrawQuad
{
public:
    /// constructor
    DrawQuad();
    /// destructor
    ~DrawQuad();

    /// setup the object
    void Setup(SizeT width, SizeT height);
    /// discard the object
    void Discard();
    /// return true if object is valid
    bool IsValid() const;
    /// draw the fullscreen quad
    void Draw();

private:
    Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
    CoreGraphics::PrimitiveGroup primGroup;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
DrawQuad::IsValid() const
{
    return this->isValid;
}

} // namespace RnederUtil
//------------------------------------------------------------------------------
