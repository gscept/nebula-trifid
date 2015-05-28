//------------------------------------------------------------------------------
//  theoravideoserver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/theora/theoravideoserver.h"

namespace Video
{
__ImplementClass(Video::TheoraVideoServer, 'THVI', Base::VideoServerBase);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
TheoraVideoServer::TheoraVideoServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TheoraVideoServer::~TheoraVideoServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}


//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoServer::Close()
{
    n_assert(this->IsOpen());
	VideoServerBase::Close();
    
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoServer::OnFrame(Timing::Time time)
{
	for (int i = 0; i < this->players.Size(); i++)
	{
		this->players.ValueAtIndex(i)->OnFrame();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoServer::OnRenderBefore(Timing::Time time)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TheoraVideoServer::SetupVideoSystem()
{
    // empty
}

//------------------------------------------------------------------------------
/**    
*/
void 
TheoraVideoServer::StartVideo(const Util::StringAtom& resName,
                            const Math::float2& upperLeft,
                            const Math::float2& upperRight,
                            const Math::float2& lowerLeft,
                            const Math::float2& lowerRight,
                            bool fromFile,
                            bool autoDelete,
                            bool loop)
{
	Ptr<Video::TheoraVideoPlayer> player = Video::TheoraVideoPlayer::Create();
	player->Setup(resName);
	player->Start();
	this->players.Add(resName, player);
}

//------------------------------------------------------------------------------
/**    
*/
void
TheoraVideoServer::StopVideo(const Util::StringAtom& resName,
                           bool del)
{
	n_assert(this->players.Contains(resName));
	this->players[resName]->Stop();
}

//------------------------------------------------------------------------------
/**    
*/
void
TheoraVideoServer::PauseVideo(const Util::StringAtom& resName)
{
	n_assert(this->players.Contains(resName));
	this->players[resName]->Pause();
}

//------------------------------------------------------------------------------
/**    
*/
void 
TheoraVideoServer::ResumeVideo(const Util::StringAtom& resName)
{
	n_assert(this->players.Contains(resName));
	this->players[resName]->Resume();
}

//------------------------------------------------------------------------------
/**
*/
bool
TheoraVideoServer::DeleteVideo(const Util::StringAtom& resName)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
TheoraVideoServer::IsVideoPlaying(const Util::StringAtom& resName)
{
	n_assert(this->players.Contains(resName));
	return this->players[resName]->IsPlaying();
}

//------------------------------------------------------------------------------
/**
*/
bool
TheoraVideoServer::IsVideoPausing(const Util::StringAtom& resName)
{
	n_assert(this->players.Contains(resName));
	return this->players[resName]->IsPaused();
}
} // namespace Video
