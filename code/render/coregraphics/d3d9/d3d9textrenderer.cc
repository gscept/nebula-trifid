//------------------------------------------------------------------------------
//  d3d9textrenderer.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9textrenderer.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/displaydevice.h"
#include "threading/thread.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9TextRenderer, 'D9TR', Base::TextRendererBase);
__ImplementSingleton(Direct3D9::D3D9TextRenderer);

using namespace CoreGraphics;
using namespace Threading;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
D3D9TextRenderer::D3D9TextRenderer():
    d3dFont(0),
    d3dSprite(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
D3D9TextRenderer::~D3D9TextRenderer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9TextRenderer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(0 == this->d3dFont);
    n_assert(0 == this->d3dSprite);

    // call parent
    Base::TextRendererBase::Open();

    // setup D3DX font object
    HRESULT hr;
    IDirect3DDevice9* d3d9Dev = RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Dev);
    hr = D3DXCreateFont(d3d9Dev,                    // pDevice
                        14,                         // Height
                        0,                          // Width
                        FW_NORMAL,                  // Weight
                        0,                          // MipLevels
                        FALSE,                      // Italic
                        DEFAULT_CHARSET,            // CharSet
                        OUT_DEFAULT_PRECIS,         // OutputPrecision
                        NONANTIALIASED_QUALITY,     // Quality
                        DEFAULT_PITCH|FF_DONTCARE,  // PitchAndFamily
                        "Arial",                    // pFaceName
                        &(this->d3dFont));
    n_assert(SUCCEEDED(hr));
    this->d3dFont->PreloadGlyphs(32, 255);

    // create sprite object for batched rendering
    hr = D3DXCreateSprite(d3d9Dev, &(this->d3dSprite));
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9TextRenderer::Close()
{
    n_assert(this->IsOpen());
    n_assert(0 != this->d3dFont);
    n_assert(0 != this->d3dSprite);

    // release d3d resources
    this->d3dFont->Release();
    this->d3dFont = 0;
    this->d3dSprite->Release();
    this->d3dSprite = 0;

    // call base class
    Base::TextRendererBase::Close();
}

//------------------------------------------------------------------------------
/**
    Draw buffered text. This method is called once per frame.
*/
void
D3D9TextRenderer::DrawTextElements()
{
    n_assert(this->IsOpen());
    const DisplayMode& displayMode = DisplayDevice::Instance()->GetDisplayMode();
   
    if (this->textElements.Size() > 0)
    {
        // begin batched rendering
        this->d3dSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
        
        // draw text elements
        IndexT i;
        for (i = 0; i < this->textElements.Size(); i++)
        {
            const TextElement& curTextElm = this->textElements[i];
            const float4& color = curTextElm.GetColor();
            DWORD d3dColor = D3DCOLOR_COLORVALUE(color.x(), color.y(), color.z(), color.w());
            RECT rect;
            rect.left = LONG(curTextElm.GetPosition().x() * float(displayMode.GetWidth()));
            rect.top  = LONG(curTextElm.GetPosition().y() * float(displayMode.GetHeight()));
            rect.bottom = rect.top;
            rect.right  = rect.left;
            this->d3dFont->DrawText(this->d3dSprite,                    // pSprite
                                    curTextElm.GetText().AsCharPtr(),   // pString
                                    -1,                                 // Count (number of characters)
                                    &rect,                              // pRect
                                    DT_LEFT|DT_TOP|DT_NOCLIP,           // Format
                                    d3dColor);                          // Color
        }
        // finish batched rendering
        this->d3dSprite->End();

        // delete the text elemenets of my own thread id, all other text elements
        // are from other threads and will be deleted through DeleteTextByThreadId()
        this->DeleteTextElementsByThreadId(Thread::GetMyThreadId());
    }
}

} // namespace Direct3D9

