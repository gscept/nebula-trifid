//------------------------------------------------------------------------------
//  fxhandler.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxhandler.h"
#include "fxprotocol.h"
#include "fx/rt/fxserver.h"


namespace FX
{
__ImplementClass(FX::FxHandler, 'CFXH', Messaging::Handler);

//------------------------------------------------------------------------------
/**
*/
FxHandler::FxHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FxHandler::~FxHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
FxHandler::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	n_assert(msg.isvalid());

	// handle messages
	__StaticHandle(CreateCameraShakeEffect);
	__StaticHandle(CreateCameraAnimationEffect);
	__StaticHandle(CreateVibrationEffect);
	__StaticHandle(CreateAudioEffect);
	__StaticHandle(CreateGraphicsEffect);
	__StaticHandle(CreateCameraEffectMixer);

	// unknown message
	return false;
}

} // namespace FX






