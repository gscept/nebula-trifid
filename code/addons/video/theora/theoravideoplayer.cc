//------------------------------------------------------------------------------
//  theoravideoplayer.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "theoravideoplayer.h"
#include "io/ioserver.h"
#include "framesync/framesynctimer.h"
#include "coregraphics/memorytextureloader.h"
#include "resources/resourcemanager.h"
#include "renderutil/drawfullscreenquad.h"
#include "coregraphics/shaderserver.h"


__ImplementClass(Video::TheoraVideoPlayer, 'THVP', Core::RefCounted);

namespace Video
{

//------------------------------------------------------------------------------
/**
*/
TheoraVideoPlayer::TheoraVideoPlayer() : 
	isPaused(false), 
	isPlaying(false),
	isOpen(false),
	width(0),
	height(0),
	fps(-1.0f)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TheoraVideoPlayer::~TheoraVideoPlayer()
{
	this->Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
TheoraVideoPlayer::OnFrame( Timing::Time time )
{
	if (this->isPlaying && !this->isPaused)
	{
		if (this->decodedFrames == 0)
		{
			this->startTime = time;
		}		
		uint nextFrame = (uint)((time - this->startTime)  * this->ti.fps_numerator / ti.fps_denominator);		
		uint frames = Math::n_iclamp(nextFrame - this->decodedFrames,1,25);
		for (uint i = 0; i < frames; i++)
		{
			if (!this->DecodeFrame()) break;			
		}		
		theora_decode_YUVout(&this->td, &this->yuv);
		this->DecodeYUV();
		this->videoTexture->Update(this->rgbBuffer, this->width * this->height * 3, this->width, this->height, 0, 0, 0);		
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Setup(const Util::StringAtom& resName)
{
	this->fileStream = IO::IoServer::Instance()->CreateStream(resName.Value());
	n_assert(this->fileStream->Open());
	
	this->isOpen = true;
	this->decodedFrames = 0;

	// initialize theora
	this->theora_p = 0;
	this->stateflag = 0;
	this->videobuf_ready = 0;
	this->videobuf_granulepos = -1;
	this->videobuf_time = 0;
	Memory::Clear(&this->td, sizeof(this->td));
	
	/*
	Ok, Ogg parsing. The idea here is we have a bitstream
	that is made up of Ogg pages. The libogg sync layer will
	find them for us. There may be pages from several logical
	streams interleaved; we find the first theora stream and
	ignore any others.

	Then we pass the pages for our stream to the libogg stream
	layer which assembles our original set of packets out of
	them. It's the packets that libtheora actually knows how
	to handle.
	*/

	/* start up Ogg stream synchronization layer */
	ogg_sync_init(&this->oy);

	/* init supporting Theora structures needed in header parsing */
	theora_comment_init(&this->tc);
	theora_info_init(&this->ti);

	/* Ogg file open; parse the headers */

	/* Vorbis and Theora both depend on some initial header packets
	for decoder setup and initialization. We retrieve these first
	before entering the main decode loop. */

	/* Only interested in Theora streams */
	while (!this->stateflag)
	{
		int ret = this->BufferData();
		if (ret == 0)break;
		while (ogg_sync_pageout(&this->oy, &this->og) > 0)
		{
			ogg_stream_state test;
			/* is this a mandated initial header? If not, stop parsing */
			if (!ogg_page_bos(&this->og)){
				/* don't leak the page; get it into the appropriate stream */
				this->QueuePage();
				this->stateflag = 1;
				break;
			}
			ogg_stream_init(&test, ogg_page_serialno(&this->og));
			ogg_stream_pagein(&test, &this->og);
			ogg_stream_packetout(&test, &this->op);
			/* identify the codec: try theora */
			if (!theora_p && theora_decode_header(&this->ti, &this->tc, &this->op) >= 0){
				/* it is theora -- save this stream state */
				Memory::Copy(&test, &this->to, sizeof(test));				
				this->theora_p = 1;
			}
			else{
				/* whatever it is, we don't care about it */
				ogg_stream_clear(&test);
			}
		}
		/* fall through to non-initial page parsing */
	}

	/* we're expecting more header packets. */
	while (this->theora_p && this->theora_p < 3)
	{
		int ret;

		/* look for further theora headers */
		while (this->theora_p && (this->theora_p < 3) && (ret = ogg_stream_packetout(&this->to, &this->op)))
		{
			// "Error parsing Theora stream headers; corrupt stream?" ?
			n_assert(ret >= 0);

			// "Error parsing Theora stream headers; corrupt stream?" ?
			n_assert(theora_decode_header(&this->ti, &this->tc, &this->op) == 0);

			this->theora_p++;
			if (this->theora_p == 3)
				break;
		}


		/* The header pages/packets will arrive before anything else we
		care about, or the stream is not obeying spec */

		if (ogg_sync_pageout(&this->oy, &this->og)>0)
		{
			this->QueuePage(); /* demux into the stream state */
		}
		else
		{
			int ret = this->BufferData(); /* need more data */
			// "End of file while searching for codec headers." ?
			n_assert(ret != 0);
		}
	}

	/* Now we have all the required headers. initialize the decoder. */
	if (this->theora_p)
	{
		theora_decode_init(&this->td, &this->ti);
		/*
		fprintf(stderr,"Ogg logical stream %x is Theora %dx%d %.02f fps video\nEncoded frame content is %dx%d with %dx%d offset\n",
		to.serialno,ti.width,ti.height, (double)ti.fps_numerator/ti.fps_denominator,
		ti.frame_width, ti.frame_height, ti.offset_x, ti.offset_y);
		*/
		this->width = this->ti.frame_width;
		this->height = this->ti.frame_height;
		this->fps = ((float)ti.fps_numerator / ti.fps_denominator);
	}
	else
	{
		/* tear down the partial theora setup */
		theora_info_clear(&this->ti);
		theora_comment_clear(&this->tc);
	}
	/* Finally the main decode loop.

	It's one Theora packet per frame, so this is pretty
	straightforward if we're not trying to maintain sync
	with other multiplexed streams.

	the videobuf_ready flag is used to maintain the input
	buffer in the libogg stream state. If there's no output
	frame available at the end of the decode step, we must
	need more input data. We could simplify this by just
	using the return code on ogg_page_packetout(), but the
	flag system extends easily to the case were you care
	about more than one multiplexed stream (like with audio
	playback). In that case, just maintain a flag for each
	decoder you care about, and pull data when any one of
	them stalls.

	videobuf_time holds the presentation time of the currently
	buffered video frame. We ignore this value.
	*/
	this->stateflag = 0; /* playback has not begun */
	/* queue any remaining pages from data we buffered but that did not
	contain headers */
	while (ogg_sync_pageout(&this->oy, &this->og) > 0)
	{
		this->QueuePage();
	};
	this->theoraLoaded = true;	

	SizeT frameSize = this->ti.width * this->ti.height * 4;
	this->rgbBuffer = (unsigned char*) Memory::Alloc(Memory::DefaultHeap, frameSize);
	Memory::Clear(this->rgbBuffer, frameSize);
	this->frameNr = -1;

	// create texture
	this->videoTexture = CoreGraphics::Texture::Create();
	Ptr<CoreGraphics::MemoryTextureLoader> loader = CoreGraphics::MemoryTextureLoader::Create();
	loader->SetImageBuffer(this->rgbBuffer, this->width, this->height, CoreGraphics::PixelFormat::SRGBA8);
	this->videoTexture->SetLoader(loader.upcast<Resources::ResourceLoader>());
	this->videoTexture->SetAsyncEnabled(false);
	this->videoTexture->SetResourceId(resName);
	this->videoTexture->Load();	
	n_assert(this->videoTexture->IsLoaded());
	this->videoTexture->SetLoader(0);	
	Resources::ResourceManager::Instance()->RegisterUnmanagedResource(this->videoTexture.upcast<Resources::Resource>());	
	this->quad.Setup(this->width, this->height);
	this->shader = CoreGraphics::ShaderServer::Instance()->CreateShaderInstance("shd:copy");
	this->videoTextureVariable = this->shader->GetVariableByName("CopyBuffer");
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Start()
{
	this->isPlaying = true;
	// reset times

	this->startTime = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Pause()
{
	this->isPaused = true;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Resume()
{
	this->isPaused = false;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Stop()
{
	this->Close();
	this->isPlaying = false;
}

//------------------------------------------------------------------------------
/**
*/
const bool
TheoraVideoPlayer::IsPlaying() const
{
	return this->isPlaying;
}

//------------------------------------------------------------------------------
/**
*/
const bool
TheoraVideoPlayer::IsPaused() const
{
	return this->isPaused;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::DecodeYUV()
{
	int rgbIndex = 0;
	int y;
	for (y = this->yuv.y_height-1; y >= 0 ; y--)
	{
		int xsize = this->yuv.y_width;
		int uvy = (y / 2) * this->yuv.uv_stride;
		int yy = y * this->yuv.y_stride;
		int x;
		for (x = 0; x < xsize; x++)
		{
			int Y = this->yuv.y[yy + x] - 16;
			int U = this->yuv.u[uvy + (x / 2)] - 128;
			int V = this->yuv.v[uvy + (x / 2)] - 128;
			int R = ((298 * Y + 409 * V + 128) >> 8);
			int G = ((298 * Y - 100 * U - 208 * V + 128) >> 8);
			int B = ((298 * Y + 516 * U + 128) >> 8);
			if (R < 0) R = 0; if (R>255) R = 255;
			if (G < 0) G = 0; if (G>255) G = 255;
			if (B < 0) B = 0; if (B>255) B = 255;
			this->rgbBuffer[rgbIndex++] = (unsigned char)R;
			this->rgbBuffer[rgbIndex++] = (unsigned char)G;
			this->rgbBuffer[rgbIndex++] = (unsigned char)B;			
			this->rgbBuffer[rgbIndex++] = 255;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
TheoraVideoPlayer::DecodeFrame()
{
	n_assert(this->isPlaying);
	this->videobuf_ready = 0;
	while (!this->videobuf_ready)
	{
		while (this->theora_p && !this->videobuf_ready)
		{
			// theora is one in, one out... 
			if (ogg_stream_packetout(&this->to, &this->op) > 0)
			{

				theora_decode_packetin(&this->td, &this->op);
				this->videobuf_granulepos = this->td.granulepos;
				this->videobuf_time = theora_granule_time(&this->td, this->videobuf_granulepos);
				this->videobuf_ready = 1;

			}
			else
				break;
		}
		if (!this->videobuf_ready && this->fileStream->Eof())
		{
			// reached end, stop playing
			this->isPlaying = false;
			return false;
		};

		if (!videobuf_ready)
		{
			// no data yet for somebody.  Grab another page 
			this->BufferData();
			while (ogg_sync_pageout(&this->oy, &this->og) > 0)
			{
				this->QueuePage();				
			}
			videobuf_ready = 0;
		};
	};
	if (videobuf_ready == 0)
	{
		return false;
	}
	frameNr++;
	decodedFrames++;	
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Reset()
{

}

//------------------------------------------------------------------------------
/**
*/
int
TheoraVideoPlayer::BufferData()
{
	char * buffer = ogg_sync_buffer(&this->oy, 4096);
	int bytes = this->fileStream->Read(buffer, 4096);
	ogg_sync_wrote(&this->oy, bytes);
	return bytes;
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::QueuePage()
{
	if (this->theora_p)
	{
		ogg_stream_pagein(&this->to, &this->og);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Close()
{
	if (this->isOpen)
	{
		if (this->theora_p)
		{
			ogg_stream_clear(&this->to);
			theora_clear(&this->td);
			theora_comment_clear(&this->tc);
			theora_info_clear(&this->ti);
		};
		ogg_sync_clear(&this->oy);
		
		this->isOpen = false;
		this->fileStream->Close();
		Memory::Free(Memory::DefaultHeap, this->rgbBuffer);
		this->rgbBuffer = 0;
		this->shader->Discard();
		this->shader = 0;
		this->videoTextureVariable = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoPlayer::Render()
{
	if (this->isPlaying)
	{
		this->videoTextureVariable->SetTexture(this->videoTexture);
		this->shader->Begin();
		this->shader->BeginPass(0);
		this->shader->Commit();
		this->quad.Draw();
		this->shader->EndPass();
		this->shader->End();
	}
}

} // namespace Video