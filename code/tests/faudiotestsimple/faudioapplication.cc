//------------------------------------------------------------------------------
//  faudioapplication.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "faudioapplication.h"
#include "input/keyboard.h"
#include "faudio/eventparameterid.h"
#include "debugrender/debugrender.h"
#include "faudio/audiodevice.h"

namespace Test
{
using namespace Resources;
using namespace Math;
using namespace Util;
using namespace Graphics;
using namespace FAudio;
using namespace Input;

//------------------------------------------------------------------------------
/**
*/
FAudioApplication::FAudioApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FAudioApplication::~FAudioApplication()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
FAudioApplication::Open()
{
    n_assert(!this->IsOpen());
    if (ViewerApplication::Open())
    {
        // setup the audio system
		
		this->audioServer = AudioServer::Create();
		this->audioServer->Open();        
		this->listener = FAudio::AudioListener::Instance();

		// load fmod project
		AudioDevice::Instance()->LoadBank("Master Bank");
		AudioDevice::Instance()->LoadBank("Master Bank.strings");
		AudioDevice::Instance()->LoadBank("Vehicles");
		AudioDevice::Instance()->LoadBank("Surround_Ambience");
		AudioDevice::Instance()->LoadBank("Weapons");        
        
		// play sound using the emitter
        const FAudio::EventId eventId0("event:/Ambience/Country");
		this->emitter = FAudio::AudioEmitter::Create();
        this->emitter->Setup(eventId0);
		this->emitter->SetVolume(0.25f);
        this->emitter->Play();
		this->debugText = "play: event:/Ambience/Country";

				
		// enable auditioning feature
		// With auditioning eventsproperties can be altered in the FMOD-Designer 
        // during the the application is running and will be applied immediatly
        // over a TCP connection.
		

		return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
FAudioApplication::Close()
{
	this->listener = 0;
    this->emitter->Discard();
    this->audioServer->Close();
    this->audioServer = NULL;    

	ViewerApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
FAudioApplication::OnProcessInput()
{
    const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
    
	// test 2D-sound
    if (keyboard->KeyDown(Key::Key1))
    {
		const FAudio::EventId eventId("event:/Weapons/Single-Shot");
		AudioDevice::Instance()->EventPlayFireAndForget(eventId, 1.0f);
		this->debugText = "play: event:/Weapons/Single-Shot";
    }
	
	// test 3D-sound
	// listener is located at (0, 3, 10) and looks at (0, 0, 0)
	// audiosource is located at (-5, 0, 0), hearable at the left stereo channel
	if (keyboard->KeyDown(Key::Key2))
    {
        const FAudio::EventId eventId("event:/Explosions/Single Explosion");
		matrix44 transform = matrix44::translation(-5.0f, 0.0f, 0.0f);
		AudioDevice::Instance()->EventPlayFireAndForget3D(eventId, transform, vector(0.0f, 0.0f, 0.0f), 1.0f);
		this->debugText = "play: event:/Explosions/Single Explosion";
    }
    

	ViewerApplication::OnProcessInput();
}

//------------------------------------------------------------------------------
/**
*/
void
FAudioApplication::OnUpdateFrame()
{
	Util::String text = "Press '1' to play 2D-sound\n"
		                "Press '2' to play 3D-sound\n"						
						"Press '4' to change volume on programmer sound to 0.25f\n";
    _debug_text(text, Math::float2(0.01f, 0.01f), Math::float4(1.0f, 0.5f, 0.0f, 1.0f));
	_debug_text(this->debugText, Math::float2(0.01f, 0.15f), Math::float4(0.0f, 1.0f, 0.0f, 1.0f));

	this->listener->SetTransform(this->camera->GetTransform());
    this->audioServer->OnFrame();
    ViewerApplication::OnUpdateFrame();
}

} // namespace Test