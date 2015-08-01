#pragma once
//------------------------------------------------------------------------------
/**
    @class Picking::PickingServer
    
    Server responsible to handle id-based rendering.
    
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/singleton.h"
#include "core/refcounted.h"
#include "math/float2.h"
#include "math/rectangle.h"

namespace Graphics
{
	class CameraEntity;
}
namespace CoreGraphics
{
	class RenderTarget;
}
namespace Frame
{
	class FrameShader;
}

namespace Picking
{
class PickingServer : public Core::RefCounted
{
	__DeclareClass(PickingServer);
	__DeclareSingleton(PickingServer);	
public:
	/// constructor
	PickingServer();
	/// destructor
	virtual ~PickingServer();

	/// open the PickingServer
	void Open();
	/// close the PickingServer
	void Close();
	/// returns true if server is open
	const bool IsOpen() const;

	/// enables picking
	void SetEnabled(bool b);
	/// returns true if picking is enabled
	const bool IsEnabled() const;

	/// begins a frame with a specific camera
	void BeginFrame(const Ptr<Graphics::CameraEntity>& camera);
	/// renders a frame
    void Render(IndexT frameIndex);
	/// ends a frame
	void EndFrame();

	/// returns picking id from pixel
	IndexT FetchIndex(const Math::float2& position);
	/// returns array of picking ids from a rectangle
	Util::Array<IndexT> FetchSquare(const Math::rectangle<float>& rect);
	/// returns depth of position
	float FetchDepth(const Math::float2& position);
	/// returns normal of position
	Math::float4 FetchNormal(const Math::float2& position);
private:
	bool isOpen;
	bool enabled;
	Ptr<Frame::FrameShader> frameShader;
	Ptr<CoreGraphics::RenderTarget> pickingBuffer;
	Ptr<CoreGraphics::RenderTarget> depthBuffer;
	Ptr<CoreGraphics::RenderTarget> normalBuffer;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const bool 
PickingServer::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
PickingServer::IsEnabled() const
{
	return this->enabled;
}

} // namespace Picking
//------------------------------------------------------------------------------