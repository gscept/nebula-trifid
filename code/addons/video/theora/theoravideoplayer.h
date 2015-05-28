#pragma once
//------------------------------------------------------------------------------
/**
	@class Video::TheoraVideoPlayer
	
	
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "theora/theora.h"
#include "timing/time.h"
#include "io/stream.h"

namespace Video
{
class TheoraVideoPlayer : public Core::RefCounted
{
	__DeclareClass(TheoraVideoPlayer);
public:
	/// constructor
	TheoraVideoPlayer();
	/// destructor
	virtual ~TheoraVideoPlayer();

	///
	void OnFrame();

	/// 
	void Close();

	/// setup
	void Setup(const Util::StringAtom& resName);
	/// 
	void Start();
	/// 
	void Pause();
	/// 
	void Resume();
	/// 
	void Stop();
	/// is playing
	const bool IsPlaying() const;
	/// is paused
	const bool IsPaused() const;
	///
	const unsigned int GetFrameWidth() const;
	///
	const unsigned int GetFrameHeight() const;
	///
	const float GetFrameRate() const;
private:

	///
	void DecodeYUV();
	/// decode frame
	void DecodeFrame();
	/// reset
	void Reset();
	///
	int BufferData();
	///
	void QueuePage();

	/// Ogg and codec state for demux/decode 
	ogg_sync_state oy;
	ogg_page og;
	ogg_stream_state vo;
	ogg_stream_state to;
	theora_info ti;
	theora_comment tc;
	theora_state td;

	int theora_p;
	int stateflag;

	/// single frame video buffering
	int videobuf_ready;
	ogg_int64_t videobuf_granulepos;
	double videobuf_time;
	int frameNr;

	ogg_packet op;
	Ptr<IO::Stream> fileStream;
	yuv_buffer yuv;

	unsigned char *rgbBuffer;
	int fileVideoDataOffset;
	bool theoraLoaded;
	bool isPlaying;
	bool isPaused;	
	bool isOpen;
	Timing::Time startTime;
	uint decodedFrames;

	unsigned int width;
	unsigned int height;
	float fps;
	
};
} // namespace Video