#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::AudioServer

    Front-end of the Audio subsystem. Initializes the audio 
    subsystem.	

    (C) 2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"


namespace FAudio { class AudioDevice; class AudioHandler; }

//------------------------------------------------------------------------------
namespace FAudio
{
class AudioServer : public Core::RefCounted
{
	__DeclareClass(AudioServer);
	__DeclareSingleton(AudioServer);
public:
    /// constructor
	AudioServer();
    /// destructor
	virtual ~AudioServer();

    /// open the audio subsystem (waits for completion)
    void Open();
    /// close the audio subsystem (waits for completion)
    void Close();
    /// return true if the audio subsystem is open
    bool IsOpen() const;
    /// called per-frame 
    void OnFrame();     
	/// load all banks marked as autoload
	void LoadBanks();

private:
    bool isOpen;
    IndexT adapterIndex;     
	Ptr<AudioDevice> device;
	Ptr<AudioHandler> handler;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
AudioServer::IsOpen() const
{
    return this->isOpen;
}

} // namespace FAudio
//------------------------------------------------------------------------------
    