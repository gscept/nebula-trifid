#pragma once
//------------------------------------------------------------------------------
/**
	A FrameScript describes render operations being done to produce a single frame.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/rendertexture.h"
#include "coregraphics/shaderreadwritetexture.h"
#include "coregraphics/shaderreadwritebuffer.h"
#include "coregraphics/event.h"
#include "algorithm/algorithm.h"
#include "frame2/frameop.h"
namespace Frame2
{
class FrameScript : public Core::RefCounted
{
	__DeclareClass(FrameScript);
public:
	/// constructor
	FrameScript();
	/// destructor
	virtual ~FrameScript();

	/// add frame operation
	void AddOp(const Ptr<Frame2::FrameOp>& op);
	/// add color texture
	void AddColorTexture(const Util::StringAtom& name, const Ptr<CoreGraphics::RenderTexture>& tex);
	/// get color texture
	const Ptr<CoreGraphics::RenderTexture>& GetColorTexture(const Util::StringAtom& name);
	/// add depth-stencil texture
	void AddDepthStencilTexture(const Util::StringAtom& name, const Ptr<CoreGraphics::RenderTexture>& tex);
	/// get depth-stencil texture
	const Ptr<CoreGraphics::RenderTexture>& GetDepthStencilTexture(const Util::StringAtom& name);
	/// add read-write texture
	void AddReadWriteTexture(const Util::StringAtom& name, const Ptr<CoreGraphics::ShaderReadWriteTexture>& tex);
	/// get read-write texture
	const Ptr<CoreGraphics::ShaderReadWriteTexture>& GetReadWriteTexture(const Util::StringAtom& name);
	/// add read-write buffer
	void AddReadWriteBuffer(const Util::StringAtom& name, const Ptr<CoreGraphics::ShaderReadWriteBuffer>& buf);
	/// get read-write buffer
	const Ptr<CoreGraphics::ShaderReadWriteBuffer>& GetReadWriteBuffer(const Util::StringAtom& name);
	/// add event
	void AddEvent(const Util::StringAtom& name, const Ptr<CoreGraphics::Event>& event);
	/// get event
	const Ptr<CoreGraphics::Event>& GetEvent(const Util::StringAtom& name);
	/// add algorithm
	void AddAlgorithm(const Util::StringAtom& name, const Ptr<Algorithms::Algorithm>& alg);
	/// get algorithm
	const Ptr<Algorithms::Algorithm>& GetAlgorithm(const Util::StringAtom& name);

	/// setup operation
	void Setup();
	/// run operation
	void Run(const IndexT frameIndex);
private:
	friend class FrameScriptLoader;

	Util::Array<Ptr<CoreGraphics::RenderTexture>> colorTextures;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::RenderTexture>> colorTexturesByName;
	Util::Array<Ptr<CoreGraphics::RenderTexture>> depthStencilTextures;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::RenderTexture>> depthStencilTexturesByName;
	Util::Array<Ptr<CoreGraphics::ShaderReadWriteTexture>> readWriteTextures;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderReadWriteTexture>> readWriteTexturesByName;
	Util::Array<Ptr<CoreGraphics::ShaderReadWriteBuffer>> readWriteBuffers;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ShaderReadWriteBuffer>> readWriteBuffersByName;
	Util::Array<Ptr<CoreGraphics::Event>> events;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::Event>> eventsByName;
	Util::Array<Ptr<Frame2::FrameOp>> ops;
	Util::Array<Ptr<Algorithms::Algorithm>> algorithms;
	Util::Dictionary<Util::StringAtom, Ptr<Algorithms::Algorithm>> algorithmsByName;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTexture>&
FrameScript::GetColorTexture(const Util::StringAtom& name)
{
	return this->colorTexturesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTexture>&
FrameScript::GetDepthStencilTexture(const Util::StringAtom& name)
{
	return this->depthStencilTexturesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderReadWriteTexture>&
FrameScript::GetReadWriteTexture(const Util::StringAtom& name)
{
	return this->readWriteTexturesByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ShaderReadWriteBuffer>&
FrameScript::GetReadWriteBuffer(const Util::StringAtom& name)
{
	return this->readWriteBuffersByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Event>&
FrameScript::GetEvent(const Util::StringAtom& name)
{
	return this->eventsByName[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Algorithms::Algorithm>&
FrameScript::GetAlgorithm(const Util::StringAtom& name)
{
	return this->algorithmsByName[name];
}

} // namespace Frame2