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
void
PickingServer::FetchSquare( const Math::rectangle<float>& rect, Util::Array<IndexT> & indices, Util::Array<IndexT> & edgeIndices)
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

	int width = tex->GetWidth();
	int height = tex->GetHeight();
#if (__DX11__ || __DX9__)
#error This is most likely utterly broken now
	// calculate pixel offset	
	int pixelSize = PixelFormat::ToSize(tex->GetPixelFormat());
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
	int xMin = Math::n_iclamp((int)rect.left, 0, width - 1);
	int yMin = tex->GetHeight() - Math::n_iclamp((int)rect.bottom, 0, height - 1);

	int xMax = Math::n_iclamp((int)rect.right, 0, width - 1);
	int yMax = tex->GetHeight() - Math::n_iclamp((int)rect.top, 0, height - 1);
#endif
	
	float* values = (float*)info.data;			
	
	for (IndexT i = yMin; i < yMax; i++)
	{
		for (IndexT j = xMin; j < xMax; j++)
		{
			IndexT index = IndexT(values[i*width + j]);
			if (indices.BinarySearchIndex(index) == InvalidIndex)
			{
				indices.InsertSorted(index);
			}			
			if (i == yMin || i == yMax-1 || j == xMin || j == xMax-1)
			{
				if (edgeIndices.BinarySearchIndex(index) == InvalidIndex)
				{
					edgeIndices.InsertSorted(index);
				}
			}
		}		
	}
	// release texture map
	tex->Unmap(0);	
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
