//------------------------------------------------------------------------------
//  pickingserver.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "pickingserver.h"
#include "frame/frameserver.h"
#include "graphics/cameraentity.h"
#include "coregraphics/texture.h"
#include "coregraphics/base/texturebase.h"
#include "resources/resourcemanager.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"


using namespace Base;
using namespace CoreGraphics;
using namespace Frame;
namespace Picking
{
__ImplementClass(Picking::PickingServer, 'PISR', Core::RefCounted);
__ImplementSingleton(Picking::PickingServer);


//------------------------------------------------------------------------------
/**
*/
PickingServer::PickingServer() :
	isOpen(false),
	frameShader(0),
	enabled(false)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PickingServer::~PickingServer()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PickingServer::Open()
{
	n_assert(!this->IsOpen());
	this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
PickingServer::Close()
{
	n_assert(this->IsOpen());
	if (this->frameShader.isvalid())
	{
		this->frameShader->Discard();
		this->frameShader = 0;

		this->pickingBuffer = 0;
		this->normalBuffer = 0;
		this->depthBuffer = 0;
	}
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
PickingServer::BeginFrame(const Ptr<Graphics::CameraEntity>& camera)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PickingServer::Render(IndexT frameIndex)
{
	n_assert(this->IsOpen());
	if (this->frameShader.isvalid() && this->enabled)
	{
        this->frameShader->Render(frameIndex);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
PickingServer::EndFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PickingServer::SetEnabled( bool b )
{
	n_assert(this->IsOpen());
	this->enabled = b;
	if (b)
	{
		// get picking shader
		this->frameShader = FrameServer::Instance()->LookupFrameShader("picking");

		// also get default frame shader to get geometry buffers
        const Ptr<FrameShader>& defaultFrameShader = Graphics::GraphicsServer::Instance()->GetDefaultView()->GetFrameShader();

		// get render target from frame shader
		this->pickingBuffer = this->frameShader->GetRenderTargetByName("PickingBuffer");
		this->depthBuffer = defaultFrameShader->GetRenderTargetByName("DepthBuffer");
		this->normalBuffer = defaultFrameShader->GetRenderTargetByName("NormalBuffer");
	}
	else
	{
		n_assert(this->frameShader.isvalid());

		// unload picking buffer pointer
		this->pickingBuffer = 0;

		// unload frame shader if it's loaded
		this->frameShader->Discard();
		this->frameShader = 0;		
	}
}


//------------------------------------------------------------------------------
/**
*/
IndexT 
PickingServer::FetchIndex( const Math::float2& position )
{
	n_assert(this->IsOpen());
	Ptr<Texture> tex = this->pickingBuffer->GetResolveTexture();

#if __DX11__
	// ugly fix for DX11 which lets us copy the GPU texture to CPU, so that we can read it
	tex->GetCPUTexture();
#endif

	TextureBase::MapInfo info;
	bool status = tex->Map(0, ResourceBase::MapRead, info);
	n_assert(status);

	// calculate pixel offset
	int width = tex->GetWidth();

#if (__DX11__ || __DX9__)
	int pixelSize = PixelFormat::ToSize(tex->GetPixelFormat());
	int calcedPitch = width * pixelSize;
	int pitchDiff = info.rowPitch - calcedPitch;
	width += pitchDiff / pixelSize;

	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = (int)position.y();
#elif __OGL4__
	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = tex->GetHeight() - (int)position.y();
#endif	

	// clamp within texture bounds
	xCoord = Math::n_iclamp(xCoord, 0, tex->GetWidth()-1);
	yCoord = Math::n_iclamp(yCoord, 0, tex->GetHeight()-1);

	// calculate offset in buffer
	int offset = width * (yCoord - 1) + xCoord;

	// get value
	float value = ((const float*)(info.data))[offset];

	// release texture map
	tex->Unmap(0);

	return (IndexT)value;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<IndexT> 
PickingServer::FetchSquare( const Math::rectangle<float>& rect )
{
	n_assert(this->IsOpen());
	Ptr<Texture> tex = this->pickingBuffer->GetResolveTexture();

#if __DX11__
	// ugly fix for DX11 which lets us copy the GPU texture to CPU, so that we can read it
	tex->GetCPUTexture();
#endif

	TextureBase::MapInfo info;
	bool status = tex->Map(0, ResourceBase::MapRead, info);
	n_assert(status);

	// calculate pixel offset
	int pixelSize = PixelFormat::ToSize(tex->GetPixelFormat());
	int width = tex->GetWidth();

#if (__DX11__ || __DX9__)
	int calcedPitch = width * pixelSize;
	int pitchDiff = info.rowPitch - calcedPitch;
	width += pitchDiff / pixelSize;

	// get coordinate
	int xMin = (int)rect.left;
	int yMin = (int)rect.top;

	int xMax = (int)rect.right;
	int yMax = (int)rect.bottom;
#elif __OGL4__
	// get coordinate
	int xMin = (int)rect.left;
	int yMin = (int)rect.bottom;

	int xMax = (int)rect.right;
	int yMax = (int)rect.top;
#endif

	// calculate start offset and end offset in buffer
	int upperLeft = width * (yMin - 1) + xMin;
	int upperRight = width * (yMin - 1) + xMax;
	int lowerLeft = width * (yMax - 1) + xMin;
	int lowerRight = width * (yMax - 1) + xMax;

	// get offset to start
	float* values = &((float*)(info.data))[upperLeft];

	// create return list indices
	//Util::Array<IndexT> indices;

	// make a dictionary which we will use as a set, so the byte value is just a dummy
	Util::Dictionary<IndexT, ubyte> indices;

	// get values
	IndexT i;
	IndexT j;
	for (i = 0; i < rect.height(); i++)
	{
		for (j = 0; j < rect.width(); j++)
		{
			IndexT index = IndexT(values[(i + 1) * j]);
			if (!indices.Contains(index)) indices.Add(index, 0);
		}

		// offset values by a row
		values += width;
	}

	return indices.KeysAsArray();
}

//------------------------------------------------------------------------------
/**
*/
float 
PickingServer::FetchDepth( const Math::float2& position )
{
	n_assert(this->IsOpen());
	Ptr<Texture> tex = this->depthBuffer->GetResolveTexture();

#if __DX11__
	// ugly fix for DX11 which lets us copy the GPU texture to CPU, so that we can read it
	tex->GetCPUTexture();
#endif

	TextureBase::MapInfo info;
	bool status = tex->Map(0, ResourceBase::MapRead, info);
	n_assert(status);

	// calculate pixel offset
	int width = tex->GetWidth();

#if (__DX11__ || __DX9__)
	int pixelSize = PixelFormat::ToSize(tex->GetPixelFormat());
	int calcedPitch = width * pixelSize;
	int pitchDiff = info.rowPitch - calcedPitch;
	width += pitchDiff / pixelSize;

	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = (int)position.y();
#elif __OGL4__
	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = tex->GetHeight() - (int)position.y();
#endif	

	// calculate offset in buffer
	int offset = width * (yCoord - 1) + xCoord;

	// get value
	float value = ((const float*)(info.data))[offset];

	// release texture map
	tex->Unmap(0);

	// return depth
	return value;
}

//------------------------------------------------------------------------------
/**
*/
Math::float4 
PickingServer::FetchNormal( const Math::float2& position )
{
	n_assert(this->IsOpen());
	Ptr<Texture> tex = this->pickingBuffer->GetResolveTexture();

#if __DX11__
	// ugly fix for DX11 which lets us copy the GPU texture to CPU, so that we can read it
	tex->GetCPUTexture();
#endif

	TextureBase::MapInfo info;
	bool status = tex->Map(0, ResourceBase::MapRead, info);
	n_assert(status);

	// calculate pixel offset
	int width = tex->GetWidth();

#if (__DX11__ || __DX9__)
	int pixelSize = PixelFormat::ToSize(tex->GetPixelFormat());
	int calcedPitch = width * pixelSize;
	int pitchDiff = info.rowPitch - calcedPitch;
	width += pitchDiff / pixelSize;

	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = (int)position.y();
#elif __OGL4__
	// get coordinate
	int xCoord = (int)position.x();
	int yCoord = tex->GetHeight() - (int)position.y();
#endif	

	// calculate offset in buffer
	int offset = width * (yCoord - 1) + xCoord;

	// get value
	float* values = ((float*)(info.data));

	// create normal vector
	Math::float4 normal(values[0], values[1], values[2], values[3]);

	// decode normal
	normal *= Math::float4(2, 2, 2, 0);
	normal -= Math::float4(1, 1, 1, 0);
	normal.w() = 1;

	// release texture map
	tex->Unmap(0);

	// return normal
	return normal;
}
} // namespace Picking
