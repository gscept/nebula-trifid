#pragma once
//------------------------------------------------------------------------------
/**
    @class Vibration::VibrationHandler
    
    Message handler for the vibration subsystem. 
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "vibration/vibrationprotocol.h"
#include "timing/timer.h"

//------------------------------------------------------------------------------
namespace Vibration
{
class VibrationHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(VibrationHandler);
public:
    /// the vibration trigger interval in seconds
    static const Timing::Time VibrationTriggerInterval;

    /// constructor
    VibrationHandler();
    /// destructor
    virtual ~VibrationHandler();

    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

    /// do per-frame work 
    virtual void DoWork();

private:
    /// calculate current values
    bool CalculateValues();
    /// apply the values
    void ApplyValues();

    struct Vibrato 
    {
        float highFreqIntensity;
        Timing::Time highFreqAttack;
        Timing::Time highFreqSustain;
        Timing::Time highFreqRelease;

        float lowFreqIntensity;
        Timing::Time lowFreqAttack;
        Timing::Time lowFreqSustain;
        Timing::Time lowFreqRelease;

        Timing::Time startTime;
    };

    /// set the new vibration effect
    void OnAddEnvelopeVibration(const Ptr<AddEnvelopeVibration>& msg);
    /// shutdown the audio runtime
    void OnClearVibration(const Ptr<ClearVibration>& msg);
    
    Util::FixedArray<float> curHiFreqValues;                // current set hi freq vibrator value
    Util::FixedArray<float> curLowFreqValues;               // current set low freq vibrator value

    Util::FixedArray<Vibrato> targetVibratos;               // target structure
    Util::FixedArray<bool> vibratosRunnung;                 // flag if current vibrator is animated
    
    Timing::Timer timer;                                    // we need there an own timer, to prevent timefactor stuff
    Timing::Time lastTriggerTime;
    bool reseted;
};

} // namespace Vibration
//------------------------------------------------------------------------------