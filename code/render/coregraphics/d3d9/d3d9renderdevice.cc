//------------------------------------------------------------------------------
//  d3d9renderdevice.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/config.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/win360/d3d9types.h"

#include <dxerr.h>

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9RenderDevice, 'D9RD', Base::RenderDeviceBase);
__ImplementSingleton(Direct3D9::D3D9RenderDevice);

using namespace Win360;
using namespace CoreGraphics;

IDirect3D9* D3D9RenderDevice::d3d9 = 0;

//------------------------------------------------------------------------------
/**
*/
D3D9RenderDevice::D3D9RenderDevice() :
    d3d9Device(0),
    adapter(0),
    displayFormat(D3DFMT_X8R8G8B8),
    deviceBehaviourFlags(0),
    frameId(0)
{
    __ConstructSingleton;
    Memory::Clear(&this->presentParams, sizeof(this->presentParams));
    Memory::Clear(&this->d3d9DeviceCaps, sizeof(this->d3d9DeviceCaps));
    this->OpenDirect3D();
}

//------------------------------------------------------------------------------
/**
*/
D3D9RenderDevice::~D3D9RenderDevice()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    this->CloseDirect3D();
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Get a pointer to the Direct3D interface. Opens Direct3D if not
    happened yet.
*/
IDirect3D9*
D3D9RenderDevice::GetDirect3D()
{
    OpenDirect3D();
    n_assert(0 != d3d9);
    return d3d9;
}

//------------------------------------------------------------------------------
/**
    Test if the right Direct3D version is installed by trying to open
    Direct3D.
*/
bool
D3D9RenderDevice::CanCreate()
{
    OpenDirect3D();
    return (0 != d3d9);
}

//------------------------------------------------------------------------------
/**
    Return a pointer to d3d device. Asserts that the device exists.
*/
IDirect3DDevice9*
D3D9RenderDevice::GetDirect3DDevice() const
{
    n_assert(0 != this->d3d9Device);
    return this->d3d9Device;
}

//------------------------------------------------------------------------------
/**
    Open Direct3D. This method is called by the constructor. If you're not
    sure whether Direct3D is supported on this machine, use the static
    CanCreate() method to check before creating the D3D9RenderDevice object.
    Note that this method may also be called by D3D9DisplayDevice because
    it needs access to the machine's adapters.
*/
void
D3D9RenderDevice::OpenDirect3D()
{
    if (0 == d3d9)
    {
        d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    }
}

//------------------------------------------------------------------------------
/**
    Close Direct3D9. This method is exclusively called by the destructor.
*/
void
D3D9RenderDevice::CloseDirect3D()
{
    if (0 != d3d9)
    {
        d3d9->Release();
        d3d9 = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Open the render device. When successful, the RenderEvent::DeviceOpen
    will be sent to all registered event handlers after the Direct3D
    device has been opened.
*/
bool
D3D9RenderDevice::Open()
{
    n_assert(!this->IsOpen());
    bool success = false;
    if (this->OpenDirect3DDevice())
    {
        // hand to parent class, this will notify event handlers
        success = RenderDeviceBase::Open();
    }
    return success;
}

//------------------------------------------------------------------------------
/**
    Close the render device. The RenderEvent::DeviceClose will be sent to
    all registered event handlers.
*/
void
D3D9RenderDevice::Close()
{
    n_assert(this->IsOpen());
    RenderDeviceBase::Close();
    this->CloseDirect3DDevice();
}

//------------------------------------------------------------------------------
/**
    This selects the adapter to use for the render device. This
    method will set the "adapter" member and the d3d9 device caps
    member.
*/
void
D3D9RenderDevice::SetupAdapter()
{
    n_assert(0 != this->d3d9);

    DisplayDevice* displayDevice = DisplayDevice::Instance();
    Adapter::Code requestedAdapter = displayDevice->GetAdapter();
    n_assert(displayDevice->AdapterExists(requestedAdapter));

    #if NEBULA3_DIRECT3D_USENVPERFHUD
        this->adapter = this->d3d9->GetAdapterCount() - 1;
    #else
        this->adapter = (UINT) requestedAdapter;
    #endif

    HRESULT hr = this->d3d9->GetDeviceCaps(this->adapter, NEBULA3_DIRECT3D_DEVICETYPE, &(this->d3d9DeviceCaps));
    n_assert(SUCCEEDED(hr));
}

//------------------------------------------------------------------------------
/**
    Select the display, back buffer and depth buffer formats and update
    the presentParams member. 
*/
void
D3D9RenderDevice::SetupBufferFormats()
{
    HRESULT hr;
    n_assert(0 != this->d3d9);
    DisplayDevice* displayDevice = DisplayDevice::Instance();

    if (displayDevice->IsFullscreen())
    {
        if (displayDevice->IsTripleBufferingEnabled())
        {
            this->presentParams.BackBufferCount = 2;
        }
        else
        {
            this->presentParams.BackBufferCount = 1;
        }
        if (displayDevice->IsDisplayModeSwitchEnabled())
        {
            this->presentParams.Windowed = FALSE;
        }
        else
        {
            this->presentParams.Windowed = TRUE;
        }
    }
    else
    {
        // windowed mode
        this->presentParams.BackBufferCount = 1;
        this->presentParams.Windowed = TRUE;
    }

    D3DFORMAT backbufferPixelFormat;
    if (this->presentParams.Windowed)
    {
        // windowed mode: use desktop pixel format
        backbufferPixelFormat = D3D9Types::AsD3D9PixelFormat(displayDevice->GetCurrentAdapterDisplayMode((Adapter::Code)this->adapter).GetPixelFormat());
    }
    else
    {
        // fullscreen: use requested pixel format
        backbufferPixelFormat = D3D9Types::AsD3D9PixelFormat(displayDevice->GetDisplayMode().GetPixelFormat());
    }

    // make sure the device supports a D24S8 depth buffers
    hr = this->d3d9->CheckDeviceFormat(this->adapter,
                                       NEBULA3_DIRECT3D_DEVICETYPE,
                                       backbufferPixelFormat,
                                       D3DUSAGE_DEPTHSTENCIL,
                                       D3DRTYPE_SURFACE,
                                       D3DFMT_D24S8);
    if (FAILED(hr))
    {
        n_error("Rendering device doesn't support D24S8 depth buffer!\n");
        return;
    }

    // check that the depth buffer format is compatible with the backbuffer format
    hr = this->d3d9->CheckDepthStencilMatch(this->adapter,
                                            NEBULA3_DIRECT3D_DEVICETYPE,
                                            backbufferPixelFormat,
                                            backbufferPixelFormat,
                                            D3DFMT_D24S8);
    if (FAILED(hr))
    {
        n_error("Backbuffer format not compatible with D24S8 depth buffer!\n");
        return;
    }

    // fill presentParams
    this->presentParams.BackBufferFormat = backbufferPixelFormat;
    this->presentParams.BackBufferWidth = displayDevice->GetDisplayMode().GetWidth();
    this->presentParams.BackBufferHeight = displayDevice->GetDisplayMode().GetHeight();
    this->presentParams.EnableAutoDepthStencil = FALSE;
    this->presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
}

//------------------------------------------------------------------------------
/**
    Set the deviceBehaviour member to a suitable value.
*/
void
D3D9RenderDevice::SetupDeviceBehaviour()
{
    #if NEBULA3_DIRECT3D_DEBUG
        this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    #else
        this->deviceBehaviourFlags = D3DCREATE_FPU_PRESERVE;
        if (this->d3d9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            this->deviceBehaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        }
        else
        {
            this->deviceBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
    #endif
}

//------------------------------------------------------------------------------
/**
    Setup the (remaining) presentation parameters. This will initialize
    the presentParams member.
*/
void
D3D9RenderDevice::SetupPresentParams()
{
    DisplayDevice* displayDevice = DisplayDevice::Instance();
    n_assert(displayDevice->IsOpen());

    #if NEBULA3_DIRECT3D_DEBUG
        this->presentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    #else
        this->presentParams.Flags = 0;
    #endif

    if (displayDevice->IsVerticalSyncEnabled())
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }
    else
    {
        this->presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    this->presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    this->presentParams.hDeviceWindow = displayDevice->GetHwnd();
    this->presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    this->presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    this->presentParams.MultiSampleQuality = 0;
}

//------------------------------------------------------------------------------
/**
    Initialize the Direct3D device with initial device state.
*/
void
D3D9RenderDevice::SetInitialDeviceState()
{
    n_assert(this->d3d9Device);

    this->d3d9Device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
    this->d3d9Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    // setup viewport
    D3DVIEWPORT9 viewPort;
    viewPort.Width = this->presentParams.BackBufferWidth;
    viewPort.Height = this->presentParams.BackBufferHeight;
    viewPort.X = 0;
    viewPort.Y = 0;
    viewPort.MinZ = 0.0f;
    viewPort.MaxZ = 1.0f;
    this->d3d9Device->SetViewport(&viewPort);

}

//------------------------------------------------------------------------------
/**
    Open the Direct3D device. This will completely setup the device
    into a usable state.
*/
bool
D3D9RenderDevice::OpenDirect3DDevice()
{
    n_assert(0 != this->d3d9);
    n_assert(0 == this->d3d9Device);
    HRESULT hr;
    DisplayDevice* displayDevice = DisplayDevice::Instance();
    n_assert(displayDevice->IsOpen());

    // setup device creation parameters
    Memory::Clear(&this->presentParams, sizeof(this->presentParams));
    this->SetupAdapter();
    this->SetupBufferFormats();
    this->SetupDeviceBehaviour();
    this->SetupPresentParams();
    
    // create device
    hr = this->d3d9->CreateDevice(this->adapter, 
                                  NEBULA3_DIRECT3D_DEVICETYPE,
                                  displayDevice->GetHwnd(),
                                  this->deviceBehaviourFlags,
                                  &(this->presentParams),
                                  &(this->d3d9Device));
    if (FAILED(hr))
    {
        n_error("Failed to create Direct3D device object: %s!\n", DXGetErrorString(hr));
        return false;
    }

    // get actual device caps
    this->d3d9Device->GetDeviceCaps(&this->d3d9DeviceCaps);

    // set initial device state
    this->SetInitialDeviceState();

    // create double buffer query to avoid gpu to render more than 1 frame ahead
    IndexT i;
    for (i = 0; i < numSyncQueries; ++i)
    {
        this->d3d9Device->CreateQuery(D3DQUERYTYPE_EVENT, &this->gpuSyncQuery[i]);  	
    }         

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the Direct3D device.
*/
void
D3D9RenderDevice::CloseDirect3DDevice()
{
    n_assert(0 != this->d3d9);
    n_assert(0 != this->d3d9Device);

    this->UnbindD3D9Resources();   
    IndexT i;
    for (i = 1; i < 4; i++)
    {
        this->d3d9Device->SetRenderTarget(i, NULL);
    }
    this->d3d9Device->SetDepthStencilSurface(NULL);

    // release queries
    for (i = 0; i < numSyncQueries; ++i)
    {
        this->gpuSyncQuery[i]->Release();  	
    }

    // release the Direct3D device
    this->d3d9Device->Release();
    this->d3d9Device = 0;
}

//------------------------------------------------------------------------------
/**
    This catches the lost device state, and tries to restore a lost device.
    The method will send out the events DeviceLost and DeviceRestored.
    Resources should react to these events accordingly. As long as
    the device is in an invalid state, the method will return false.
    This method is called by BeginFrame().
*/
bool
D3D9RenderDevice::TestResetDevice()
{
    n_assert(0 != this->d3d9Device);

    HRESULT hr = this->d3d9Device->TestCooperativeLevel();
    if (SUCCEEDED(hr))
    {
        // everything is ok
        return true;
    }
    else if (D3DERR_DEVICENOTRESET == hr)
    {
        // notify event handlers that the device was lost
        this->NotifyEventHandlers(RenderEvent(RenderEvent::DeviceLost));

        // if we are in windowed mode, the cause for the lost
        // device may be a desktop display mode switch, in this
        // case we need to find new buffer formats
        if (this->presentParams.Windowed)
        {
            this->SetupBufferFormats();
        }

        // try to reset the device
        hr = this->d3d9Device->Reset(&this->presentParams);
        if (SUCCEEDED(hr))
        {
            // set initial device state
            this->SetInitialDeviceState();

            // send the DeviceRestored event
            this->NotifyEventHandlers(RenderEvent(RenderEvent::DeviceRestored));
        }
    }
    // the device cannot be restored at this time
    return false;
}

//------------------------------------------------------------------------------
/**
    Begin a complete frame. Call this once per frame before any rendering 
    happens. If rendering is not possible for some reason (e.g. a lost
    device) the method will return false. This method may also send
    the DeviceLost and DeviceRestored RenderEvents to attached event
    handlers.
*/
bool
D3D9RenderDevice::BeginFrame()
{
//		Direct3D9::D3D9RenderDevice::Instance()->GetDirect3DDevice()->ShowCursor(false);
//	bool foo = this->d3d9Device->ShowCursor(false);
    n_assert(this->d3d9Device);
    if (RenderDeviceBase::BeginFrame())
    {
        // check for and handle lost device
        if (!this->TestResetDevice())
        {
            this->inBeginFrame = false;
            return false;
        }
        HRESULT hr = this->d3d9Device->BeginScene();
        n_assert(SUCCEEDED(hr));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    End a complete frame. Call this once per frame after rendering
    has happened and before Present(), and only if BeginFrame() returns true.
*/
void
D3D9RenderDevice::EndFrame()
{
    RenderDeviceBase::EndFrame();
    HRESULT hr = this->d3d9Device->EndScene();
    n_assert(SUCCEEDED(hr));
    this->UnbindD3D9Resources();
}

//------------------------------------------------------------------------------
/**
    End the current rendering pass. This will flush all texture stages
    in order to keep the d3d9 resource reference counts consistent without too
    much hassle.
*/
void
D3D9RenderDevice::EndPass()
{
    this->UnbindD3D9Resources();
    RenderDeviceBase::EndPass();
}

//------------------------------------------------------------------------------
/**
    NOTE: Present() should be called as late as possible after EndFrame()
    to improve parallelism between the GPU and the CPU.
*/
void
D3D9RenderDevice::Present()
{
    RenderDeviceBase::Present();
          
    // present backbuffer...
    n_assert(0 != this->d3d9Device);
    if (0 != D3D9DisplayDevice::Instance()->GetHwnd())
    {
        HRESULT hr = this->d3d9Device->Present(NULL, NULL, 0, NULL);
        n_assert(SUCCEEDED(hr));
    }    

    // sync cpu thread with gpu
    this->SyncGPU();    
}

//------------------------------------------------------------------------------
/**
    Sets the vertex buffer to use for the next Draw().
*/
void
D3D9RenderDevice::SetStreamSource(IndexT streamIndex, const Ptr<VertexBuffer>& vb, IndexT offsetVertexIndex)
{
    n_assert((streamIndex >= 0) && (streamIndex < MaxNumVertexStreams));
    n_assert(this->inBeginPass);
    n_assert(0 != this->d3d9Device);    
    n_assert(vb.isvalid());
    if ((this->streamVertexBuffers[streamIndex] != vb) || (this->streamVertexOffsets[streamIndex] != offsetVertexIndex))
    {
        HRESULT hr;
        IDirect3DVertexBuffer9* d3d9VertexBuffer = vb->GetD3D9VertexBuffer();
        DWORD vertexByteSize = vb->GetVertexLayout()->GetVertexByteSize();
        DWORD vertexByteOffset = offsetVertexIndex * vertexByteSize;
        hr = this->d3d9Device->SetStreamSource(streamIndex, d3d9VertexBuffer, vertexByteOffset, vertexByteSize);
        n_assert(SUCCEEDED(hr));
    }
    RenderDeviceBase::SetStreamSource(streamIndex, vb, offsetVertexIndex);
}

//------------------------------------------------------------------------------
/**
    Sets the vertex layout for the next Draw()
*/
void
D3D9RenderDevice::SetVertexLayout(const Ptr<VertexLayout>& vl)
{
    n_assert(this->inBeginPass);
    n_assert(0 != this->d3d9Device);    
    n_assert(vl.isvalid());

    if (this->vertexLayout != vl)
    {
        HRESULT hr;
        IDirect3DVertexDeclaration9* d3d9VertexDecl = vl->GetD3D9VertexDeclaration();
        hr = this->d3d9Device->SetVertexDeclaration(d3d9VertexDecl);
        n_assert(SUCCEEDED(hr));
    }
    RenderDeviceBase::SetVertexLayout(vl);
}

//------------------------------------------------------------------------------
/**
    Sets the index buffer to use for the next Draw().
*/
void
D3D9RenderDevice::SetIndexBuffer(const Ptr<IndexBuffer>& ib)
{
    n_assert(this->inBeginPass);
    n_assert(0 != this->d3d9Device);
    n_assert(ib.isvalid());

    if (this->indexBuffer != ib)
    {
        HRESULT hr;

        // set the index buffer on the d3d device
        IDirect3DIndexBuffer9* d3d9IndexBuffer = ib->GetD3D9IndexBuffer();
        hr = this->d3d9Device->SetIndices(d3d9IndexBuffer);
        n_assert(SUCCEEDED(hr));
    }
    RenderDeviceBase::SetIndexBuffer(ib);
}

//------------------------------------------------------------------------------
/**
    Draw the current primitive group. Requires a vertex buffer, an
    optional index buffer and a primitive group to be set through the
    respective methods. To use non-indexed rendering, set the number
    of indices in the primitive group to 0.
*/
void
D3D9RenderDevice::Draw()
{
    n_assert(this->inBeginPass);
    n_assert(0 != this->d3d9Device);

    D3DPRIMITIVETYPE d3dPrimType = D3D9Types::AsD3D9PrimitiveType(this->primitiveGroup.GetPrimitiveTopology());
    if (this->primitiveGroup.GetNumIndices() > 0)
    {
        // use indexed rendering
        HRESULT hr;
        hr = this->d3d9Device->DrawIndexedPrimitive(
                d3dPrimType,                                        // Type
                0,                                                  // BaseVertexIndex
                this->primitiveGroup.GetBaseVertex(),               // MinIndex
                this->primitiveGroup.GetNumVertices(),              // NumVertices
                this->primitiveGroup.GetBaseIndex(),                // StartIndex
                this->primitiveGroup.GetNumPrimitives());           // PrimitiveCount
        n_assert(SUCCEEDED(hr));
    }
    else
    {
        // use non-indexed rendering
        HRESULT hr;
        hr = this->d3d9Device->DrawPrimitive(
                d3dPrimType,                                        // Type
                this->primitiveGroup.GetBaseVertex(),               // StartVertex
                this->primitiveGroup.GetNumPrimitives());           // PrimitiveCount
        n_assert(SUCCEEDED(hr));
    }

    // update debug stats
    _incr_counter(RenderDeviceNumPrimitives, this->primitiveGroup.GetNumPrimitives());
    _incr_counter(RenderDeviceNumDrawCalls, 1);
}

//------------------------------------------------------------------------------
/**
    Draw N instances of the current primitive group. Requires the following
    setup:
        
        - vertex stream 0: vertex buffer with instancing data, one vertex per instance
        - vertex stream 1: vertex buffer with instance geometry data
        - index buffer: index buffer for geometry data
        - primitive group: the primitive group which describes one instance
        - vertex declaration: describes a combined vertex from stream 0 and stream 1
*/
void
D3D9RenderDevice::DrawIndexedInstanced(SizeT numInstances)
{
    n_assert(this->inBeginPass);
    n_assert(numInstances > 0);
    n_assert(0 != this->d3d9Device);

    this->d3d9Device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numInstances);
    this->d3d9Device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

    D3DPRIMITIVETYPE d3dPrimType = D3D9Types::AsD3D9PrimitiveType(this->primitiveGroup.GetPrimitiveTopology());
    n_assert(this->primitiveGroup.GetNumIndices() > 0);

    HRESULT hr;
    hr = this->d3d9Device->DrawIndexedPrimitive(
            d3dPrimType,                                        // Type
            0,                                                  // BaseVertexIndex
            this->primitiveGroup.GetBaseVertex(),               // MinIndex
            this->primitiveGroup.GetNumVertices(),              // NumVertices
            this->primitiveGroup.GetBaseIndex(),                // StartIndex
            this->primitiveGroup.GetNumPrimitives());           // PrimitiveCount
    n_assert(SUCCEEDED(hr));

    this->d3d9Device->SetStreamSourceFreq(0, 1);
    this->d3d9Device->SetStreamSourceFreq(1, 1);

    // update debug stats
    _incr_counter(RenderDeviceNumPrimitives, this->primitiveGroup.GetNumPrimitives() * numInstances);
    _incr_counter(RenderDeviceNumDrawCalls, 1);
}

//------------------------------------------------------------------------------
/**
    Save the backbuffer to the provided stream.
*/
ImageFileFormat::Code
D3D9RenderDevice::SaveScreenshot(ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream)
{
    n_assert(!this->inBeginFrame);
    n_assert(0 != this->d3d9Device);
    HRESULT hr;

    // create a plain offscreen surface to capture data to
    IDirect3DSurface9* captureSurface = NULL;
    hr = this->d3d9Device->CreateOffscreenPlainSurface(this->presentParams.BackBufferWidth,     // Width    
                                                       this->presentParams.BackBufferHeight,    // Height
                                                       this->presentParams.BackBufferFormat,    // Format
                                                       D3DPOOL_SYSTEMMEM,                       // Pool
                                                       &captureSurface,                         // ppSurface
                                                       NULL);                                   // pSharedHandle
    n_assert(SUCCEEDED(hr));
    n_assert(0 != captureSurface);

    // get the back buffer surface
    IDirect3DSurface9* backBufferSurface = 0;
    hr = this->d3d9Device->GetBackBuffer(0,0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurface);
    n_assert(SUCCEEDED(hr));

    // capture backbuffer to offscreen surface
    hr = this->d3d9Device->GetRenderTargetData(backBufferSurface, captureSurface);
    n_assert(SUCCEEDED(hr));
    backBufferSurface->Release();

    // finally save the image to the provided stream
    D3DXIMAGE_FILEFORMAT d3dxFmt= D3D9Types::AsD3DXImageFileFormat(fmt);
    ID3DXBuffer* d3dxBuffer = 0;    
    hr = D3DXSaveSurfaceToFileInMemory(&d3dxBuffer,     // ppDestBuf
                                       d3dxFmt,         // DestFormat
                                       captureSurface,  // pSrcSurface,
                                       NULL,            // pSrcPalette
                                       NULL);           // pSrcRect
    n_assert(SUCCEEDED(hr));
    captureSurface->Release();

    // write result to stream
    void* dataPtr  = d3dxBuffer->GetBufferPointer();
    DWORD dataSize = d3dxBuffer->GetBufferSize();
    outStream->SetAccessMode(IO::Stream::WriteAccess);
    if (outStream->Open())
    {
        outStream->Write(dataPtr, dataSize);
        outStream->Close();
        outStream->SetMediaType(ImageFileFormat::ToMediaType(fmt));
    }
    d3dxBuffer->Release();
    return fmt;
}

//------------------------------------------------------------------------------
/**
    Unbind all d3d9 resources from the device, this is necessary to keep the
    resource reference counts consistent. Should be called at the
    end of each rendering pass.
*/
void
D3D9RenderDevice::UnbindD3D9Resources()
{
    this->d3d9Device->SetVertexShader(NULL);
    this->d3d9Device->SetPixelShader(NULL);
    this->d3d9Device->SetIndices(NULL);
    IndexT i;
    for (i = 0; i < 8; i++)
    {
        this->d3d9Device->SetTexture(i, NULL);
    }
    for (i = 0; i < MaxNumVertexStreams; i++)
    {
        this->d3d9Device->SetStreamSource(i, NULL, 0, 0);
        this->streamVertexBuffers[i] = 0;
    }
    this->indexBuffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
D3D9RenderDevice::SyncGPU()
{
    // sync with gpu, wait till command buffer is empty
    // add event to gpu command stream  
    this->gpuSyncQuery[this->frameId % numSyncQueries]->Issue(D3DISSUE_END);                              
    this->frameId++;
    while (S_FALSE == this->gpuSyncQuery[this->frameId % numSyncQueries]->GetData(NULL, 0, D3DGETDATA_FLUSH))
    {
        // wait till gpu has finsihed rendering the previous frame        
    }
}
} // namespace CoreGraphics
