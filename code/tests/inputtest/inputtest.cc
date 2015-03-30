//------------------------------------------------------------------------------
//  ps3test.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/types.h"
#include "system/appentry.h"
#include "core/coreserver.h"
#include "input/inputserver.h"
#include "input/gamepad.h"

ImplementNebulaApplication();

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const Util::CommandLineArgs& args)
{
    Ptr<Core::CoreServer> coreServer = Core::CoreServer::Create();
    coreServer->SetCompanyName(StringAtom("Radon Labs GmbH"));
    coreServer->SetAppName(StringAtom("PS3Test"));
    coreServer->Open();

	Ptr<Input::InputServer> inputServer = Input::InputServer::Create();
	inputServer->Open();

	while(1)
	{
		inputServer->BeginFrame();
		inputServer->OnFrame();

		const Ptr<Input::GamePad> &gamePad = inputServer->GetDefaultGamePad(0);
		if(gamePad->IsConnected())
		{
#if 0
			// test digital button
			bool down = gamePad->ButtonDown(Input::GamePad::DPadLeftButton);
			bool pressed = gamePad->ButtonPressed(Input::GamePad::DPadLeftButton);
			bool up = gamePad->ButtonUp(Input::GamePad::DPadLeftButton);
			if(down || pressed || up)
			{
				if(down) n_printf("[DPadLeftButton] down\n" );
				if(pressed) n_printf("[DPadLeftButton] pressed\n" );
				if(up) n_printf("[DPadLeftButton] up\n" );
			}
#endif

#if 1
			// test axis
			static float lastVal = -100.0f;
			float v = gamePad->GetAxisValue(Input::GamePad::LeftThumbXAxis);
			if(v != lastVal)
			{
				n_printf("val: %f\n", v );
				lastVal = v;
			}
#endif

#ifdef __PS3__
#if 0
			// sixaxis
			{
				static Math::float4 last(-2.0f, -2.0f, -2.0f, -2.0f);
				Math::vector sixAxisData = gamePad->GetSensorAcceleration();
				if(sixAxisData != last)
				{
					n_printf("%f %f %f %f\n", sixAxisData.x(), sixAxisData.y(), sixAxisData.z(), gamePad->GetSensorAngularVelocity());
					last = sixAxisData;
				}
			}
#endif // if 0
#endif // ifdef __PS3__

#if 0
			// test rumble
			float v = gamePad->GetAxisValue(Input::GamePad::LeftTriggerAxis);
			gamePad->SetLowFrequencyVibrator(v);
			if(gamePad->ButtonPressed(Input::GamePad::CrossButton))
			{
				gamePad->SetHighFrequencyVibrator(1.0f);
			}
			else
			{
				gamePad->SetHighFrequencyVibrator(0.0f);
			}
#endif
		}

		inputServer->EndFrame();
	}

	inputServer->Close();
	inputServer = 0;
}