//------------------------------------------------------------------------------
//  audioplayer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vibration/vibrationplayer.h"
#include "vibration/vibrationprotocol.h"
#include "vibration/vibrationinterface.h"

namespace Vibration
{
__ImplementClass(Vibration::VibrationPlayer, 'VIBP', Core::RefCounted);
__ImplementSingleton(Vibration::VibrationPlayer);

using namespace Math;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
VibrationPlayer::VibrationPlayer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
VibrationPlayer::~VibrationPlayer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationPlayer::AddEnvelopeVibration( IndexT playerIndex, 
                                       float hiFreq, 
                                       Timing::Time hiAttack, 
                                       Timing::Time hiSustain, 
                                       Timing::Time hiRelease, 
                                       float lowFreq, 
                                       Timing::Time lowAttack, 
                                       Timing::Time lowSustain, 
                                       Timing::Time lowRelease)
{
    Ptr<Vibration::AddEnvelopeVibration> msg = Vibration::AddEnvelopeVibration::Create();
    msg->SetPlayerIndex(playerIndex);
    msg->SetHighFreqAmount(hiFreq);
    msg->SetHighFreqAttack(hiAttack);
    msg->SetHighFreqSustain(hiSustain);
    msg->SetHighFreqRelease(hiRelease);
    msg->SetLowFreqAmount(lowFreq);
    msg->SetLowFreqAttack(lowAttack);
    msg->SetLowFreqSustain(lowSustain);
    msg->SetLowFreqRelease(lowRelease);
    Vibration::VibrationInterface::Instance()->Send(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
VibrationPlayer::ClearVibration(IndexT playerIndex, bool hiFreq /* = true */, bool lowFreq /* = true */)
{
    Ptr<Vibration::ClearVibration> msg = Vibration::ClearVibration::Create();
    msg->SetPlayerIndex(playerIndex);
    msg->SetHighFreq(hiFreq);
    msg->SetLowFreq(lowFreq);
    Vibration::VibrationInterface::Instance()->Send(msg.cast<Messaging::Message>());
}
} // namespace Vibration