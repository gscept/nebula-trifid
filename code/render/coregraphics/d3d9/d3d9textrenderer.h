#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9TextRenderer
  
    Implements a simple text renderer for Direct3D9. This is only intended
    for outputting debug text, not for high-quality text rendering!

    FIXME: Need to handle Lost Device (ID3DXFont)
    
    (C) 2007 Radon Labs GmbH
*/    
#include "coregraphics/base/textrendererbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9TextRenderer : public Base::TextRendererBase
{
    __DeclareClass(D3D9TextRenderer);
    __DeclareSingleton(D3D9TextRenderer);
public:
    /// constructor
    D3D9TextRenderer();
    /// destructor
    virtual ~D3D9TextRenderer();

    /// open the device
    void Open();
    /// close the device
    void Close();
    /// draw the accumulated text
    void DrawTextElements();

private:
    ID3DXFont* d3dFont;
    ID3DXSprite* d3dSprite;
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
