#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::RenderDeviceBase
  
    The central rendering object of the Nebula3 core graphics system. This
    is basically an encapsulation of the Direct3D device. The render device
    will presents its backbuffer to the display managed by the
    CoreGraphics::DisplayDevice singleton.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/rendereventhandler.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/framebatchtype.h"
#include "coregraphics/imagefileformat.h"
#include "io/stream.h"
#include "debug/debugcounter.h"
#include "math/float4.h"
#include "math/rectangle.h"
#include "graphics/view.h"
#include "graphics/graphicsserver.h"
#include "coregraphics/shader.h"
#include "util/queue.h"

namespace CoreGraphics
{
class Texture;
class VertexBuffer;
class IndexBuffer;
class FeedbackBuffer;
class VertexLayout;
class ShaderInstance;
class RenderTarget;
class MultipleRenderTarget;
class BufferLock;
};

//------------------------------------------------------------------------------
namespace Base
{
class RenderDeviceBase : public Core::RefCounted
{
    __DeclareClass(RenderDeviceBase);
    __DeclareSingleton(RenderDeviceBase);
public:
    /// max number of vertex streams
    static const IndexT MaxNumVertexStreams = 2;

	enum MemoryBarrierFlag
	{
		NoBarrier = 0,
		ImageAccessBarrier			= 1 << 0,	// subsequent images will see data done written before barrier
		BufferAccessBarrier			= 1 << 1,	// subsequent buffers will see data done written before barrier
		SamplerAccessBarrier		= 1 << 2,	// subsequent texture samplers will see data done written before barrier
		RenderTargetAccessBarrier	= 1 << 3	// subsequent samples from render targets will be visible
	};

    /// constructor
    RenderDeviceBase();
    /// destructor
    virtual ~RenderDeviceBase();
    /// test if a compatible render device can be created on this machine
    static bool CanCreate();

    /// set an override for the default render target (call before Open()!)
    void SetOverrideDefaultRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt);

    /// open the device
    bool Open();
    /// close the device
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// attach a render event handler
    void AttachEventHandler(const Ptr<CoreGraphics::RenderEventHandler>& h);
    /// remove a render event handler
    void RemoveEventHandler(const Ptr<CoreGraphics::RenderEventHandler>& h);

    /// get default render target
    const Ptr<CoreGraphics::RenderTarget>& GetDefaultRenderTarget() const;   
    /// is a pass rendertarget set?
    bool HasPassRenderTarget() const;
    /// get current set pass render target
    const Ptr<CoreGraphics::RenderTarget>& GetPassRenderTarget() const;

    /// begin complete frame
    bool BeginFrame(IndexT frameIndex);
    /// begin rendering a frame pass
	void BeginPass(const Ptr<CoreGraphics::RenderTarget>& rt, const Ptr<CoreGraphics::Shader>& passShader);
    /// begin rendering a frame pass with a multiple rendertarget
    void BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt, const Ptr<CoreGraphics::Shader>& passShader);
    /// begin rendering a frame pass with a rendertarget cube
    void BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& crt, const Ptr<CoreGraphics::Shader>& passShader);
	/// begin rendering a transform feedback with a vertex buffer as target, updateFeedback checks if the feedback buffer should be used for updating, or for rendering
    void BeginFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb, CoreGraphics::PrimitiveTopology::Code primType, const Ptr<CoreGraphics::Shader>& shader);
    /// begin rendering a batch
    void BeginBatch(CoreGraphics::FrameBatchType::Code batchType);
    /// set the current vertex stream source
    void SetStreamSource(IndexT streamIndex, const Ptr<CoreGraphics::VertexBuffer>& vb, IndexT offsetVertexIndex);
    /// get currently set vertex buffer
    const Ptr<CoreGraphics::VertexBuffer>& GetStreamSource(IndexT streamIndex) const;
    /// get currently set vertex stream offset
    IndexT GetStreamVertexOffset(IndexT streamIndex) const;
    /// set current vertex layout
    void SetVertexLayout(const Ptr<CoreGraphics::VertexLayout>& vl);
    /// get current vertex layout
    const Ptr<CoreGraphics::VertexLayout>& GetVertexLayout() const;
    /// set current index buffer
    void SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ib);
    /// get current index buffer
    const Ptr<CoreGraphics::IndexBuffer>& GetIndexBuffer() const;
    /// set current primitive group
    void SetPrimitiveGroup(const CoreGraphics::PrimitiveGroup& pg);
    /// get current primitive group
    const CoreGraphics::PrimitiveGroup& GetPrimitiveGroup() const;
    /// draw current primitives
    void Draw();
    /// draw indexed, instanced primitives
    void DrawIndexedInstanced(SizeT numInstances, IndexT baseInstance);
	/// draw from stream output/transform feedback buffer
	void DrawFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb);
	/// draw from stream output/transform feedback buffer, instanced
	void DrawFeedbackInstanced(const Ptr<CoreGraphics::FeedbackBuffer>& fb, SizeT numInstances);
    /// perform computation
	void Compute(int dimX, int dimY, int dimZ, uint flag = NoBarrier); // use MemoryBarrierFlag
    /// end current batch
    void EndBatch();
    /// end current pass
    void EndPass();
	/// end current feedback
	void EndFeedback();
    /// end current frame
	void EndFrame(IndexT frameIndex);
    /// check if inside BeginFrame
    bool IsInBeginFrame() const;
    /// present the rendered scene
    void Present();
    /// save a screenshot to the provided stream
    CoreGraphics::ImageFileFormat::Code SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream);
	/// save a region of the screen to the provided stream
	CoreGraphics::ImageFileFormat::Code SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream, const Math::rectangle<int>& rect, int x, int y);
    /// get visualization of mipmaps flag	
    bool GetVisualizeMipMaps() const;
    /// set visualization of mipmaps flag	
    void SetVisualizeMipMaps(bool val);
	/// set the render as wireframe flag
	void SetRenderWireframe(bool b);
	/// get the render as wireframe flag
	bool GetRenderWireframe() const;
	/// sets the shader for this pass
    void SetPassShader(const Ptr<CoreGraphics::Shader>& passShader);
    /// gets the pass shader
    const Ptr<CoreGraphics::Shader>& GetPassShader() const;

	/// enqueue a buffer lock which will cause the render device to lock a buffer index whenever the next draw command gets executed
	static void EnqueueBufferLockIndex(const Ptr<CoreGraphics::BufferLock>& lock, IndexT buffer);
	/// enqueue a buffer lock which will cause the render device to lock a buffer range whenever the next draw command gets executed
	static void EnqueueBufferLockRange(const Ptr<CoreGraphics::BufferLock>& lock, IndexT start, SizeT range);
	/// empty queues and lock buffers
	static void DequeueBufferLocks();

	/// sets whether or not the render device should tessellate
	void SetUsePatches(bool state);
	/// gets whether or not the render device should tessellate
	bool GetUsePatches();

	/// call when window gets resized
	void DisplayResized(SizeT width, SizeT height);

	/// adds a viewport
	void SetViewport(const Math::rectangle<int>& rect, int index);
	/// adds a scissor rect
	void SetScissorRect(const Math::rectangle<int>& rect, int index);

protected:
    /// notify event handlers about an event
    bool NotifyEventHandlers(const CoreGraphics::RenderEvent& e);

	enum __BufferLockMode
	{
		BufferRing = 0,
		BufferRange = 1
	};
	struct __BufferLockData
	{
		Ptr<CoreGraphics::BufferLock> lock;
		__BufferLockMode mode;
		IndexT start;
		SizeT range;
		IndexT i;
	};
    
	static Util::Queue<__BufferLockData> bufferLockQueue;
    Util::Array<Ptr<CoreGraphics::RenderEventHandler> > eventHandlers;
    Ptr<CoreGraphics::RenderTarget> defaultRenderTarget;
    Ptr<CoreGraphics::VertexBuffer> streamVertexBuffers[MaxNumVertexStreams];
    IndexT streamVertexOffsets[MaxNumVertexStreams];
    Ptr<CoreGraphics::VertexLayout> vertexLayout;
    Ptr<CoreGraphics::IndexBuffer> indexBuffer;
    CoreGraphics::PrimitiveGroup primitiveGroup;
    Ptr<CoreGraphics::RenderTarget> passRenderTarget;
    Ptr<CoreGraphics::MultipleRenderTarget> passMultipleRenderTarget;
    Ptr<CoreGraphics::RenderTargetCube> passRenderTargetCube;
	Ptr<CoreGraphics::DepthStencilTarget> passDepthStencilTarget;
    Ptr<CoreGraphics::Shader> passShader;
    bool isOpen;
    bool inNotifyEventHandlers;
    bool inBeginFrame;
    bool inBeginPass;
	bool inBeginFeedback;
    bool inBeginBatch;
	bool renderWireframe;
    bool visualizeMipMaps;
	bool usePatches;
	IndexT currentFrameIndex;

    _declare_counter(RenderDeviceNumComputes);
    _declare_counter(RenderDeviceNumPrimitives);
	_declare_counter(RenderDeviceNumDrawCalls);
};

//------------------------------------------------------------------------------
/**
*/
inline void
RenderDeviceBase::SetPrimitiveGroup(const CoreGraphics::PrimitiveGroup& pg)
{
    this->primitiveGroup = pg;
}

//------------------------------------------------------------------------------
/**
*/
inline const CoreGraphics::PrimitiveGroup&
RenderDeviceBase::GetPrimitiveGroup() const
{
    return this->primitiveGroup;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>&
RenderDeviceBase::GetDefaultRenderTarget() const
{
    n_assert(this->defaultRenderTarget.isvalid());
    return this->defaultRenderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderDeviceBase::HasPassRenderTarget() const
{
    return this->passRenderTarget.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>&
RenderDeviceBase::GetPassRenderTarget() const
{
    n_assert(this->passRenderTarget.isvalid());
    return this->passRenderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderDeviceBase::IsInBeginFrame() const
{
    return this->inBeginFrame;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderDeviceBase::GetVisualizeMipMaps() const
{
    return this->visualizeMipMaps;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderDeviceBase::SetVisualizeMipMaps(bool val)
{
    this->visualizeMipMaps = val;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RenderDeviceBase::SetRenderWireframe( bool b )
{
	this->renderWireframe = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
RenderDeviceBase::GetRenderWireframe() const
{
	return this->renderWireframe;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderDeviceBase::SetUsePatches(bool state)
{
	this->usePatches = state;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
RenderDeviceBase::GetUsePatches()
{
	return this->usePatches;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderDeviceBase::SetPassShader(const Ptr<CoreGraphics::Shader>& passShader)
{
    this->passShader = passShader;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<CoreGraphics::Shader>&
RenderDeviceBase::GetPassShader() const
{
    return this->passShader;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderDeviceBase::SetViewport(const Math::rectangle<int>& rect, int index)
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
inline void
RenderDeviceBase::SetScissorRect(const Math::rectangle<int>& rect, int index)
{
	// implement in subclass
}

} // namespace Base
//------------------------------------------------------------------------------

