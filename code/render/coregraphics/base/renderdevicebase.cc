//------------------------------------------------------------------------------
//  renderdevicebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/renderdevicebase.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/multiplerendertarget.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/feedbackbuffer.h"
#include "coregraphics/shaderinstance.h"
#include "frame/frameserver.h"
#include "coregraphics/displaydevice.h"
#include "math/scalar.h"
#include "graphics/graphicsserver.h"

namespace Base
{
__ImplementClass(Base::RenderDeviceBase, 'RNDB', Core::RefCounted);
__ImplementSingleton(Base::RenderDeviceBase);

using namespace Util;
using namespace CoreGraphics;
using namespace Graphics;

Util::Queue<RenderDeviceBase::__BufferLockData> RenderDevice::bufferLockQueue;
//------------------------------------------------------------------------------
/**
*/
RenderDeviceBase::RenderDeviceBase() :
    isOpen(false),
    inNotifyEventHandlers(false),
    inBeginFrame(false),
    inBeginPass(false),
	inBeginFeedback(false),
    inBeginBatch(false),
	renderWireframe(false),
    visualizeMipMaps(false),
	usePatches(false),
	currentFrameIndex(InvalidIndex)
{
    __ConstructSingleton;
    _setup_grouped_counter(RenderDeviceNumPrimitives, "Render");
	_setup_grouped_counter(RenderDeviceNumDrawCalls, "Render");
	_setup_grouped_counter(RenderDeviceNumComputes, "Render");
    Memory::Clear(this->streamVertexOffsets, sizeof(this->streamVertexOffsets));
}

//------------------------------------------------------------------------------
/**
*/
RenderDeviceBase::~RenderDeviceBase()
{
    n_assert(!this->IsOpen());
    n_assert(this->eventHandlers.IsEmpty());

    _discard_counter(RenderDeviceNumPrimitives);
    _discard_counter(RenderDeviceNumDrawCalls);
    _discard_counter(RenderDeviceNumComputes);
    
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    This static method can be used to check whether a RenderDevice 
    object can be created on this machine before actually instantiating
    the device object (for instance by checking whether the right Direct3D
    version is installed). Use this method at application startup
    to check if the application should run at all.
*/
bool
RenderDeviceBase::CanCreate()
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Override the default render target (which is normally created in Open())
    with a render target provided by the application, this is normally only
    useful for debugging and testing purposes.
*/
void
RenderDeviceBase::SetOverrideDefaultRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt)
{
    n_assert(!this->isOpen);
    n_assert(!this->defaultRenderTarget.isvalid());
    this->defaultRenderTarget = rt;
}

//------------------------------------------------------------------------------
/**
*/
bool
RenderDeviceBase::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    this->isOpen = true;

    // notify event handlers
    RenderEvent openEvent(RenderEvent::DeviceOpen);
    this->NotifyEventHandlers(openEvent);

    // create default render target (if not overriden by application
    if (!this->defaultRenderTarget.isvalid())
    {
        this->defaultRenderTarget = RenderTarget::Create();
        this->defaultRenderTarget->SetDefaultRenderTarget(true);
        this->defaultRenderTarget->Setup();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::Close()
{
    n_assert(this->IsOpen());
    n_assert(!this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);

    // release default render target
    if (this->defaultRenderTarget->IsValid())
    {
        this->defaultRenderTarget->Discard();
    }
    this->defaultRenderTarget = 0;

	// clear buffer locks
	this->bufferLockQueue.Clear();

    // notify event handlers
    RenderEvent closeEvent(RenderEvent::DeviceClose);
    this->NotifyEventHandlers(closeEvent);

	// unreference shaders
	this->passShader = 0;

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
RenderDeviceBase::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::SetStreamVertexBuffer(IndexT streamIndex, const Ptr<VertexBuffer>& vb, IndexT offsetVertexIndex)
{
    n_assert((streamIndex >= 0) && (streamIndex < MaxNumVertexStreams));
    this->streamVertexBuffers[streamIndex] = vb;
    this->streamVertexOffsets[streamIndex] = offsetVertexIndex;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<VertexBuffer>&
RenderDeviceBase::GetStreamVertexBuffer(IndexT streamIndex) const
{
    n_assert((streamIndex >= 0) && (streamIndex < MaxNumVertexStreams));
    return this->streamVertexBuffers[streamIndex];
}

//------------------------------------------------------------------------------
/**
*/
IndexT
RenderDeviceBase::GetStreamVertexOffset(IndexT streamIndex) const
{
    n_assert((streamIndex >= 0) && (streamIndex < MaxNumVertexStreams));
    return this->streamVertexOffsets[streamIndex];
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::SetVertexLayout(const Ptr<VertexLayout>& vl)
{
    this->vertexLayout = vl;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<VertexLayout>&
RenderDeviceBase::GetVertexLayout() const
{
    return this->vertexLayout;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::SetIndexBuffer(const Ptr<IndexBuffer>& ib)
{
    this->indexBuffer = ib;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<IndexBuffer>&
RenderDeviceBase::GetIndexBuffer() const
{
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
    Attach an event handler to the render device.
*/
void
RenderDeviceBase::AttachEventHandler(const Ptr<RenderEventHandler>& handler)
{
    n_assert(handler.isvalid());
    n_assert(InvalidIndex == this->eventHandlers.FindIndex(handler));
    n_assert(!this->inNotifyEventHandlers);
    this->eventHandlers.Append(handler);
    handler->OnAttach();
}

//------------------------------------------------------------------------------
/**
    Remove an event handler from the display device.
*/
void
RenderDeviceBase::RemoveEventHandler(const Ptr<RenderEventHandler>& handler)
{
    n_assert(handler.isvalid());
    n_assert(!this->inNotifyEventHandlers);
    IndexT index = this->eventHandlers.FindIndex(handler);
    n_assert(InvalidIndex != index);
    this->eventHandlers.EraseIndex(index);
    handler->OnRemove();
}

//------------------------------------------------------------------------------
/**
    Notify all event handlers about an event.
*/
bool
RenderDeviceBase::NotifyEventHandlers(const RenderEvent& event)
{
    n_assert(!this->inNotifyEventHandlers);
    bool handled = false;
    this->inNotifyEventHandlers = true;
    IndexT i;
    for (i = 0; i < this->eventHandlers.Size(); i++)
    {
        handled |= this->eventHandlers[i]->PutEvent(event);
    }
    this->inNotifyEventHandlers = false;
    return handled;
}

//------------------------------------------------------------------------------
/**
*/
bool
RenderDeviceBase::BeginFrame(IndexT frameIndex)
{
    n_assert(!this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    n_assert(!this->indexBuffer.isvalid());
    n_assert(!this->vertexLayout.isvalid());
    IndexT i;
    for (i = 0; i < MaxNumVertexStreams; i++)
    {
        n_assert(!this->streamVertexBuffers[i].isvalid());
    }

	if (frameIndex != this->currentFrameIndex)
	{
		_begin_counter(RenderDeviceNumComputes);
		_begin_counter(RenderDeviceNumPrimitives);
		_begin_counter(RenderDeviceNumDrawCalls);
	}

    this->inBeginFrame = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::BeginPass(const Ptr<RenderTarget>& rt, const Ptr<Shader>& shd)
{
    n_assert(this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    n_assert(!this->passShader.isvalid());
    this->inBeginPass = true;

    // apply pass shader
    this->passShader = shd;
    if (this->passShader.isvalid())
    {
        //this->passShader->Commit();
    }

	// notify render targets
    this->passRenderTarget = rt;
    this->passRenderTarget->BeginPass();    
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderDeviceBase::BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt, const Ptr<CoreGraphics::Shader>& shd)
{
    n_assert(this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    n_assert(!this->passShader.isvalid());
    this->inBeginPass = true;

    // apply pass shader
    this->passShader = shd;
    if (this->passShader.isvalid())
    {
        //this->passShader->Commit();
    }

    // notify render targets
    this->passMultipleRenderTarget = mrt;
    this->passMultipleRenderTarget->BeginPass();    
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderDeviceBase::BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& crt, const Ptr<CoreGraphics::Shader>& shd)
{
    n_assert(this->inBeginFrame);
    n_assert(!this->inBeginPass);
    n_assert(!this->inBeginBatch);
    n_assert(!this->passShader.isvalid());
    this->inBeginPass = true;

    // apply pass shader
    this->passShader = shd;
    if (this->passShader.isvalid())
    {
        //this->passShader->Commit();
    }

    // notify render targets
    this->passRenderTargetCube = crt;
    this->passRenderTargetCube->BeginPass();    
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::BeginFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb, CoreGraphics::PrimitiveTopology::Code primType, const Ptr<CoreGraphics::Shader>& shader)
{
	n_assert(this->inBeginFrame);
	n_assert(!this->inBeginPass);
	n_assert(!this->passShader.isvalid());
	this->inBeginPass = true;

	// apply pass shader
	this->passShader = shader;
	if (this->passShader.isvalid())
	{
		//this->passShader->Commit();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::BeginBatch(FrameBatchType::Code batchType)
{
    n_assert(this->inBeginPass);
    n_assert(!this->inBeginBatch);
    n_assert(this->passRenderTarget.isvalid() || this->passMultipleRenderTarget.isvalid() || this->passRenderTargetCube.isvalid());
    this->inBeginBatch = true;

    if (this->passRenderTarget.isvalid())
    {
        // notify render target
        this->passRenderTarget->BeginBatch(batchType);    
    }
    else if (this->passMultipleRenderTarget.isvalid())
    {
        // notify multiple render target
        this->passMultipleRenderTarget->BeginBatch(batchType);    
    }
    else if (this->passRenderTargetCube.isvalid())
    {
        this->passRenderTargetCube->BeginBatch(batchType);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EndBatch()
{
    n_assert(this->inBeginBatch);
    n_assert(this->passRenderTarget.isvalid() || this->passMultipleRenderTarget.isvalid() || this->passRenderTargetCube.isvalid());
    this->inBeginBatch = false;

    // notify render targets
    if (this->passRenderTarget.isvalid())
    {
        this->passRenderTarget->EndBatch();
    }
    else if (this->passMultipleRenderTarget.isvalid())
    {
        this->passMultipleRenderTarget->EndBatch();
    }
    else if (this->passRenderTargetCube.isvalid())
    {
        this->passRenderTargetCube->EndBatch();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EndPass()
{
    n_assert(this->inBeginPass);
    n_assert(this->passRenderTarget.isvalid() || this->passMultipleRenderTarget.isvalid() || this->passRenderTargetCube.isvalid());

	// finish rendering to depth-stencil
	if (this->passDepthStencilTarget.isvalid())
	{
		this->passDepthStencilTarget->EndPass();
		this->passDepthStencilTarget = 0;
	}

    // finish rendering to render target
    if (this->passRenderTarget.isvalid())
    {        
        this->passRenderTarget->EndPass();
        this->passRenderTarget = 0;
    }
    else if (this->passMultipleRenderTarget.isvalid())
    {
        this->passMultipleRenderTarget->EndPass();
        this->passMultipleRenderTarget = 0;
    }
    else if (this->passRenderTargetCube.isvalid())
    {
        this->passRenderTargetCube->EndPass();
        this->passRenderTargetCube = 0;
    }

    // finish pass shader
    if (this->passShader.isvalid())
    {
        this->passShader = 0;
    }
    this->inBeginPass = false;
}


//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EndFeedback()
{
	n_assert(this->inBeginPass);

	// finish pass shader
	if (this->passShader.isvalid())
	{
		this->passShader = 0;
	}
	this->inBeginPass = false;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EndFrame(IndexT frameIndex)
{
    n_assert(this->inBeginFrame);

	if (this->currentFrameIndex != frameIndex)
	{
		_end_counter(RenderDeviceNumComputes);
		_end_counter(RenderDeviceNumPrimitives);
		_end_counter(RenderDeviceNumDrawCalls);
		this->currentFrameIndex = frameIndex;
	}	
    
    this->inBeginFrame = false;
    IndexT i;
    for (i = 0; i < MaxNumVertexStreams; i++)
    {
        this->streamVertexBuffers[i] = 0;
    }
    this->vertexLayout = 0;
    this->indexBuffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::Present()
{
    n_assert(!this->inBeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::Draw()
{
    n_assert(this->inBeginPass);
    // override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::DrawIndexedInstanced(SizeT numInstances, IndexT baseInstance)
{
    n_assert(this->inBeginPass);
    // override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::DrawFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb)
{
	n_assert(this->inBeginPass);
	// override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::DrawFeedbackInstanced(const Ptr<CoreGraphics::FeedbackBuffer>& fb, SizeT numInstances)
{
	n_assert(this->inBeginPass);
	// override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderDeviceBase::Compute(int dimX, int dimY, int dimZ, uint flag)
{
    n_assert(this->inBeginPass);
    // override in subclass!
}

//------------------------------------------------------------------------------
/**
*/
ImageFileFormat::Code
RenderDeviceBase::SaveScreenshot(ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream)
{
    // override in subclass!
    return fmt;    
}

//------------------------------------------------------------------------------
/**
*/
ImageFileFormat::Code
RenderDeviceBase::SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream, const Math::rectangle<int>& rect, int x, int y)
{
	// override in subclass!
	return fmt;
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderDeviceBase::DisplayResized(SizeT width, SizeT height)
{
    n_assert(this->IsOpen());

	// update frame server
	Frame::FrameServer::Instance()->DisplayResized(width, height);

	// update main camera
	GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity()->OnDisplayResized();

    // notify rt plugins that the display has been resized
    RenderModules::RTPluginRegistry::Instance()->OnWindowResized(width, height);

	// also update the default render target
	this->defaultRenderTarget->OnDisplayResized(width, height);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EnqueueBufferLockIndex(const Ptr<CoreGraphics::BufferLock>& lock, IndexT buffer)
{
	__BufferLockData data;
	data.mode = __BufferLockMode::BufferRing;
	data.i = buffer;
	data.lock = lock;
	RenderDeviceBase::bufferLockQueue.Enqueue(data);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::EnqueueBufferLockRange(const Ptr<CoreGraphics::BufferLock>& lock, IndexT start, SizeT range)
{
	__BufferLockData data;
	data.mode = __BufferLockMode::BufferRing;
	data.start = start;
	data.range = range;
	data.lock = lock;
	RenderDeviceBase::bufferLockQueue.Enqueue(data);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderDeviceBase::DequeueBufferLocks()
{
	IndexT i;
	for (i = 0; i < RenderDeviceBase::bufferLockQueue.Size(); i++)
	{
		const __BufferLockData& data = RenderDeviceBase::bufferLockQueue[i];
		switch (data.mode)
		{
		case __BufferLockMode::BufferRing:
			data.lock->LockBuffer(data.i);
			break;
		case __BufferLockMode::BufferRange:
			data.lock->LockRange(data.start, data.range);
			break;
		}
	}
	RenderDeviceBase::bufferLockQueue.Clear();
}

} // namespace Base
