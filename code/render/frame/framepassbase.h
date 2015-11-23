#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FramePassBase
    
    A frame pass encapsulates all 3d rendering to a render target, organized
    into FrameBatches.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "resources/resourceid.h"
#include "coregraphics/shader.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/rendertargetcube.h"
#include "coregraphics/multiplerendertarget.h"
#include "coregraphics/shadervariableinstance.h"
#include "frame/framebatch.h"
#include "debug/debugtimer.h"
#include "coregraphics/rendertargetcube.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FramePassBase : public Core::RefCounted
{
    __DeclareClass(FramePassBase);
public:
    /// constructor
    FramePassBase();
    /// destructor
    virtual ~FramePassBase();
    /// discard the frame pass
    virtual void Discard();

	/// begin pass
	virtual void Begin();
    /// render the pass
    virtual void Render(IndexT frameIndex);
	/// end pass
	virtual void End();

	/// sets shader instance
	void SetShader(const Ptr<CoreGraphics::Shader>& shd);
	/// gets the shader instance
    const Ptr<CoreGraphics::Shader>& GetShader() const;
	/// returns true if frame pass base has shader
	bool HasShader() const;
    /// set the name of the frame pass
    void SetName(const Resources::ResourceId& resId);
    /// get the name of the frame pass
    const Resources::ResourceId& GetName() const;
    /// set render target
    void SetRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt);
    /// get render target
    const Ptr<CoreGraphics::RenderTarget>& GetRenderTarget() const;
    /// set multiple render target
    void SetMultipleRenderTarget(const Ptr<CoreGraphics::MultipleRenderTarget>& rt);
    /// get multiple render target
    const Ptr<CoreGraphics::MultipleRenderTarget>& GetMultipleRenderTarget() const;
    /// set render target cube
    void SetRenderTargetCube(const Ptr<CoreGraphics::RenderTargetCube>& rtc);
    /// get render target cube
    const Ptr<CoreGraphics::RenderTargetCube>& GetRenderTargetCube() const;

    /// add a shader variable instance to the frame pass
    void AddVariable(const Util::StringAtom& semantic, const Ptr<CoreGraphics::ShaderVariableInstance>& var);
    /// get number of shader variables
    SizeT GetNumVariables() const;
    /// get shader variable by index
    const Ptr<CoreGraphics::ShaderVariableInstance>& GetVariableByIndex(IndexT i) const;
	/// get shader variable by name
	const Ptr<CoreGraphics::ShaderVariableInstance>& GetVariableByName(const Util::StringAtom& name);
	/// get all shader variable instances
	const Util::Array<Ptr<CoreGraphics::ShaderVariableInstance> >& GetVariables() const;

    /// add a frame batch to the frame pass
    void AddBatch(const Ptr<FrameBatch>& batch);
    /// get number of frame batches
    SizeT GetNumBatches() const;
    /// get batch by index
    const Ptr<FrameBatch>& GetBatchByIndex(IndexT i) const;

    /// set clear flags (mask of RenderTarget::Clear*)
    void SetClearFlags(uint clearFlags);
    /// get clear flags
    uint GetClearFlags() const;
    /// set clear color for all rendertargets
    void SetClearColor(const Math::float4& c);
    /// get clear color 
    const Math::float4& GetClearColor() const;
    /// set clear depth for all rendertargets
    void SetClearDepth(float d);
    /// get clear depth
    float GetClearDepth() const;
    /// set clear stencil value for all rendertargets
    void SetClearStencil(int s);
    /// get clear stencil value
    uchar GetClearStencil() const;

	/// called from frame shader whenever the display resizes
	virtual void OnDisplayResize(SizeT width, SizeT height);

protected:
    Resources::ResourceId name;
    Ptr<CoreGraphics::Shader> shader;
    Ptr<CoreGraphics::RenderTarget> renderTarget;
    Ptr<CoreGraphics::MultipleRenderTarget> multipleRenderTarget;
    Ptr<CoreGraphics::RenderTargetCube> renderTargetCube;
    Util::Array<Ptr<CoreGraphics::ShaderVariableInstance>> shaderVariables;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderVariableInstance>> shaderVariablesByName;
    Util::Array<Ptr<FrameBatch> > batches;
    uint clearFlags;
    Math::float4 clearColor;
    float clearDepth;
    int clearStencil;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetName(const Resources::ResourceId& resId)
{
    this->name = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
FramePassBase::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetShader(const Ptr<CoreGraphics::Shader>& shd)
{
	n_assert(shd.isvalid());
    this->shader = shd;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
FramePassBase::HasShader() const
{
	return this->shader.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>&
FramePassBase::GetShader() const
{
	n_assert(this->shader.isvalid());
    return this->shader;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetRenderTarget(const Ptr<CoreGraphics::RenderTarget>& rt)
{
    this->renderTarget = rt;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>&
FramePassBase::GetRenderTarget() const
{
    return this->renderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetMultipleRenderTarget(const Ptr<CoreGraphics::MultipleRenderTarget>& rt)
{
    this->multipleRenderTarget = rt;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::MultipleRenderTarget>&
FramePassBase::GetMultipleRenderTarget() const
{
    return this->multipleRenderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FramePassBase::SetRenderTargetCube( const Ptr<CoreGraphics::RenderTargetCube>& rtc )
{
    this->renderTargetCube = rtc;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTargetCube>& 
FramePassBase::GetRenderTargetCube() const
{
    return this->renderTargetCube;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::AddVariable(const Util::StringAtom& semantic, const Ptr<CoreGraphics::ShaderVariableInstance>& var)
{
	n_assert(!this->shaderVariablesByName.Contains(semantic));
    this->shaderVariables.Append(var);
	this->shaderVariablesByName.Add(semantic, var);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FramePassBase::GetNumVariables() const
{
    return this->shaderVariables.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariableInstance>& 
FramePassBase::GetVariableByIndex(IndexT i) const
{
    return this->shaderVariables[i];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderVariableInstance>& 
FramePassBase::GetVariableByName( const Util::StringAtom& name )
{
	n_assert(this->shaderVariablesByName.Contains(name));
	return this->shaderVariablesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<CoreGraphics::ShaderVariableInstance> >&
FramePassBase::GetVariables() const
{
	return this->shaderVariables;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::AddBatch(const Ptr<FrameBatch>& batch)
{
    this->batches.Append(batch);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FramePassBase::GetNumBatches() const
{
    return this->batches.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<FrameBatch>&
FramePassBase::GetBatchByIndex(IndexT i) const
{
    return this->batches[i];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetClearFlags(uint f)
{
    this->clearFlags = f;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
FramePassBase::GetClearFlags() const
{
    return this->clearFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetClearColor(const Math::float4& c)
{
    this->clearColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
FramePassBase::GetClearColor() const
{
    return this->clearColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetClearDepth(float d)
{
    this->clearDepth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline float
FramePassBase::GetClearDepth() const
{
    return this->clearDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FramePassBase::SetClearStencil(int s)
{
    this->clearStencil = s;
}

//------------------------------------------------------------------------------
/**
*/
inline uchar
FramePassBase::GetClearStencil() const
{
    return this->clearStencil;
}


} // namespace Frame
//------------------------------------------------------------------------------

    